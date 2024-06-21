#include "object.h"
#include "2DVec.h"
#include "bbox.h"
#include "circle.h"
#include "simParams.h"
#include "utility.h"

#include <cmath>

extern SimParameters simParams;

Object::Object()
    : _bbox(new Circle(Vec2(), 10)), _m(1), _elast(0), objType("Object") {}

Object::Object(BBox *bounds, double mass, const Vec2 &position,
               const Vec2 &velocity, const double &elasticity, const double &aVel)
    : _bbox(bounds), _m(mass), _vel(velocity), _aVel(aVel),
      _k(simParams.collisionMultiplierInterObj),
      _b(simParams.collisionMultiplierEnvBoundary), _elast(elasticity),
      _fNet(Vec2()), _tNet(0), objType("Object"), _selected(false) {
  std::cerr << "obj create pos " << _bbox->pos() << " vel " << _vel << " elast "
            << elasticity << "\n";
}

Object::~Object() { std::cerr << "obj delete pos " << _bbox->pos() << "\n"; }

double Object::kenergy() const {
  return (0.5 * _m * pow(_vel.mag() / simParams.envScale, 2));
}

double Object::penergy() const {
  return (_m * simParams.envGravity.mag() * _bbox->pos().y() /
          simParams.envScale);
}

bool Object::collidesWith(double dt, const Object &otherObj) const {
  if (this == &otherObj)
    return false;
  return bbox()->intersects(*otherObj.bbox());
}

void Object::resolveCollision(Object &otherObj, double dt) {
  double sumRadii =
      ((Circle *)_bbox)->radius() + ((Circle *)otherObj.bbox())->radius();
  Vec2 positionDiff = otherObj._bbox->pos() - _bbox->pos();
  Vec2 velocityDiff = _vel - otherObj.vel();
  Vec2 reactiveForce =
      -((_k * (sumRadii - positionDiff.mag()) * positionDiff.unit()) -
        (_b * velocityDiff.unit())) *
      dt;
  Vec2 torqueArm = positionDiff.unit() * ((Circle *)_bbox)->radius();
  Vec2 contactPointVel = _vel + Vec2(_aVel * ((Circle *)_bbox)->radius(),
                                     torqueArm.dir() + M_PI_2, MagDir);
  Vec2 frictionForce = -contactPointVel * reactiveForce.mag() * dt;

  _fNet += reactiveForce + frictionForce;
  _tNet += torqueArm.cross(frictionForce);
}

Vec2 Object::nextPos(double dt) const {
  return rk4(_bbox->pos(), _vel, _accel, dt);
}

double Object::nextAngle(double dt) const {
  return rk4(_bbox->angle(), _aVel, _aAccel, dt);
}

BBox *Object::nextBBox(double dt) const {
  return _bbox->shift(nextPos(dt) - _bbox->pos());
}

void Object::move(double dt, Vec2 outsideEnv) {
  if (outsideEnv.mag() > 0.01) {
    Vec2 reactiveForce = -((_k * outsideEnv) + (_b * _vel)) * dt;
    Vec2 torqueArm = outsideEnv.unit() * ((Circle *)_bbox)->radius();
    Vec2 contactPointVel = _vel + Vec2(_aVel * ((Circle *)_bbox)->radius(),
                                       torqueArm.dir() + M_PI_2, MagDir);
    Vec2 frictionForce = -contactPointVel * reactiveForce.mag() * dt;
    _fNet += reactiveForce + frictionForce;
    _tNet += torqueArm.cross(frictionForce);
  }
  _accel = (simParams.envGravity * simParams.envScale) + (_fNet / _m);
  _vel = rk4(_vel, _accel, Vec2(), dt);
  _aAccel = _tNet / (0.4 * _m * pow(((Circle *)_bbox)->radius(), 2));
  _aVel = rk4(_aVel, _aAccel, 0, dt);
  _bbox->setPos(nextPos(dt));
  _bbox->setAngle(nextAngle(dt));
  // zero out net force
  _fNet = Vec2();
  _tNet = 0;
}

void Object::print(std::ostream &out) const {
  out << "type " << objType << " dim " << _bbox->h() << "x" << _bbox->w()
      << " pos " << _bbox->pos() << " vel " << _vel << " accel " << _accel
      << (_selected ? " selected" : "");
}
