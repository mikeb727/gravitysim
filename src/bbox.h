#ifndef BBOX_H
#define BBOX_H

#include "quaternion.h"
#include "vec3d.h"

enum BBoxProperties { None = 0, IsSpherical = 1, IsPrismatic = 2 };

BBoxProperties operator|(BBoxProperties lv, BBoxProperties rv);

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
  BBox();
  BBox(const Vec3 &pos, double side);
  BBox(const Vec3 &pos, double width, double height, double depth);

  // dimensions
  double h() const { return _h; };
  double w() const { return _w; };
  double d() const { return _d; };

  // setters
  void setH(double h) { _h = h; };
  void setW(double w) { _w = w; };
  void setD(double d) { _d = d; };

  BBoxProperties properties() const { return _props; };
  void setProperties(BBoxProperties p) { _props = p; };

  // key points and rotation angle
  const Vec3 &pos() const { return _pos; };
  Vec3
  point(BBoxLocation loc) const; // return position of arbitrary reference point

  // directly set position and angle
  void setPos(const Vec3 &newPos) { _pos = newPos; };

  // intersection operations
  BBox shift(const Vec3 &offset) const;
  double distanceAlong(
      const Vec3 &dir) const; // distance from defined center to surface
  double distanceFrom(const BBox &otherBBox) const;
  bool containsPoint(const Vec3 &pos) const;
  bool intersects(const BBox &otherBBox) const;
  bool containsBBox(const BBox &otherBBox) const;

private:
  double _w; // bounding box width
  double _h; // bounding box height
  double _d; // bounding box depth
  Vec3 _pos; // bounding box position
  BBoxProperties _props;
};

#endif
