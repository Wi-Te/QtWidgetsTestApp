#include "PaintedWidget.h"
#include "qpainter.h"
#include "qevent.h"

PaintedWidget::PaintedWidget(QWidget* parent) : QWidget(parent) {}
PaintedWidget::~PaintedWidget() {}

void PaintedWidget::assignPixmap(QPixmap* ptr){
	this->pixmap = ptr;
}

void PaintedWidget::keyPressEvent(QKeyEvent* event) {	
	if (Qt::Key::Key_Escape == event->key()) {
		emit SignalEsc();
	}
	QWidget::keyPressEvent(event);
}

void PaintedWidget::mousePressEvent(QMouseEvent* event) {
	if (event) {
		switch (event->button()) {
		case Qt::MouseButton::LeftButton:
			emit SignalStart(event->position());
			break;
		case Qt::MouseButton::RightButton:
			emit SignalEsc();
			break;
		}
	}
}

void PaintedWidget::mouseMoveEvent(QMouseEvent* event) {
	if (event) {
		emit SignalUpdFigure(event->position());
	}
}

void PaintedWidget::mouseReleaseEvent(QMouseEvent* event) {
	if (event && Qt::MouseButton::LeftButton == event->button()) {		
		emit SignalDone(event->position());
	}
}

void PaintedWidget::resizeEvent(QResizeEvent* event) {
	if (event) {
		emit SignalUpdImage(event->size());
	}
}

void PaintedWidget::paintEvent(QPaintEvent* event) {
	if (event) {
		auto rect = event->rect();
		auto painter = QPainter{ this };
		painter.fillRect(rect, Qt::white);
		if (pixmap) {
			painter.drawPixmap(rect, *pixmap, rect);
		}
	}
}