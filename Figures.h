#pragma once
#include <vector>
#include <memory>

class QPoint;
class QPainter;
class QPolygon;
class QString;
class QXmlStreamWriter;

namespace fig {
	class ALink;

	class Base {
	protected:
		std::vector<std::shared_ptr<fig::ALink>> links{};
		int ll, tt, ww{ 0 }, hh{ 0 };
		bool valid{ false };

		virtual void InnerDraw(QPainter&) const = 0;
		void InnerToXML(QXmlStreamWriter&, const QString&) const;
	public:
		explicit Base(const QPoint&);
		virtual ~Base();

		virtual void MoveTo(const QPoint&);
		virtual void Complete(const QPoint&);		
		void Draw(QPainter&) const;
		bool HitTest(const QPoint&) const;
		bool Valid() const;

		QPoint getCenter() const;
		QPoint getCorner() const;

		virtual void toXML(QXmlStreamWriter&) const = 0;
	};

	class Rect : public Base {
	private:
		void InnerDraw(QPainter&) const override;
	public:
		Rect(const QPoint&);
		void toXML(QXmlStreamWriter&) const override;
	};

	class Circle : public Base {
	private:
		void InnerDraw(QPainter&) const override;
	public:
		Circle(const QPoint&);
		void toXML(QXmlStreamWriter&) const override;
	};

	class Triangle : public Base {
	private:
		QPolygon* polygon;
		void calcPolygon();
		void InnerDraw(QPainter&) const override;
	public:
		Triangle(const QPoint&);
		~Triangle() override;

		void MoveTo(const QPoint&) override;
		void Complete(const QPoint&) override;
		void toXML(QXmlStreamWriter&) const override;
	};
}