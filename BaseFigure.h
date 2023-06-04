#pragma once
class QPainter;
class BaseFigure {
protected:	
	int ll, tt, ww, hh;
public:
	explicit BaseFigure();
	explicit BaseFigure(int left, int top);
	~BaseFigure();

	void Complete(int right, int bott);
	void Draw(QPainter* painter) const;
};