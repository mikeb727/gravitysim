#include "circle.h"

const double PI = 3.1415927;

Circle::Circle() : _r(0) {}
Circle::Circle(const Vec2 &p, double r) : BBox(p, 2 * r, 2 * r, 0), _r(r) {}
Circle::~Circle() {}

Vec2 Circle::center() const { return _pos; }
Vec2 Circle::top() const { return _pos + Vec2(_r, PI / 2, MagDir); }
Vec2 Circle::topLeft() const { return _pos + Vec2(_r, 3 * PI / 4, MagDir); }
Vec2 Circle::left() const { return _pos + Vec2(_r, PI, MagDir); }
Vec2 Circle::bottomLeft() const { return _pos + Vec2(_r, 5 * PI / 4, MagDir); }
Vec2 Circle::bottom() const { return _pos + Vec2(_r, 3 * PI / 2, MagDir); }
Vec2 Circle::bottomRight() const { return _pos + Vec2(_r, 7 * PI / 4, MagDir); }
Vec2 Circle::right() const { return _pos + Vec2(_r, 0, MagDir); }
Vec2 Circle::topRight() const { return _pos + Vec2(_r, PI / 4, MagDir); }

BBox *Circle::shift(const Vec2 &offset) const { return new Circle(_pos + offset, _r); }

double Circle::distanceFrom(const BBox &b) const {
  return (center() - b.center()).mag();
}
bool Circle::containsPoint(const Vec2 &p) const {
  Vec2 distance = _pos - p;
  return distance.mag() < _r;
}
bool Circle::containsBBox(const BBox &b) const {
  return (containsPoint(b.top()) && containsPoint(b.topLeft()) &&
          containsPoint(b.left()) && containsPoint(b.bottomLeft()) &&
          containsPoint(b.bottom()) && containsPoint(b.bottomRight()) &&
          containsPoint(b.right()) && containsPoint(b.topRight()));
}
bool Circle::intersects(const BBox &b) const {
  return (containsPoint(b.top()) || containsPoint(b.topLeft()) ||
          containsPoint(b.left()) || containsPoint(b.bottomLeft()) ||
          containsPoint(b.bottom()) || containsPoint(b.bottomRight()) ||
          containsPoint(b.right()) || containsPoint(b.topRight()));
}