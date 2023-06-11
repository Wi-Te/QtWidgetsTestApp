#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"

namespace fig {
    class Base;
    class ALink;
}
class QtWidgetsApplication1 : public QMainWindow {
    Q_OBJECT

public:
    explicit QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();

private:
    enum class Tools;
    Tools selectedTool;

    QPixmap buffer{};
    QPixmap picture{};

    QPoint moveOffset{};
    QPoint originalPos{};

    std::unique_ptr<fig::Base> tempFigure{ nullptr };
    std::vector<std::shared_ptr<fig::Base>> allFigures{};

    std::unique_ptr<fig::ALink> tempLink{ nullptr };
    std::vector<std::shared_ptr<fig::ALink>> allLinks{};
    std::vector<std::shared_ptr<fig::ALink>> selectedLinks{};

    void moveSelectedFigureTo(const QPoint&);

    bool hover{ false };
    bool moving{ false };
    std::shared_ptr<fig::Base> hovered{ nullptr };
    std::shared_ptr<fig::Base> selected{ nullptr };
    bool setFigureAt(std::shared_ptr<fig::Base>&, const QPoint&);

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

    void SlotSaveToFile();
    void SlotLoadFromFile();
};
