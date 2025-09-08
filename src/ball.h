/* An object within a two-dimensional physics
    environment. */

#ifndef OBJECT_H
#define OBJECT_H

#include "bbox.h"
#include "quaternion.h"
#include "simParams.h"
#include "vec3d.h"

#include <cmath>
#include <iostream>

extern SimParameters simParams;

class Ball {
public:
  // ctor, dtor
  Ball();
  Ball(BBox bounds, double mass, const Vec3 &position, const Vec3 &velocity,
       const double &elasticity, const Vec3 &aVel);

  // getters
  BBox bbox() const { return _bbox; }; // can be used for position
  Quaternion rot() const { return _rot; };
  const Vec3 &vel() const { return _vel; };
  const Vec3 &accel() const { return _accel; };
  double mass() const { return _m; };
  bool selected() const { return _selected; };
  std::string type() const { return objType; };
  double kenergy() const;
  double penergy() const;

  // setters
  void setPos(const Vec3 &v) { _bbox.setPos(v); };
  void setVel(const Vec3 &v) { _vel = v; };
  void setAccel(const Vec3 &v) { _accel = v; };
  void setSelectState(bool state) { _selected = state; };
  void setNetForce(const Vec3 &v) { _fNet = v; }
  void setNetTorque(const Vec3 &v) { _tNet = v; }

  // physics/motion operations
  BBox nextBBox(double dt) const; // object bbox at next time step
  bool collidesWith(
      double dt,
      const Ball &obj) const; // whether bbox collides with other object's
  void resolveCollision(Ball &obj, double dt);
  // outsideEnv: push object back in bounds if it would leave
  void move(double dt, Vec3 outsideEnv = Vec3());

  // debug
  void print(std::ostream &out) const;

private:
  std::string objType;
  BBox _bbox;      // object bounding box
  Quaternion _rot; // object rotational orientation
  Vec3 _vel;       // object velocity
  Vec3 _accel;     // object acceleration
  Vec3 _aVel;      // object angular velocity
  Vec3 _aAccel;    // object angular acceleration
  Vec3 _fNet;      // net force on object
  Vec3 _tNet;      // net torque on object

  double _m; // object mass
  double _k; // object stiffness (spring constant)
  double _b; // object damping ratio

  bool _selected; // selected objects will not move
  Vec3 nextPos(
      double dt) const; // position of object at next time step; used internally
  double nextAngle(double dt) const;
};

// print compatibility with cout/cerr
inline std::ostream &operator<<(std::ostream &out, const Ball &obj) {
  obj.print(out);
  return out;
}

#endif
