#include "ball.h"
#include "bbox.h"
#include "quaternion.h"
#include "simParams.h"
#include "vec3d.h"

#include <cmath>
#include <iostream>

extern SimParameters simParams;

Ball::Ball() : _bbox(BBox(Vec3(), 0, 0, 0)), _m(1), objType("Object") {
  _bbox.setProperties(BBoxProperties::IsSpherical);
}

Ball::Ball(BBox bounds, double mass, const Vec3 &position, const Vec3 &velocity,
           const double &elasticity, const Vec3 &aVel)
    : _bbox(bounds), _rot({0, 0, 1, 0}), _m(mass), _vel(velocity), _aVel(aVel),
      _k(simParams.objSpringCoeff), _b(simParams.objSpringDamping),
      _fNet(Vec3()), _tNet(Vec3()), objType("bouncing ball"), _selected(false) {
  _bbox.setProperties(BBoxProperties::IsSpherical);
  std::cerr << "obj \"" << objType << "\" create pos " << _bbox.pos() << " vel "
            << _vel << " radius " << _bbox.w() * 0.5 << "\n";
}

double Ball::kenergy() const {
  return (0.5 * _m * pow(_vel.mag() / simParams.envScale, 2));
}

double Ball::penergy() const {
  return (_m * simParams.envGravity.mag() * _bbox.pos().y() /
          simParams.envScale);
}

// assume all objects circular
bool Ball::collidesWith(double dt, const Ball &otherObj) const {
  if (this == &otherObj)
    return false;
  double sumRadii = (_bbox.w() * 0.5) + (otherObj._bbox.w() * 0.5);
  Vec3 positionDiff = otherObj._bbox.pos() - _bbox.pos();
  return positionDiff.mag() < sumRadii;
}

// use sumRadii and positionDiff from collidesWith?
void Ball::resolveCollision(Ball &otherObj, double dt) {
  double sumRadii = (_bbox.w() * 0.5) + (otherObj._bbox.w() * 0.5);
  Vec3 positionDiff = otherObj._bbox.pos() - _bbox.pos();
  Vec3 velocityDiff = _vel - otherObj.vel();
  Vec3 reactiveForce =
      -((_k * (sumRadii - positionDiff.mag()) * positionDiff.unit()) -
        (_b * velocityDiff.unit()));
  Vec3 torqueArm = positionDiff.unit() * (_bbox.w() * 0.5);
  Vec3 velContactPoint = _vel + _aVel.cross(torqueArm);
  Vec3 frictionForce =
      -velContactPoint * simParams.objFrictionCoeff * reactiveForce.mag();
  Vec3 frictionTorque =
      -_aVel * simParams.objFrictionCoeff * reactiveForce.mag();
  _fNet += reactiveForce + frictionForce;
  _tNet += torqueArm.cross(frictionForce) + frictionTorque;
}

Vec3 Ball::nextPos(double dt) const {
  return rk4(_bbox.pos(), _vel, _accel, dt);
}

double Ball::nextAngle(double dt) const { return 0; }

BBox Ball::nextBBox(double dt) const {
  return _bbox.shift(nextPos(dt) - _bbox.pos());
}

void Ball::move(double dt, Vec3 outsideEnv) {
  // collision with walls
  if (outsideEnv.mag() > 0.01) {
    Vec3 reactiveForce = -((_k * outsideEnv) + (_b * _vel));
    Vec3 torqueArm = outsideEnv.unit() * _bbox.w() * 0.5;
    Vec3 velContactPoint = _vel + _aVel.cross(torqueArm);
    Vec3 frictionForce =
        -velContactPoint * simParams.objFrictionCoeff * reactiveForce.mag();
    // prevent ball from spinning indefinitely at rest on ground
    Vec3 frictionTorque =
        -_aVel * simParams.objFrictionCoeff * reactiveForce.mag();
    _fNet += reactiveForce + frictionForce;
    _tNet += torqueArm.cross(frictionForce) + frictionTorque;
  }

  _accel = (simParams.envGravity * simParams.envScale) + (_fNet / _m);
  _vel = rk4(_vel, _accel, Vec3(), dt);
  _bbox.setPos(nextPos(dt));

  _aAccel = _tNet / (0.4 * _m * pow(_bbox.w() * 0.5, 2));
  _aVel = rk4(_aVel, _aAccel, Vec3(), dt);
  Quaternion aVelQuat(_aVel.mag() > 0 ? _aVel.unit() : Vec3(0, 1, 0),
                      _aVel.mag() * dt);
  _rot = _rot * aVelQuat;

  // zero out net force and torque
  _fNet = Vec3();
  _tNet = Vec3();
}

void Ball::print(std::ostream &out) const {
  out << "type " << objType << " dim " << _bbox.h() << "x" << _bbox.w()
      << " pos " << _bbox.pos() << " vel " << _vel << " accel " << _accel
      << (_selected ? " selected" : "");
}
