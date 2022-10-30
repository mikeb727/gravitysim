#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "bbox.h"

class Rectangle : public BBox {
public:
	Rectangle();
	Rectangle(const Vec&, double, double);
	~Rectangle();
	
	double getH() const {return _h;};
	double getW() const {return _w;};
	const Vec& getPos() const {return _pos;};
	void setPos(const Vec& newPos) {_pos = newPos;};
	
	Vec center() const;
	Vec top() const;
	Vec topLeft() const;
	Vec left() const;
	Vec bottomLeft() const;
	Vec bottom() const;
	Vec bottomRight() const;
	Vec right() const;
	Vec topRight() const;

	BBox* shift(const Vec&) const;
	double distanceFrom(const BBox&) const;
	bool containsPoint(const Vec&) const;
	bool containsBBox(const BBox&) const;
	bool intersects(const BBox&) const;

	void draw(Window* win, SDL_Color c) const;
};


#endif