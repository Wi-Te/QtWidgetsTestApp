#include "ALink.h"
#include "Figures.h"
#include <qpoint>
#include <qpainter>

fig::ALink::ALink(std::shared_ptr<fig::Base> figure) {
	fig1 = std::move(figure);
	if (fig1) {
		lt = fig1->getCenter();
	}
}

void fig::ALink::Complete(std::shared_ptr<fig::Base> figure) {
	fig2 = std::move(figure);
	if (fig2) {
		rb = fig2->getCenter();
	}
	valid = fig1 && fig2 && fig1 != fig2;
}

void fig::ALink::TempLineTo(const QPoint& point) {
	rb = point;
}

void fig::ALink::FigureMoved(const std::shared_ptr<fig::Base>& figure) {
	if (figure == fig1) {
		lt = fig1->getCenter();
	} else if (figure == fig2) {
		rb = fig2->getCenter();
	}
}

void fig::ALink::Draw(QPainter& painter) const {
	painter.drawLine(lt, rb);
}

bool fig::ALink::connectedTo(const std::shared_ptr<fig::Base>& figure) const {
	return figure == fig1 || figure == fig2;
}

bool fig::ALink::Valid() const {
	return valid;
}