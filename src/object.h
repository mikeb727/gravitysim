/* An object within a two-dimensional physics
    environment. */

#ifndef OBJECT_H
#define OBJECT_H

#include "2DVec.h"
#include "circle.h"

#include <cmath>
#include <iostream>

class Object {
public:
  // ctor, dtor
  Object();
  Object(BBox *bounds, double mass, const Vec &position, const Vec &velocity,
         const double &elasticity);
  ~Object();

  // getters
  BBox *bbox() const { return _bbox; }; // can be used for position
  const Vec &vel() const { return _vel; };
  const Vec &accel() const { return _accel; };
  double mass() const { return _m; };
  bool selected() const { return _selected; };
  std::string type() const { return objType; };

  // setters
  void setPos(const Vec &v) { _bbox->setPos(v); };
  void setVel(const Vec &v) { _vel = v; };
  void setAccel(const Vec &v) { _accel = v; };
  void setSelectState(bool state) { _selected = state; };

  // physics/motion operations
  BBox *nextBBox() const; // object bbox at next time step
  bool collidesWith(
      const Object &obj) const; // whether bbox collides with other object's
  void resolveCollision(Object &obj);
  void move(bool reverseX, bool reverseY);
  void reverseDirection(bool x, bool y);

  // debug
  void print(std::ostream &out) const;

private:
  std::string objType;
  BBox *_bbox; // object bounding box
  Vec _vel;    // object velocity
  Vec _accel;  // object acceleration

  double _m;     // object mass
  double _elast; // ratio of pre- to post- collision velocity

  bool _selected;      // selected objects will not move
  Vec nextPos() const; // position of object at next time step; used internally
};

#endif
