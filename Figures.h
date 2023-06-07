#pragma once
class QPoint;
class QPainter;

namespace fig {
	class Base {
	protected:
		int ll, tt, ww{ 0 }, hh{ 0 };
	public:
		explicit Base(const QPoint&);
		virtual ~Base() = 0;

		void Complete(const QPoint&);
		virtual void Draw(QPainter*) const = 0;
	};

	class Rect : public Base {
	public:
		Rect(const QPoint&);
		void Draw(QPainter*) const;
	};

	class Circle : public Base {
	public:
		Circle(const QPoint&);
		void Draw(QPainter*) const;
	};

	class Triangle : public Base {
	public:
		Triangle(const QPoint&);
		void Draw(QPainter*) const;
	};
}