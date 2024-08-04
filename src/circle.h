#ifndef CIRCLE_H
#define CIRCLE_H

#include "bbox.h"

class Circle : public BBox {
public:
  // ctor and dtor
  Circle();
  Circle(const Vec3 &pos, double r);
  ~Circle();

  // dimensions
  double h() const { return _h; };
  double w() const { return _w; };
  double d() const { return _d; };
  double radius() const { return _r; };

  // key points
  const Vec3 &pos() const { return _pos; };

  Vec3 point(BBoxLocation loc) const;

  // intersection operations
  bool containsBBox(const BBox &b) const;
  bool containsPoint(const Vec3 &p) const;
  double distanceFrom(const BBox &b) const;
  bool intersects(const BBox &b) const;
  BBox *shift(const Vec3 &offset) const;

protected:
  double _r; // circle radius
};

#endif
