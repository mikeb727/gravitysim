#ifndef CIRCLE_H
#define CIRCLE_H

#include "bbox.h"

#include <iostream>

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
  Vec2 center() const;
  Vec2 top() const;
  Vec2 topLeft() const;
  Vec2 left() const;
  Vec2 bottomLeft() const;
  Vec2 bottom() const;
  Vec2 bottomRight() const;
  Vec2 right() const;
  Vec2 topRight() const;

  void setPos(const Vec2 &newPos) { _pos = newPos; };

  // intersection operations
  BBox *shift(const Vec2 &) const;
  double distanceFrom(const BBox &) const;
  bool containsPoint(const Vec2 &) const;
  bool containsBBox(const BBox &) const;
  bool intersects(const BBox &) const;

  // debug
  void printAllPoints(std::ostream &) const;

protected:
  double _r; // circle radius
};

#endif
