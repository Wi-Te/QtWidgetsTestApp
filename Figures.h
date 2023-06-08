#pragma once
class QPoint;
class QPainter;
class QPolygon;

namespace fig {
	class Base {
	protected:
		int ll, tt, ww{ 0 }, hh{ 0 };
		bool valid{ false };

		virtual void InnerDraw(QPainter&) const = 0;
	public:
		explicit Base(const QPoint&);
		virtual ~Base() = 0;

		virtual void Complete(const QPoint&);
		void Draw(QPainter&) const;
		bool HitTest(const QPoint&) const;
		bool Valid() const;
	};

	class Rect : public Base {
	public:
		Rect(const QPoint&);
		void InnerDraw(QPainter&) const;
	};

	class Circle : public Base {
	public:
		Circle(const QPoint&);
		void InnerDraw(QPainter&) const;
	};

	class Triangle : public Base {
	private:
		QPolygon* polygon;
	public:
		Triangle(const QPoint&);
		~Triangle();

		void Complete(const QPoint&);
		void InnerDraw(QPainter&) const;
	};
}