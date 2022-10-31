#ifndef BBOX_H
#define BBOX_H

#include "2DVec.h"

class BBox {
public:
	virtual ~BBox() = 0;
	virtual double getH() const = 0;
	virtual double getW() const = 0;
	virtual const Vec& getPos() const = 0;
	virtual void setPos(const Vec& newPos) = 0;

	virtual Vec center() const = 0; // return position of center
	virtual Vec top() const = 0; // return position of top point
	virtual Vec topLeft() const = 0; // return position of top-left corner
	virtual Vec left() const = 0; // return position of left point
	virtual Vec bottomLeft() const = 0; // return position of bottom-left corner
	virtual Vec bottom() const = 0; // return position of bottom point
	virtual Vec bottomRight() const = 0; // return position of bottom-right corner
	virtual Vec right() const = 0; // return position of right point
	virtual Vec topRight() const = 0; // return position of top-right corner

	virtual BBox* shift(const Vec& offset) const = 0; 
	virtual double distanceFrom(const BBox& otherBBox) const = 0;
	virtual bool containsPoint(const Vec& pos) const = 0;
	virtual bool intersects(const BBox& otherBBox) const = 0;
	virtual bool containsBBox(const BBox& otherBBox) const = 0;
	
protected:
	BBox();
	BBox(const Vec& pos, double height, double width);
	double _h; // bounding box height
	double _w; // bounding box width
	Vec _pos; // bounding box position
};


#endif
