#include "circle.h"

const double PI = 3.1415927;

Circle::Circle() : _r(0) {}
Circle::Circle(const Vec3 &p, double r) : BBox(p, 2 * r, 2 * r, 2 * r), _r(r) {}
Circle::~Circle() {}

BBox *Circle::shift(const Vec3 &offset) const {
  return new Circle(_pos + offset, _r);
}

double Circle::distanceFrom(const BBox &b) const {
  return (pos() - b.pos()).mag();
}

Vec3 Circle::point(BBoxLocation loc) const {
  return Vec3((_pos.x() - (0.5 * _w)) + (0.5 * _w * (loc & 0x03)),
              (_pos.y() - (0.5 * _h)) + (0.5 * _h * ((loc >> 2) & 0x03)),
              (_pos.z() - (0.5 * _d)) + (0.5 * _d * ((loc >> 4) & 0x03)));
}

bool Circle::containsPoint(const Vec3 &p) const {
  Vec3 distance = _pos - p;
  return distance.mag() < _r;
}

bool Circle::containsBBox(const BBox &b) const {
  return (containsPoint(b.point(Left | Bottom | Far)) &&
          containsPoint(b.point(Right | Top | Near)));
}

bool Circle::intersects(const BBox &b) const {
  int testPoints[] = {0,  1,  2,  4,  5,  6,  8,  9,  10, 16, 17, 18, 20,
                      22, 24, 25, 26, 32, 33, 34, 36, 37, 38, 40, 41, 42};
  for (int i = 0; i < 26; ++i) {
    if (containsPoint(b.point((BBoxLocation)i))) {
      return true;
    }
  }
  return false;
}