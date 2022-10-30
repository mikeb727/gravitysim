#include "rectangle.h"

Rectangle::Rectangle(){};
Rectangle::Rectangle(const Vec& p, double h, double w) : BBox(p, h, w) {}
Rectangle::~Rectangle() {}

Vec Rectangle::center() const {
	return _pos + Vec(_w / 2, _h / 2, true);
}
Vec Rectangle::top() const {
	return _pos + Vec(_w / 2, 0, true);
}
Vec  Rectangle::topLeft() const {
	return _pos;
}
Vec Rectangle::left() const {
	return _pos + Vec(0, _h / 2, true);
}
Vec Rectangle::bottomLeft() const {
	return _pos + Vec(0, _h, true);
}
Vec Rectangle::bottom() const {
	return _pos + Vec(_w / 2, _h, true);
}
Vec Rectangle::bottomRight() const {
	return _pos + Vec(_w, _h, true);
}
Vec Rectangle::right() const {
	return _pos + Vec(_w, _h / 2, true);
}
Vec Rectangle::topRight() const {
	return _pos + Vec(_w, 0, true);
}

BBox* Rectangle::shift(const Vec& d) const {
	return new Rectangle(_pos + d, _h, _w);
}

double Rectangle::distanceFrom(const BBox& b) const {
	return (center() - b.center()).mag();
}
bool Rectangle::containsPoint(const Vec& p) const {
	return (top().y() < p.y() &&
	        bottom().y() > p.y() &&
	        left().x() < p.x() &&
	        right().x() > p.x());
}
bool Rectangle::containsBBox(const BBox& b) const {
	return (containsPoint(b.top()) &&
	        containsPoint(b.topLeft()) &&
	        containsPoint(b.left()) &&
	        containsPoint(b.bottomLeft()) &&
	        containsPoint(b.bottom()) &&
	        containsPoint(b.bottomRight()) &&
	        containsPoint(b.right()) &&
	        containsPoint(b.topRight()));
}
bool Rectangle::intersects(const BBox& b) const {
	return (containsPoint(b.top()) ||
	        containsPoint(b.topLeft()) ||
	        containsPoint(b.left()) ||
	        containsPoint(b.bottomLeft()) ||
	        containsPoint(b.bottom()) ||
	        containsPoint(b.bottomRight()) ||
	        containsPoint(b.right()) ||
	        containsPoint(b.topRight()));
}

void Rectangle::draw(Window* win, SDL_Color c) const{
	win->drawRectangle(c, _pos.x(), _pos.y(), _w, _h);
}