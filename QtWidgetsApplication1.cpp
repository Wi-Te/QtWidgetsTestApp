#include "QtWidgetsApplication1.h"
#include <qxmlstream.h>
#include <QtXml\qdom.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qpainter>
#include "Figures.h"
#include "ALink.h"

enum class QtWidgetsApplication1::Tools {
    None,
    Triangle,
    Circle,
    Rect,
    Link,
    Move,
    Trash
};

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent) 
    : QMainWindow(parent), selectedTool(Tools::None) {
    ui.setupUi(this);

    ui.RadioButtons->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
    connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(SlotSaveToFile()));
    connect(ui.actionLoad, SIGNAL(triggered()), this, SLOT(SlotLoadFromFile()));
    connect(ui.RadioButtons, SIGNAL(triggered(QAction*)), this, SLOT(ToolChanged(QAction*)));

    connect(ui.centralWidget,   SIGNAL(SignalEsc()),                    this, SLOT(SlotEsc()));
    connect(ui.centralWidget,   SIGNAL(SignalStart(const QPoint&)),     this, SLOT(SlotStart(const QPoint&)));
    connect(ui.centralWidget,   SIGNAL(SignalDone(const QPoint&)),      this, SLOT(SlotDone(const QPoint&)));
    connect(ui.centralWidget,   SIGNAL(SignalUpdFigure(const QPoint&)), this, SLOT(SlotUpdFigure(const QPoint&)));
    connect(ui.centralWidget,   SIGNAL(SignalUpdImage(const QSize&)),   this, SLOT(SlotUpdImage(const QSize&)));

    allLinks.reserve(16);
    allFigures.reserve(64);
    selectedLinks.reserve(8);
    ui.centralWidget->assignPixmap(&picture);
}

QtWidgetsApplication1::~QtWidgetsApplication1() {}

void QtWidgetsApplication1::ToolChanged(QAction* action) {
    if (action && action->isChecked()) {
        if (action == ui.actionCircle) {
            selectedTool = Tools::Circle;
        } else if (action == ui.actionTriangle) {
            selectedTool = Tools::Triangle;
        } else if (action == ui.actionRect) {
            selectedTool = Tools::Rect;
        } else if (action == ui.actionLink) {
            selectedTool = Tools::Link;
        } else if (action == ui.actionMove) {
            selectedTool = Tools::Move;
        } else if (action == ui.actionTrash) {
            selectedTool = Tools::Trash;
        }
    } else {
        selectedTool = Tools::None;
    }

    SlotEsc();
    if (hovered) {
        hovered.reset();
    }

    switch (selectedTool) {
    case Tools::Link:
    case Tools::Move:
    case Tools::Trash:
        hover = true;
        break;
    default:
        hover = false;
    }

    ui.centralWidget->setMouseTracking(hover);
}

void QtWidgetsApplication1::SlotEsc() {
    if (moving && selected) {
        moveSelectedFigureTo(originalPos);
        renderBuffer();
    }
    if (!selectedLinks.empty()) {
        selectedLinks.clear();
    }
    if (tempFigure) {
        tempFigure.reset();
    }
    if (tempLink) {
        tempLink.reset();
    }
    if (selected) {
        selected.reset();
    }

    moving = false;
    ui.centralWidget->setCursor(Qt::ArrowCursor);
    preparePict();
}

//returns true if value was changed
bool QtWidgetsApplication1::setFigureAt(std::shared_ptr<fig::Base>& fref, const QPoint& point){
    if (fref && fref->HitTest(point)) {
        return false;   
    } else {
        auto it = std::find_if(allFigures.begin(), allFigures.end(),
            [&point](const auto& figure) {
                return figure->HitTest(point);
            });
        if (it == allFigures.end()) {
            fref.reset();
        } else {
            fref = *it;
        }
        return true;
    }
}

void QtWidgetsApplication1::SlotStart(const QPoint& point) {
    if (tempFigure) { 
        tempFigure.reset(); 
    }
    if (tempLink) {
        tempLink.reset();
    }

    switch (selectedTool) {
    case Tools::Rect:
        tempFigure = std::make_unique<fig::Rect>(point);
        break;
    case Tools::Circle:
        tempFigure = std::make_unique<fig::Circle>(point);
        break;
    case Tools::Triangle:
        tempFigure = std::make_unique<fig::Triangle>(point);
        break;
    case Tools::Link:
        setFigureAt(selected, point);
        if (selected) {
            tempLink = std::make_unique<fig::ALink>(selected);
            tempLink->TempLineTo(point);
        }
        break;
    case Tools::Move:
        setFigureAt(selected, point);
        if (selected) {
            ui.centralWidget->setCursor(Qt::OpenHandCursor);

            buffer.fill(Qt::white);
            QPainter painter{ &buffer };
            painter.setBrush(Qt::transparent);
            for (const auto& figure : allFigures) {
                if (figure != selected) {
                    figure->Draw(painter);
                }
            }
            if (!selectedLinks.empty()) {
                selectedLinks.clear();
            }
            for (const auto& link : allLinks) {                
                if (link->connectedTo(selected)) {
                    selectedLinks.emplace_back(link);
                } else {
                    link->Draw(painter);
                }
            }            

            originalPos = selected->getCorner();
            moveOffset = point - originalPos;
            moving = true;
        }
        break;
    case Tools::Trash:
        setFigureAt(hovered, point);
        if (hovered) {
            allLinks.erase(std::remove_if(allLinks.begin(), allLinks.end(),
                [this](const auto& link) {
                    return link->connectedTo(hovered);
                }), allLinks.end());
            allFigures.erase(std::remove(allFigures.begin(), allFigures.end(), hovered), allFigures.end());
            hovered.reset();
            renderBuffer();
            preparePict();
        }
        break;
    }
}

