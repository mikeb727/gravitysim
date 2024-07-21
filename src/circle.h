#ifndef CIRCLE_H
#define CIRCLE_H

#include "bbox.h"

class Circle : public BBox {
public:
  // ctor and dtor
  Circle();
  Circle(const Vec2 &pos, double r);
  ~Circle();

  // dimensions
  double h() const { return _h; };
  double w() const { return _w; };
  double radius() const { return _r; };

  // key points
  const Vec2 &pos() const { return _pos; };
  double angle() const { return _angle; };
  Vec2 center() const;
  Vec2 top() const;
  Vec2 topLeft() const;
  Vec2 left() const;
  Vec2 bottomLeft() const;
  Vec2 bottom() const;
  Vec2 bottomRight() const;
  Vec2 right() const;
  Vec2 topRight() const;

  void setPos(const Vec2 &pos) { _pos = pos; };
  void setAngle(double newAngle) { _angle = newAngle; };

  // intersection operations
  bool containsBBox(const BBox &b) const;
  bool containsPoint(const Vec2 &p) const;
  double distanceFrom(const BBox &b) const;
  bool intersects(const BBox &b) const;
  BBox *shift(const Vec2 &offset) const;

protected:
  double _r; // circle radius
};

#endif
