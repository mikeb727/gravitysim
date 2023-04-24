#ifndef CIRCLE_H
#define CIRCLE_H

#include "bbox.h"

#include <iostream>

class Circle : public BBox {
public:
  // ctor and dtor
  Circle();
  Circle(const Vec &pos, double r);
  ~Circle();

  // dimensions
  double h() const { return _h; };
  double w() const { return _w; };
  double radius() const { return _r; };

  // key points
  const Vec &pos() const { return _pos; };
  Vec center() const;
  Vec top() const;
  Vec topLeft() const;
  Vec left() const;
  Vec bottomLeft() const;
  Vec bottom() const;
  Vec bottomRight() const;
  Vec right() const;
  Vec topRight() const;

  void setPos(const Vec &newPos) { _pos = newPos; };

  // intersection operations
  BBox *shift(const Vec &) const;
  double distanceFrom(const BBox &) const;
  bool containsPoint(const Vec &) const;
  bool containsBBox(const BBox &) const;
  bool intersects(const BBox &) const;

  // debug
  void printAllPoints(std::ostream &) const;

protected:
  double _r; // circle radius
};

#endif
