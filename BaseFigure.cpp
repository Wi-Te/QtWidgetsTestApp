#include "BaseFigure.h"
#include <qpainter.h>

BaseFigure::BaseFigure(int left, int top) : ll(left), tt(top), ww(0), hh(0) {
	qDebug("BaseFigure: xy constructor");
}
BaseFigure::BaseFigure() : BaseFigure(0, 0) {
	qDebug("BaseFigure: default constructor");
}
BaseFigure::~BaseFigure() {
	qDebug("BaseFigure: destructor");
};

void BaseFigure::Complete(int right, int bott) {
	this->ww = right - this->ll;
	this->hh = bott - this->tt;
}

void BaseFigure::Draw(QPainter* painter) const {
	if (this->ww && this->hh && painter) {
		painter->drawRect(this->ll, this->tt, this->ww, this->hh);
	}
}