#include "QtWidgetsApplication1.h"
#include "BaseFigure.h"
#include "qpainter.h"

enum class QtWidgetsApplication1::Tools {
    None,
    Triangle,
    Circle,
    Rect,
    Link,
    Move,
    Trash
};

enum class QtWidgetsApplication1::State {
    Idle,
    Drawing
};

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent) 
    : QMainWindow(parent), selectedTool(Tools::None), currentState(State::Idle) {
    ui.setupUi(this);

    ui.RadioButtons->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
    connect(ui.RadioButtons, SIGNAL(triggered(QAction*)), this, SLOT(ToolChanged(QAction*)));

    connect(ui.centralWidget, SIGNAL(SignalStart(const QPointF&)), this, SLOT(SlotStart(const QPointF&)));
    connect(ui.centralWidget, SIGNAL(SignalDone(const QPointF&)), this, SLOT(SlotDone(const QPointF&)));
    connect(ui.centralWidget, SIGNAL(SignalUpdFigure(const QPointF&)), this, SLOT(SlotUpdFigure(const QPointF&)));
    connect(ui.centralWidget, SIGNAL(SignalUpdImage(const QSize&)), this, SLOT(SlotUpdImage(const QSize&)));
    connect(ui.centralWidget, SIGNAL(SignalEsc()), this, SLOT(SlotEsc()));

    allFigures.reserve(64);
    ui.centralWidget->assignPixmap(&image);
}

QtWidgetsApplication1::~QtWidgetsApplication1() {}

void QtWidgetsApplication1::ToolChanged(QAction* action) {
    if (action && action->isChecked()) {
        if (action == ui.actionCircle) {
            this->selectedTool = Tools::Circle;
        } else if (action == ui.actionTriangle) {
            this->selectedTool = Tools::Triangle;
        } else if (action == ui.actionRect) {
            this->selectedTool = Tools::Rect;
        } else if (action == ui.actionLink) {
            this->selectedTool = Tools::Link;
        } else if (action == ui.actionMove) {
            this->selectedTool = Tools::Move;
        } else if (action == ui.actionTrash) {
            this->selectedTool = Tools::Trash;
        }
    } else {
        this->selectedTool = Tools::None;
    }
}

void QtWidgetsApplication1::SlotStart(const QPointF& point) {
    if ((State::Idle == this->currentState) && (Tools::Rect == this->selectedTool)) {
        this->currentState = State::Drawing;
        tempFigure = std::make_unique<BaseFigure>(point.x(), point.y());
        prepareImage();
    }
}

void QtWidgetsApplication1::SlotDone(const QPointF& point) {
    if ((State::Drawing == this->currentState) && (tempFigure) && (Tools::Rect == this->selectedTool)) {
        this->currentState = State::Idle;
        tempFigure->Complete(point.x(), point.y());
        allFigures.push_back(std::move(*tempFigure.release()));
        renderBuffer();
        prepareImage();
    }
}
void QtWidgetsApplication1::SlotEsc() {
    this->currentState = State::Idle;
    if (tempFigure) {
        tempFigure.reset(nullptr);
        prepareImage();
    }
}

void QtWidgetsApplication1::SlotUpdFigure(const QPointF& point) {
    if ((State::Drawing == this->currentState) && (tempFigure) && (Tools::Rect == this->selectedTool)) {
        tempFigure->Complete(point.x(), point.y());
        prepareImage();
    }
}
void QtWidgetsApplication1::SlotUpdImage(const QSize& newSize) {
    image = QPixmap{ newSize };
    buffer = QPixmap{ newSize };
    renderBuffer();
    prepareImage();
}

void QtWidgetsApplication1::renderBuffer() {
    buffer.fill(Qt::white);
    QPainter painter{ &buffer };
    for (const auto& figure : allFigures) {
        figure.Draw(&painter);
    }
}

void QtWidgetsApplication1::prepareImage() {
    if (tempFigure) {
        QPainter painter{ &image };
        painter.drawPixmap(0, 0, buffer);
        tempFigure->Draw(&painter);
    } else {
        image = buffer;
    }

    ui.centralWidget->update();
}