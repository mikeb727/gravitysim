#ifndef BBOX_H
#define BBOX_H

#include "quaternion.h"
#include "vec3d.h"

enum BBoxLocation {
  Left = 0,
  Center = 1,
  Right = 2,
  Bottom = 0,
  Middle = 4,
  Top = 8,
  Far = 0,
  Neutral = 16,
  Near = 32
};

BBoxLocation operator|(BBoxLocation lv, BBoxLocation rv);

class BBox {
public:
  virtual ~BBox() = 0;

  // dimensions
  virtual double h() const = 0;
  virtual double w() const = 0;
  virtual double d() const = 0;

  // key points and rotation angle
  virtual const Vec3 &pos() const = 0;
  virtual Vec3 point(BBoxLocation loc)
      const = 0; // return position of arbitrary reference point

  // directly set position and angle
  void setPos(const Vec3 &newPos) { _pos = newPos; };

  // intersection operations
  virtual BBox *shift(const Vec3 &offset) const = 0;
  virtual double distanceFrom(const BBox &otherBBox) const = 0;
  virtual bool containsPoint(const Vec3 &pos) const = 0;
  virtual bool intersects(const BBox &otherBBox) const = 0;
  virtual bool containsBBox(const BBox &otherBBox) const = 0;

protected:
  BBox();
  BBox(const Vec3 &pos, double width, double height);
  BBox(const Vec3 &pos, double width, double height, double depth);
  double _w; // bounding box width
  double _h; // bounding box height
  double _d; // bounding box depth
  Vec3 _pos; // bounding box position
};

#endif
