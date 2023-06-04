#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"

class BaseFigure;
class QtWidgetsApplication1 : public QMainWindow {
    Q_OBJECT

public:
    explicit QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();

private:
    enum class Tools;
    Tools selectedTool;

    enum class State;
    State currentState;

    QPixmap buffer{};
    QPixmap image{};

    std::vector<BaseFigure> allFigures{};
    std::unique_ptr<BaseFigure> tempFigure{ nullptr };

    void renderBuffer();
    void prepareImage();

    Ui::QtWidgetsApplication1Class ui;

private slots:
    void ToolChanged(QAction*);

    void SlotEsc();
    void SlotStart(const QPointF&);
    void SlotDone(const QPointF&);
    void SlotUpdFigure(const QPointF&);
    void SlotUpdImage(const QSize&);
};
