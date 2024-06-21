/* An object within a two-dimensional physics
    environment. */

#ifndef OBJECT_H
#define OBJECT_H

#include "2DVec.h"
#include "circle.h"
#include "simParams.h"

#include <cmath>
#include <iostream>

extern SimParameters simParams;

class Object {
public:
  // ctor, dtor
  Object();
  Object(BBox *bounds, double mass, const Vec2 &position, const Vec2 &velocity,
         const double &elasticity, const double &aVel);
  ~Object();

  // getters
  BBox *bbox() const { return _bbox; }; // can be used for position
  const Vec2 &vel() const { return _vel; };
  const Vec2 &accel() const { return _accel; };
  double mass() const { return _m; };
  bool selected() const { return _selected; };
  std::string type() const { return objType; };
  double kenergy() const;
  double penergy() const;

  // setters
  void setPos(const Vec2 &v) { _bbox->setPos(v); };
  void setVel(const Vec2 &v) { _vel = v; };
  void setAccel(const Vec2 &v) { _accel = v; };
  void setSelectState(bool state) { _selected = state; };
  void setNetForce(const Vec2 &v) { _fNet = v; }

  // physics/motion operations
  BBox *nextBBox(double dt) const; // object bbox at next time step
  bool collidesWith(
      double dt,
      const Object &obj) const; // whether bbox collides with other object's
  void resolveCollision(Object &obj, double dt);
  // outsideEnv: push object back in bounds if it would leave
  void move(double dt, Vec2 outsideEnv = Vec2());

  // debug
  void print(std::ostream &out) const;

private:
  std::string objType;
  BBox *_bbox;    // object bounding box
  Vec2 _vel;      // object velocity
  Vec2 _accel;    // object acceleration
  double _aVel;   // object angular velocity
  double _aAccel; // object angular acceleration
  Vec2 _fNet;     // net force on object
  double _tNet;   // net torque on object

  double _m;     // object mass
  double _k;     // object stiffness (spring constant)
  double _b;     // object damping ratio
  double _elast; // ratio of pre- to post- collision velocity

  bool _selected; // selected objects will not move
  Vec2 nextPos(
      double dt) const; // position of object at next time step; used internally
  double nextAngle(double dt) const;
};

// print compatibility with cout/cerr
inline std::ostream &operator<<(std::ostream &out, const Object &obj) {
  obj.print(out);
  return out;
}

#endif
