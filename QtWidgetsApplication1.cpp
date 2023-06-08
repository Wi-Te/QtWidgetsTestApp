#include "QtWidgetsApplication1.h"
#include "qpainter.h"
#include "Figures.h"

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
    connect(ui.RadioButtons,    SIGNAL(triggered(QAction*)),            this, SLOT(ToolChanged(QAction*)));

    connect(ui.centralWidget,   SIGNAL(SignalEsc()),                    this, SLOT(SlotEsc()));
    connect(ui.centralWidget,   SIGNAL(SignalStart(const QPoint&)),     this, SLOT(SlotStart(const QPoint&)));
    connect(ui.centralWidget,   SIGNAL(SignalDone(const QPoint&)),      this, SLOT(SlotDone(const QPoint&)));
    connect(ui.centralWidget,   SIGNAL(SignalUpdFigure(const QPoint&)), this, SLOT(SlotUpdFigure(const QPoint&)));
    connect(ui.centralWidget,   SIGNAL(SignalUpdImage(const QSize&)),   this, SLOT(SlotUpdImage(const QSize&)));

    allFigures.reserve(64);
    ui.centralWidget->assignPixmap(picture);
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
}

void QtWidgetsApplication1::SlotStart(const QPoint& point) {
    if (tempFigure) { 
        tempFigure.reset(); 
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
    {
        auto it = std::find_if(allFigures.begin(), allFigures.end(),
            [&](const auto& figure) {
                return figure != selectedFigure
                    && figure->HitTest(point);
            });        
        if (it == allFigures.end()) {
            selectedFigure.reset();
        } else {
            selectedFigure = *it;
        }
    }
        break;
    default: return;
    }

    ui.centralWidget->setMouseTracking(true);
    preparePict();
}

void QtWidgetsApplication1::SlotDone(const QPoint& point) {
    if (tempFigure) {        
        if (tempFigure->Valid()) {
            tempFigure->HitTest(QPoint{ 10, 20 });
            tempFigure->Complete(point);
            allFigures.push_back(std::move(tempFigure));
            renderBuffer();
            preparePict();
        } else {
            tempFigure.reset();
        }
    }
    ui.centralWidget->setMouseTracking(false);
}
void QtWidgetsApplication1::SlotEsc() {
    if (tempFigure) {
        tempFigure.reset();
        preparePict();
    }
    ui.centralWidget->setMouseTracking(false);
}

void QtWidgetsApplication1::SlotUpdFigure(const QPoint& point) {
    if (tempFigure) {
        tempFigure->Complete(point);
        preparePict();
    }
}
void QtWidgetsApplication1::SlotUpdImage(const QSize& newSize) {
    *picture = QPixmap{ newSize };
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
}

void QtWidgetsApplication1::preparePict() {
    QPainter painter{ picture.get() };
    //painter.drawImage(QPointF{ 0, 0 }, buffer);
    painter.drawPixmap(0, 0, buffer);
    if (tempFigure) {
        tempFigure->Draw(painter);
    }
    if (selectedFigure) {        
        QPen pen{};
        pen.setWidth(2);
        painter.setPen(pen);
        selectedFigure->Draw(painter);
    }

    ui.centralWidget->update();
}