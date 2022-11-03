#include "object.h"
#include <cmath>

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

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

bool Object::collidesWith(const Object &otherObj) const {
  if (this == &otherObj)
    return false;
  return nextBBox()->intersects(*otherObj.nextBBox());
}

void Object::resolveCollision(Object &otherObj) {
  double sumVel = _vel.mag() + otherObj._vel.mag();
  double sumMass = _m + otherObj._m;
  Vec collVec = otherObj._bbox->pos() - _bbox->pos();
  double resultAngle = _vel.dir() + ((collVec.dir() - _vel.dir()) * 2) + M_PI;
  _vel = _elast * Vec((otherObj._m * sumVel / sumMass), resultAngle, MagDir);
  otherObj._vel =
      _elast * Vec((_m * sumVel / sumMass), resultAngle + M_PI, MagDir);
}

Vec Object::nextPos() const {
  return rk4(_bbox->pos(), _vel, _accel, (1.0 / framesPerSecond));
}

BBox *Object::nextBBox() const {
  return _bbox->shift(nextPos() - _bbox->pos());
}

void Object::move(bool reverseH = false, bool reverseV = false) {
  _vel = rk4(_vel, _accel, Vec(0, 0), (1.0 / framesPerSecond));
  reverseDirection(reverseH, reverseV);
  _bbox->setPos(nextPos());
}

void Object::reverseDirection(bool h, bool v) {
  _vel = Vec((h ? -1 : 1) * _vel.x(), (v ? -1 : 1) * _vel.y()) * _elast;
}

void Object::print(std::ostream &out) const {
  out << "type " << objType << " dim " << _bbox->h() << "x" << _bbox->w()
      << " pos " << _bbox->pos() << " vel " << _vel << " accel " << _accel
      << (_selected ? " selected" : "");
}
