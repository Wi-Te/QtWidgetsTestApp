#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"

namespace fig {
    class Base;
}
class QtWidgetsApplication1 : public QMainWindow {
    Q_OBJECT

public:
    explicit QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();

private:
    enum class Tools;
    Tools selectedTool;

    //QImage buffer{};
    QPixmap buffer{};
    std::shared_ptr<QPixmap> picture{ std::make_unique<QPixmap>() };
    std::unique_ptr<fig::Base> tempFigure{ nullptr };
    std::vector<std::shared_ptr<fig::Base>> allFigures{};
    std::shared_ptr<fig::Base> selectedFigure{ nullptr };

    void renderBuffer();
    void preparePict();

    Ui::QtWidgetsApplication1Class ui;

private slots:
    void ToolChanged(QAction*);

    void SlotEsc();
    void SlotStart(const QPoint&);
    void SlotDone(const QPoint&);
    void SlotUpdFigure(const QPoint&);
    void SlotUpdImage(const QSize&);
};
