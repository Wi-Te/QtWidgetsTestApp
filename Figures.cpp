#include "Figures.h"
#include <qxmlstream.h>
#include <qpainter.h>
#include <qpolygon.h>
#include <qpoint.h>
#include <qvariant.h>

fig::Base::Base(const QPoint& point)
    : ll(point.x()), tt(point.y()) {}

fig::Base::~Base() {}

void fig::Base::Complete(const QPoint& point) {
    ww = point.x() - ll;
    hh = point.y() - tt;
    valid = (ww > 5) && (hh > 5);
}

void fig::Base::MoveTo(const QPoint& point) {
    ll = point.x();
    tt = point.y();
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

QPoint fig::Base::getCenter() const {
    return QPoint{ll + qRound(ww / 2.0), tt + qRound(hh / 2.0)};
}

QPoint fig::Base::getCorner() const {
    return QPoint{ ll, tt };
}

void fig::Base::InnerToXML(QXmlStreamWriter& writer, const QString& ftype) const {
    writer.writeStartElement("figure");
    writer.writeAttribute("type", ftype);
        writer.writeStartElement("pos-size");
        writer.writeAttribute("left", QVariant{ ll }.toString());
        writer.writeAttribute("top", QVariant{ tt }.toString());
        writer.writeAttribute("right", QVariant{ ll + ww }.toString());
        writer.writeAttribute("bott", QVariant{ tt + hh }.toString());
        writer.writeEndElement();
    writer.writeEndElement();
}

//-----------------------RECT-------------------------//

fig::Rect::Rect(const QPoint& point)
    : Base(point) {};

void fig::Rect::InnerDraw(QPainter& painter) const {
    painter.drawRect(ll, tt, ww, hh);
}

void fig::Rect::toXML(QXmlStreamWriter& writer) const {
    InnerToXML(writer, "rect");
}

//-----------------------CIRCLE-------------------------//

fig::Circle::Circle(const QPoint& point)
    : Base(point) {};

void fig::Circle::InnerDraw(QPainter& painter) const {
    painter.drawEllipse(ll, tt, ww, hh);    
}

void fig::Circle::toXML(QXmlStreamWriter& writer) const {
    InnerToXML(writer, "circle");
}

//-----------------------TRIANGLE-------------------------//

fig::Triangle::Triangle(const QPoint& point)
    : Base(point), polygon(new QPolygon{ 3 }) {}

fig::Triangle::~Triangle() {
    delete polygon;
}

void fig::Triangle::calcPolygon(){
    polygon->setPoint(0, ll, tt + hh);
    polygon->setPoint(1, ll + qRound(ww / 2.0), tt);
    polygon->setPoint(2, ll + ww, tt + hh);
}

void fig::Triangle::Complete(const QPoint& point) {
    Base::Complete(point);
    if (valid) {
        calcPolygon();
    }
}

void fig::Triangle::MoveTo(const QPoint& point) {
    Base::MoveTo(point);
    calcPolygon();
}

void fig::Triangle::InnerDraw(QPainter& painter) const {
    painter.drawPolygon(*polygon);
}

void fig::Triangle::toXML(QXmlStreamWriter& writer) const {
    InnerToXML(writer, "triangle");
}