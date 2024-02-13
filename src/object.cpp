#include "object.h"
#include "circle.h"
#include "simParams.h"

#include <cmath>

extern SimParameters simParams;

Object::Object()
    : _bbox(new Circle(Vec2(), 10)), _m(1), _elast(0), objType("Object") {}

Object::Object(BBox *bounds, double mass, const Vec2 &position,
               const Vec2 &velocity, const double &elasticity)
    : _bbox(bounds), _m(mass), _vel(velocity),
      _k(simParams.collisionMultiplierInterObj), _b(0.1), _elast(elasticity),
      _fNet(Vec2()), objType("Object"), _selected(false) {
  std::cerr << "obj create pos " << _bbox->pos() << " vel " << _vel << " elast "
            << elasticity << "\n";
}

Object::~Object() { std::cerr << "obj delete pos " << _bbox->pos() << "\n"; }

double Object::energy() const {
  return (_m * simParams.envGravity.mag() * _bbox->pos().y() /
          simParams.envScale) +
         (0.5 * _m * pow(_vel.mag() / simParams.envScale, 2));
}

bool Object::collidesWith(double dt, const Object &otherObj) const {
  if (this == &otherObj)
    return false;
  return nextBBox(dt)->intersects(*otherObj.nextBBox(dt));
}

void Object::resolveCollision(Object &otherObj) {
  // see
  // https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
  Vec2 velocityDiff = _vel - otherObj._vel;
  Vec2 positionDiff = _bbox->pos() - otherObj._bbox->pos();
  double sumMass = _m + otherObj._m;
  _vel =
      simParams.collisionCorrectionMultiplier *
      (_vel - (((2.0 * otherObj._m) / sumMass) *
               (velocityDiff.dot(positionDiff) / pow(positionDiff.mag(), 2)) *
               positionDiff));
  otherObj._vel =
      simParams.collisionCorrectionMultiplier *
      (otherObj._vel -
       (((2.0 * _m) / sumMass) *
        (velocityDiff.dot(positionDiff) / pow(positionDiff.mag(), 2)) *
        -positionDiff));
}

void Object::resolveCollision2(Object &otherObj, double dt) {
  double sumRadii =
      ((Circle *)_bbox)->radius() + ((Circle *)otherObj.bbox())->radius();
  Vec2 positionDiff = _bbox->pos() - otherObj._bbox->pos();
  Vec2 velocityDiff = _vel - otherObj.vel();
  _fNet += (sumRadii - positionDiff.mag()) * positionDiff.unit() * dt * _k -
           _b * 2.0 * sqrt(_k * _m) * velocityDiff.unit();
}

Vec2 Object::nextPos(double dt) const {
  return rk4(_bbox->pos(), _vel, _accel, dt);
}

BBox *Object::nextBBox(double dt) const {
  return _bbox->shift(nextPos(dt) - _bbox->pos());
}

void Object::move(double dt, bool reverseH, bool reverseV) {
  _vel = rk4(_vel, _accel, Vec2(0, 0), dt);
  reverseDirection(reverseH, reverseV);
  _bbox->setPos(nextPos(dt));
}

void Object::move2(double dt, Vec2 outsideEnv) {
  _fNet += -outsideEnv.unit() * outsideEnv.mag() * _m *
           simParams.envGravity.mag() * simParams.envScale;
  _accel = (simParams.envGravity * simParams.envScale) + (_fNet / _m);
  _vel = rk4(_vel, _accel, Vec2(), dt);
  _bbox->setPos(nextPos(dt));

  // zero out net force
  _fNet = Vec2();
}

void Object::reverseDirection(bool h, bool v) {
  _vel = Vec2((h ? -1 : 1) * _vel.x(), (v ? -1 : 1) * _vel.y()) * _elast;
}

void Object::print(std::ostream &out) const {
  out << "type " << objType << " dim " << _bbox->h() << "x" << _bbox->w()
      << " pos " << _bbox->pos() << " vel " << _vel << " accel " << _accel
      << (_selected ? " selected" : "");
}