void QtWidgetsApplication1::moveSelectedFigureTo(const QPoint& point) {
    selected->MoveTo(point);
    for (const auto& link : selectedLinks) {
        link->FigureMoved(selected);
    }
}

void QtWidgetsApplication1::SlotUpdFigure(const QPoint& point) {
    bool redraw = false;

    if (moving && selected) {
        moveSelectedFigureTo(point - moveOffset);
        redraw = true;
    } else if (hover) {
        redraw |= setFigureAt(hovered, point);
    }
    if (tempFigure) {
        tempFigure->Complete(point);
        redraw = true;
    }
    if (tempLink) {
        tempLink->TempLineTo(point);
        redraw = true;
    }

    if (redraw) {
        preparePict();
    }
}

void QtWidgetsApplication1::SlotDone(const QPoint& point) {
    if (tempFigure) {        
        if (tempFigure->Valid()) {
            tempFigure->Complete(point);
            allFigures.push_back(std::move(tempFigure));
            renderBuffer();
        } else {
            tempFigure.reset();
        }
    }

    if (tempLink) {
        setFigureAt(hovered, point);
        if (selected && hovered && selected != hovered) {
            if (std::none_of(allLinks.begin(), allLinks.end(),
            [this](const auto& link) {
                return link->connectedTo(selected) && link->connectedTo(hovered);
            })) {
                tempLink->Complete(hovered);
                if (tempLink->Valid()) {
                    allLinks.push_back(std::move(tempLink));
                    renderBuffer();
                }                             
            }
        }
    }

    if (moving && selected) {
        moveSelectedFigureTo(point - moveOffset);        
        renderBuffer();
    }
    if (!selectedLinks.empty()) {
        selectedLinks.clear();
    }

    if (tempLink) {
        tempLink.reset();
    }
    if (selected) {
        selected.reset();
    }

    moving = false;
    ui.centralWidget->setCursor(Qt::ArrowCursor);
    preparePict();
}

void QtWidgetsApplication1::SlotUpdImage(const QSize& newSize) {
    picture = QPixmap{ newSize };
    buffer = QPixmap{ newSize };
    //buffer = QImage{ newSize, QImage::Format_RGB32 };

    renderBuffer();
    preparePict();
}

void QtWidgetsApplication1::renderBuffer() {
    buffer.fill(Qt::white);
    QPainter painter{ &buffer };
    painter.setBrush(Qt::transparent);
    for (const auto& figure : allFigures) {
        figure->Draw(painter);
    }
    for (const auto& link : allLinks) {
        link->Draw(painter);
    }
}

void QtWidgetsApplication1::preparePict() {
    QPainter painter{ &picture };
    painter.drawPixmap(0, 0, buffer);
    if (tempFigure) {
        tempFigure->Draw(painter);
    }
    if (tempLink) {
        tempLink->Draw(painter);
    }
    for (const auto& link : selectedLinks) {
        link->Draw(painter);
    }
    if (selected || hovered) {        
        QPen pen{};
        pen.setWidth(2);
        painter.setPen(pen);

        if (selected) {
            selected->Draw(painter);
        }
        if (hovered && hovered != selected) {
            hovered->Draw(painter);
        }
    }
    ui.centralWidget->update();
}

void QtWidgetsApplication1::SlotSaveToFile() {
    selectedTool = Tools::None;
    SlotEsc();
    hover = false;
    if (hovered) {
        hovered.reset();
    }
    ui.centralWidget->setMouseTracking(false);

    auto filename = QFileDialog::getSaveFileName(this, "Save figures As...", "", "XML Files (*.xml)", nullptr);
    if (filename.isEmpty()) {
        return;
    }
    QFile xfile{ filename };
    if (!xfile.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate | QIODeviceBase::Text)) {
        QMessageBox::warning(this, "Error saving file", "Failed to save into file [" + filename + "]: " + xfile.errorString());
        return;
    }
    QXmlStreamWriter writer{ &xfile };
    writer.setAutoFormatting(true);
    writer.writeStartDocument();

    writer.writeStartElement("figures");
    for (const auto& fig : allFigures) {
        fig->toXML(writer);
    }    
    writer.writeEndElement();

    writer.writeEndDocument();
    xfile.close();

    QMessageBox::information(this, "Saved", "Saved succesfully");
}

void QtWidgetsApplication1::SlotLoadFromFile() {
    selectedTool = Tools::None;
    SlotEsc();
    hover = false;
    if (hovered) {
        hovered.reset();
    }
    ui.centralWidget->setMouseTracking(false);

    auto filename = QFileDialog::getOpenFileName(this, "Load figures from...", "", "XML files (*.xml)", nullptr);
    if (filename.isEmpty()) {
        return;
    }
    QFile xfile{ filename };
    if (!xfile.open(QIODeviceBase::ExistingOnly | QIODeviceBase::ReadOnly | QIODeviceBase::Text)) {
        QMessageBox::warning(this, "Error loading file", "Failed to load from file [" + filename + "]: " + xfile.errorString());
        return;
    }

    QDomDocument doc{};
    if (!doc.setContent(&xfile)) {
        QMessageBox::warning(this, "Error parsing file", "Failed reading data from file [" + filename + "]");
        xfile.close();
        return;
    }
    xfile.close();

    if (!allLinks.empty()) {
        allLinks.clear();
    }
    if (!allFigures.empty()) {
        allFigures.clear();
    }
    /*
    auto figures = doc.elementsByTagName("figure");
    for (auto i = 0; i < figures.count(); i++) {
        auto node = figures.at(i);
        node.attributes("type")
    }
    */
    renderBuffer();
    preparePict();
}