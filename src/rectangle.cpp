#include "rectangle.h"
#include <iostream>

Rectangle::Rectangle() {};
// todo add angle to created rectangles
Rectangle::Rectangle(const Vec3 &pos, double width, double height)
    : BBox(pos, width, height, 0) {}
Rectangle::Rectangle(const Vec3 &pos, double width, double height, double depth)
    : BBox(pos, width, height, depth) {}
Rectangle::~Rectangle() {}

BBox *Rectangle::shift(const Vec3 &d) const {
  return new Rectangle(_pos + d, _h, _w);
}

double Rectangle::distanceFrom(const BBox &b) const {
  return (pos() - b.pos()).mag();
}

Vec3 Rectangle::point(BBoxLocation loc) const {
  return Vec3((_pos.x()) + (0.5 * _w * (loc & 0x03)),
              (_pos.y()) + (0.5 * _h * ((loc >> 2) & 0x03)),
              (_pos.z()) + (0.5 * _d * ((loc >> 4) & 0x03)));
}

bool Rectangle::containsPoint(const Vec3 &p) const {
  return (point(Left).x() < p.x() && p.x() < point(Right).x() &&
          point(Bottom).y() < p.y() && p.y() < point(Top).y() &&
          point(Far).z() < p.z() && p.z() < point(Near).z());
}

bool Rectangle::containsBBox(const BBox &b) const {
  return (containsPoint(b.point(Left | Bottom | Far)) &&
          containsPoint(b.point(Right | Top | Near)));
}

bool Rectangle::intersects(const BBox &b) const {
  int testPoints[] = {0,  1,  2,  4,  5,  6,  8,  9,  10, 16, 17, 18, 20,
                      22, 24, 25, 26, 32, 33, 34, 36, 37, 38, 40, 41, 42};
  for (int i = 0; i < 26; ++i) {
    if (containsPoint(point((BBoxLocation)i))) {
      return true;
    }
  }
  return false;
}
