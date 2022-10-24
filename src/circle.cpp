#include "circle.h"

const double PI = 3.1415927;

using namespace std;

Circle::Circle() : _r (0) {}
Circle::Circle(const Vec& p, double r) : BBox(p, 2 * r, 2 * r), _r (r) {}
Circle::~Circle() {}


Vec Circle::center() const {
	return _pos;
}
Vec Circle::top() const {
	return _pos + Vec(_r, PI / 2, false);
}
Vec Circle::topLeft() const {
	return _pos + Vec(_r, 3 * PI / 4, false);
}
Vec Circle::left() const {
	return _pos + Vec(_r, PI, false);
}
Vec Circle::bottomLeft() const {
	return _pos + Vec(_r, 5 * PI / 4, false);
}
Vec Circle::bottom() const {
	return _pos + Vec(_r, 3 * PI / 2, false);
}
Vec Circle::bottomRight() const {
	return _pos + Vec(_r, 7 * PI / 4, false);
}
Vec Circle::right() const {
	return _pos + Vec(_r, 0, false);
}
Vec Circle::topRight() const {
	return _pos + Vec(_r, PI / 4, false);
}

BBox* Circle::shift(const Vec& d) const {
	return new Circle(_pos + d, _r);
}

double Circle::distanceFrom(const BBox& b) const {
	return (center() - b.center()).mag();
}
bool Circle::containsPoint(const Vec& p) const {
	Vec distance = _pos - p;
	return distance.mag() < _r;
}
bool Circle::containsBBox(const BBox& b) const {
	return (containsPoint(b.top()) &&
	        containsPoint(b.topLeft()) &&
	        containsPoint(b.left()) &&
	        containsPoint(b.bottomLeft()) &&
	        containsPoint(b.bottom()) &&
	        containsPoint(b.bottomRight()) &&
	        containsPoint(b.right()) &&
	        containsPoint(b.topRight()));
}
bool Circle::intersects(const BBox& b) const {
	return (containsPoint(b.top()) ||
	        containsPoint(b.topLeft()) ||
	        containsPoint(b.left()) ||
	        containsPoint(b.bottomLeft()) ||
	        containsPoint(b.bottom()) ||
	        containsPoint(b.bottomRight()) ||
	        containsPoint(b.right()) ||
	        containsPoint(b.topRight()));
}
void Circle::printAllPoints(std::ostream& out) const {
	out << top() << endl
	        << topLeft() << endl
	        << left() << endl
	        << bottomLeft() << endl
	        << bottom() << endl
	        << bottomRight() << endl
	        << right() << endl
	        << topRight() << endl;
}

void Circle::draw(Window* win, SDL_Color c) const {
	win->drawCircleGradient(c, ImageTools::blend(c, 0.6, Colors::white, 0.4), _pos.x(), _pos.y(), _r);

}
