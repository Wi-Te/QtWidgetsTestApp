#include "Figures.h"
#include <qpainter.h>
#include <qpolygon.h>
#include <qpoint.h>

fig::Base::Base(const QPoint& point)
    : ll(point.x()), tt(point.y()) {}

fig::Base::~Base() {}

void fig::Base::Complete(const QPoint& point) {
    ww = point.x() - ll;
    hh = point.y() - tt;
    valid = (ww > 5) && (hh > 5);
}

bool fig::Base::Valid() const {
    return valid;
}

bool fig::Base::HitTest(const QPoint& point) const {
    return valid
        && ((unsigned)(point.x() - ll) <= ww)
        && ((unsigned)(point.y() - tt) <= hh);
}

void fig::Base::Draw(QPainter& painter) const {
    if (valid) {
        InnerDraw(painter);
    } else {
        painter.drawLine(ll - 3, tt - 3, ll + 3, tt + 3);
        painter.drawLine(ll - 3, tt + 3, ll + 3, tt - 3);
    }
}

fig::Rect::Rect(const QPoint& point)
    : Base(point) {};

void fig::Rect::InnerDraw(QPainter& painter) const {
    painter.drawRect(ll, tt, ww, hh);
}

fig::Circle::Circle(const QPoint& point)
    : Base(point) {};

void fig::Circle::InnerDraw(QPainter& painter) const {
    painter.drawEllipse(ll, tt, ww, hh);    
}

fig::Triangle::Triangle(const QPoint& point)
    : Base(point), polygon(new QPolygon{ 3 }) {}

fig::Triangle::~Triangle() {
    delete polygon;
}

void fig::Triangle::Complete(const QPoint& point) {
    Base::Complete(point);
    if (valid) {
        polygon->setPoint(0, ll, tt + hh);
        polygon->setPoint(1, ll + qRound(ww / 2.0), tt);
        polygon->setPoint(2, ll + ww, tt + hh);
    }
}

void fig::Triangle::InnerDraw(QPainter& painter) const {
    painter.drawPolygon(*polygon);
}