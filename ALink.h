#pragma once
#include <memory>
#include <qpoint>

class QPainter;
class QXmlStreamWriter;
namespace fig {
	class Base;

	class ALink {
	private:
		std::shared_ptr<fig::Base> fig1{ nullptr };
		std::shared_ptr<fig::Base> fig2{ nullptr };

		QPoint lt{}, rb{};
		bool valid{ false };
	public:
		ALink(std::shared_ptr<fig::Base>);
		void Complete(std::shared_ptr<fig::Base>);
		void TempLineTo(const QPoint&);
		void FigureMoved(const std::shared_ptr<fig::Base>&);

		void Draw(QPainter&) const;
		void toXML(QXmlStreamWriter&) const;

		bool connectedTo(const std::shared_ptr<fig::Base>&) const;
		bool Valid() const;
	};
}