#include "Figures.h"
#include <qpainter.h>
#include <qpoint.h>

fig::Base::Base(const QPoint& point)
    : ll(point.x()), tt(point.y()) {}

fig::Base::~Base() {}

void fig::Base::Complete(const QPoint& point) {
    ww = point.x() - ll;
    hh = point.y() - tt;
}

fig::Rect::Rect(const QPoint& point)
    : Base(point) {};

void fig::Rect::Draw(QPainter* painter) const {
    if (painter && hh && ww) {
        painter->drawRect(ll, tt, ww, hh);
    }
}

fig::Circle::Circle(const QPoint& point)
    : Base(point) {};

void fig::Circle::Draw(QPainter* painter) const {
    if (painter && hh && ww) {
        painter->drawEllipse(ll, tt, ww, hh);
    }
}

fig::Triangle::Triangle(const QPoint& point)
    : Base(point) {};

void fig::Triangle::Draw(QPainter* painter) const {
    if (painter && hh && ww) {
        QList<QPoint> pairs{};
        pairs.reserve(3);
        pairs.emplace_back(ll, tt+hh);
        pairs.emplace_back(ll + qRound(ww/2.0), tt);
        pairs.emplace_back(ll + ww, tt + hh);
        painter->drawPolygon(QPolygon{ std::move(pairs) });
    }
}