#include "bbox.h"

BBox::BBox() : _h(0), _w(0), _d(0) {}
BBox::BBox(const Vec3 &pos, double width, double height)
    : _pos(pos), _w(width), _h(height), _d(0) {}
BBox::BBox(const Vec3 &pos, double width, double height, double depth)
    : _pos(pos), _w(width), _h(height), _d(depth) {}
BBox::~BBox() {}

BBoxLocation operator|(BBoxLocation lv, BBoxLocation rv) {
  return BBoxLocation(int(lv) | int(rv));
}