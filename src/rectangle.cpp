#include "rectangle.h"

Rectangle::Rectangle(){};
Rectangle::Rectangle(const Vec2 &pos, double width, double height) : BBox(pos, width, height) {}
Rectangle::~Rectangle() {}

Vec2 Rectangle::center() const { return _pos + Vec2(_w / 2, _h / 2); }
Vec2 Rectangle::top() const { return _pos + Vec2(_w / 2, 0); }
Vec2 Rectangle::topLeft() const { return _pos; }
Vec2 Rectangle::left() const { return _pos + Vec2(0, _h / 2); }
Vec2 Rectangle::bottomLeft() const { return _pos + Vec2(0, _h); }
Vec2 Rectangle::bottom() const { return _pos + Vec2(_w / 2, _h); }
Vec2 Rectangle::bottomRight() const { return _pos + Vec2(_w, _h); }
Vec2 Rectangle::right() const { return _pos + Vec2(_w, _h / 2); }
Vec2 Rectangle::topRight() const { return _pos + Vec2(_w, 0); }

BBox *Rectangle::shift(const Vec2 &d) const {
  return new Rectangle(_pos + d, _h, _w);
}

double Rectangle::distanceFrom(const BBox &b) const {
  return (center() - b.center()).mag();
}
bool Rectangle::containsPoint(const Vec2 &p) const {
  return (top().y() < p.y() && bottom().y() > p.y() && left().x() < p.x() &&
          right().x() > p.x());
}
bool Rectangle::containsBBox(const BBox &b) const {
  return (containsPoint(b.top()) && containsPoint(b.topLeft()) &&
          containsPoint(b.left()) && containsPoint(b.bottomLeft()) &&
          containsPoint(b.bottom()) && containsPoint(b.bottomRight()) &&
          containsPoint(b.right()) && containsPoint(b.topRight()));
}
bool Rectangle::intersects(const BBox &b) const {
  return (containsPoint(b.top()) || containsPoint(b.topLeft()) ||
          containsPoint(b.left()) || containsPoint(b.bottomLeft()) ||
          containsPoint(b.bottom()) || containsPoint(b.bottomRight()) ||
          containsPoint(b.right()) || containsPoint(b.topRight()));
}
