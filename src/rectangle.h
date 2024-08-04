#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "bbox.h"

class Rectangle : public BBox {
public:
  // ctor, dtor
  Rectangle();
  Rectangle(const Vec3 &pos, double width, double height);
  Rectangle(const Vec3 &pos, double width, double height, double depth);
  ~Rectangle();

  // dimensions
  double w() const { return _w; };
  double h() const { return _h; };
  double d() const { return _d; }
  const Vec3 &pos() const { return _pos; };

  Vec3 point(BBoxLocation loc) const;

  // intersection operations
  BBox *shift(const Vec3 &) const;
  double distanceFrom(const BBox &) const;
  bool containsPoint(const Vec3 &) const;
  bool containsBBox(const BBox &) const;
  bool intersects(const BBox &) const;
};

#endif
