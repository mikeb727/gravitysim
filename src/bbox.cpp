#include "bbox.h"

BBox::BBox() : _h(0), _w(0), _d(0), _props(BBoxProperties::None) {}

BBox::BBox(const Vec3 &pos, double side)
    : _pos(pos), _w(side), _h(side), _d(side), _props(BBoxProperties::None) {}

BBox::BBox(const Vec3 &pos, double width, double height, double depth)
    : _pos(pos), _w(width), _h(height), _d(depth),
      _props(BBoxProperties::None) {}

Vec3 BBox::point(BBoxLocation loc) const {
  if (_props == IsSpherical) {
    return Vec3(_pos.x() - (0.5 * _w) + (0.5 * _w * (loc & 0x03)),
                _pos.y() - (0.5 * _h) + (0.5 * _h * ((loc >> 2) & 0x03)),
                _pos.z() - (0.5 * _d) + (0.5 * _d * ((loc >> 4) & 0x03)));
  }
  return Vec3((_pos.x()) + (0.5 * _w * (loc & 0x03)),
              (_pos.y()) + (0.5 * _h * ((loc >> 2) & 0x03)),
              (_pos.z()) + (0.5 * _d * ((loc >> 4) & 0x03)));
}

BBoxLocation operator|(BBoxLocation lv, BBoxLocation rv) {
  return BBoxLocation(int(lv) | int(rv));
}

BBoxProperties operator|(BBoxProperties lv, BBoxProperties rv) {
  return BBoxProperties(int(lv) | int(rv));
}

BBox BBox::shift(const Vec3 &d) const { return BBox(_pos + d, _w, _h, _d); }

double BBox::distanceAlong(const Vec3 &dir) const {
  if (_props == IsSpherical) {
    return 0.5 * _w;
  }
  // TODO cuboids
  return 0;
}

double BBox::distanceFrom(const BBox &b) const {
  return (pos() - b.pos()).mag();
}

bool BBox::containsPoint(const Vec3 &p) const {
  return (point(Left).x() < p.x() && p.x() < point(Right).x() &&
          point(Bottom).y() < p.y() && p.y() < point(Top).y() &&
          point(Far).z() < p.z() && p.z() < point(Near).z());
}

bool BBox::containsBBox(const BBox &b) const {
  return (containsPoint(b.point(Left | Bottom | Far)) &&
          containsPoint(b.point(Right | Top | Near)));
}

bool BBox::intersects(const BBox &b) const {
  int testPoints[] = {0,  1,  2,  4,  5,  6,  8,  9,  10, 16, 17, 18, 20,
                      22, 24, 25, 26, 32, 33, 34, 36, 37, 38, 40, 41, 42};
  for (int i = 0; i < 26; ++i) {
    if (containsPoint(b.point((BBoxLocation)i))) {
      return true;
    }
  }
  return false;
}