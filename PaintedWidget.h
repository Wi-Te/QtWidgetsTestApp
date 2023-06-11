#pragma once
#include <qwidget.h>

class PaintedWidget : public QWidget {
    Q_OBJECT

public:
    explicit PaintedWidget(QWidget* parent = 0);
    ~PaintedWidget();

    void assignPixmap(QPixmap*);

protected:
    void keyPressEvent(QKeyEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent*);

private:
    QPixmap* pixmap{ nullptr };

signals:
    void SignalStart(const QPoint&);
    void SignalDone(const QPoint&);
    void SignalEsc();
    void SignalUpdFigure(const QPoint&);
    void SignalUpdImage(const QSize&);
};

