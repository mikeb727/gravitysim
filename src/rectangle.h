#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "bbox.h"

class Rectangle : public BBox {
public:
  // ctor, dtor
  Rectangle();
  Rectangle(const Vec &pos, double width, double height);
  ~Rectangle();

  // dimensions
  double w() const { return _w; };
  double h() const { return _h; };
  const Vec &pos() const { return _pos; };
  void setPos(const Vec &newPos) { _pos = newPos; };

  // key points
  Vec center() const;
  Vec top() const;
  Vec topLeft() const;
  Vec left() const;
  Vec bottomLeft() const;
  Vec bottom() const;
  Vec bottomRight() const;
  Vec right() const;
  Vec topRight() const;

  // intersection operations
  BBox *shift(const Vec &) const;
  double distanceFrom(const BBox &) const;
  bool containsPoint(const Vec &) const;
  bool containsBBox(const BBox &) const;
  bool intersects(const BBox &) const;
};

#endif
