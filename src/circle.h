#ifndef CIRCLE_H
#define CIRCLE_H

#include "bbox.h"

#include <iostream>

class Circle : public BBox {
public:
	// ctor and dtor
	Circle();
	Circle(const Vec&, double);
	~Circle();

	// dimensions and position
	double getH() const {return _h;};
	double getW() const {return _w;};
	double getR() const {return _r;};
	const Vec& getPos() const {return _pos;};
	void setPos(const Vec& newPos) {_pos = newPos;};

	// key points
	Vec center() const;
	Vec top() const;
	Vec topLeft() const;
	Vec left() const;
	Vec bottomLeft() const;
	Vec bottom() const;
	Vec bottomRight() const;
	Vec right() const;
	Vec topRight() const;

	// intersection operations
	BBox* shift(const Vec&) const;
	double distanceFrom(const BBox&) const;
	bool containsPoint(const Vec&) const;
	bool containsBBox(const BBox&) const;
	bool intersects(const BBox&) const;
	
	// debug
	void printAllPoints(std::ostream&) const;

protected:
	double _r; // circle radius
};



#endif
