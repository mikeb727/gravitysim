#ifndef BBOX_H
#define BBOX_H

#include "2DVec.h"

class BBox {
public:
  virtual ~BBox() = 0;

  // dimensions
  virtual double h() const = 0;
  virtual double w() const = 0;

  // key points and rotation angle
  virtual const Vec2 &pos() const = 0;
  virtual Vec2 center() const = 0;      // return position of center
  virtual Vec2 top() const = 0;         // return position of top point
  virtual Vec2 topLeft() const = 0;     // return position of top-left corner
  virtual Vec2 left() const = 0;        // return position of left point
  virtual Vec2 bottomLeft() const = 0;  // return position of bottom-left corner
  virtual Vec2 bottom() const = 0;      // return position of bottom point
  virtual Vec2 bottomRight() const = 0; // return position of bottom-right corner
  virtual Vec2 right() const = 0;       // return position of right point
  virtual Vec2 topRight() const = 0;    // return position of top-right corner
  virtual double angle() const = 0;

  // directly set position and angle
  virtual void setPos(const Vec2 &newPos) = 0;
  virtual void setAngle(double newAngle) = 0;

  // intersection operations
  virtual BBox *shift(const Vec2 &offset) const = 0;
  virtual double distanceFrom(const BBox &otherBBox) const = 0;
  virtual bool containsPoint(const Vec2 &pos) const = 0;
  virtual bool intersects(const BBox &otherBBox) const = 0;
  virtual bool containsBBox(const BBox &otherBBox) const = 0;

protected:
  BBox();
  BBox(const Vec2 &pos, double width, double height, double angle);
  double _w; // bounding box width
  double _h; // bounding box height
  Vec2 _pos;  // bounding box position
  double _angle; // bounding box rotation angle (radians)
};

#endif
