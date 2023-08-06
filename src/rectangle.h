#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "bbox.h"

class Rectangle : public BBox {
public:
  // ctor, dtor
  Rectangle();
  Rectangle(const Vec2 &pos, double width, double height);
  ~Rectangle();

  // dimensions
  double w() const { return _w; };
  double h() const { return _h; };
  const Vec2 &pos() const { return _pos; };
  void setPos(const Vec2 &newPos) { _pos = newPos; };

  // key points
  Vec2 center() const;
  Vec2 top() const;
  Vec2 topLeft() const;
  Vec2 left() const;
  Vec2 bottomLeft() const;
  Vec2 bottom() const;
  Vec2 bottomRight() const;
  Vec2 right() const;
  Vec2 topRight() const;

  // intersection operations
  BBox *shift(const Vec2 &) const;
  double distanceFrom(const BBox &) const;
  bool containsPoint(const Vec2 &) const;
  bool containsBBox(const BBox &) const;
  bool intersects(const BBox &) const;
};

#endif
