#include "object.h"
#include <cmath>

Object::Object()
    : _bbox(new Circle(Vec(0, 0), 10)), _m(1), _elast(0), objType("Object") {}

Object::Object(BBox *bounds, double mass, const Vec &position,
               const Vec &velocity, const double &elasticity)
    : _bbox(bounds), _m(mass), _vel(velocity), _elast(elasticity),
      objType("Object"), _selected(false) {
  std::cerr << "obj create pos " << _bbox->pos() << " vel " << _vel << " elast "
            << elasticity << "\n";
}

Object::~Object() { std::cerr << "obj delete pos " << _bbox->pos() << "\n"; }

bool Object::collidesWith(double dt, const Object &otherObj) const {
  if (this == &otherObj)
    return false;
  return nextBBox(dt)->intersects(*otherObj.nextBBox(dt));
}

void Object::resolveCollision(Object &otherObj) {
  // see
  // https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
  Vec velocityDiff = _vel - otherObj._vel;
  Vec positionDiff = _bbox->pos() - otherObj._bbox->pos();
  double sumMass = _m + otherObj._m;
  double correctionMult = 0.995;
  _vel = correctionMult * (_vel - (((2.0 * otherObj._m) / sumMass) *
                 (velocityDiff.dot(positionDiff) / pow(positionDiff.mag(), 2)) * positionDiff));
  otherObj._vel = correctionMult * (otherObj._vel - (((2.0 * _m) / sumMass) *
                 (velocityDiff.dot(positionDiff) / pow(positionDiff.mag(), 2)) * -positionDiff));

}

Vec Object::nextPos(double dt) const {
  return rk4(_bbox->pos(), _vel, _accel, dt);
}

BBox *Object::nextBBox(double dt) const {
  return _bbox->shift(nextPos(dt) - _bbox->pos());
}

void Object::move(double dt, bool reverseH, bool reverseV) {
  _vel = rk4(_vel, _accel, Vec(0, 0), dt);
  reverseDirection(reverseH, reverseV);
  _bbox->setPos(nextPos(dt));
}

void Object::reverseDirection(bool h, bool v) {
  _vel = Vec((h ? -1 : 1) * _vel.x(), (v ? -1 : 1) * _vel.y()) * _elast;
}

void Object::print(std::ostream &out) const {
  out << "type " << objType << " dim " << _bbox->h() << "x" << _bbox->w()
      << " pos " << _bbox->pos() << " vel " << _vel << " accel " << _accel
      << (_selected ? " selected" : "");
}
