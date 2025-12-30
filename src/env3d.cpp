#include "env3d.h"
#include <iostream>

#include "bbox.h"

// global simulation parameters (from XML config file)
extern SimParameters simParams;

Environment::Environment() : _dt(0), _t(0) {}

Environment::Environment(double width, double height, double depth,
                         const Vec3 &gravity, double timeStep)
    : _bbox(BBox(Vec3(0, 0, 0), width, height, depth)), _dt(timeStep),
      _g(gravity), _nextObjId(0), _paused(false), _t(0) {
  std::cerr << "env create dim " << _bbox.w() << "x" << _bbox.h() << "x"
            << _bbox.d() << " gravity " << _g << " dt " << _dt << "\n";
}

Environment::~Environment() {
  _objs.clear();
  std::cerr << "env delete\n";
};

void Environment::moveObjs() {
  // check for collisions
  for (auto &obj1 : _objs) {
    for (auto &obj2 : _objs) {
      // ignore object pairs where the objects are farther apart than twice the
      // largest radius possible (since it's impossible for them to be
      // colliding)
      if (obj1.first != obj2.first &&
          obj1.second.bbox().distanceFrom(obj2.second.bbox()) <
              2.0 * simParams.controls_radius[1] * simParams.environment_unitsPerMeter) {
        if (obj1.second.collidesWith(_dt, obj2.second)) {
          obj1.second.resolveCollision(obj2.second, _dt);
        }
      }
    }
  }
  // move unselected objects
  for (auto &obj : _objs) {
    if (obj.second.selected()) {
      obj.second.setNetForce(Vec3());
      obj.second.setNetTorque(Vec3());
    } else {
      Vec3 drag((_wind - obj.second.vel()).unit() * 0.5 * _airDensity *
                pow((_wind - obj.second.vel()).mag(), 2) *
                pow(obj.second.bbox().w() * 0.5, 2) * 0.5); // assume 0.5 C_d
      obj.second.applyForce(drag * simParams.environment_unitsPerMeter); // air resistance
      obj.second.applyTorque(pow(obj.second.bbox().w() * 0.5, 2) * -obj.second.aVel() * _airDensity * simParams.environment_unitsPerMeter);
      obj.second.applyForce(
          obj.second.aVel().cross(obj.second.vel().unit() - _wind) *
          _airDensity * simParams.environment_unitsPerMeter);
      // record x and y offsets of object outside the environment
      obj.second.move(
          _dt,
          Vec3(
              fmin(obj.second.bbox().point(Left).x() - _bbox.point(Left).x(),
                   0) +
                  fmax(obj.second.bbox().point(Right).x() -
                           _bbox.point(Right).x(),
                       0),
              fmin(obj.second.bbox().point(Bottom).y() -
                       _bbox.point(Bottom).y(),
                   0) +
                  fmax(obj.second.bbox().point(Top).y() - _bbox.point(Top).y(),
                       0),
              fmin(obj.second.bbox().point(Far).z() - _bbox.point(Far).z(), 0) +
                  fmax(obj.second.bbox().point(Near).z() -
                           _bbox.point(Near).z(),
                       0)));
    }
  }
}

void Environment::update() {
  if (!_paused) {
    moveObjs();
  }
  _t++;
}

void Environment::print(std::ostream &out) const {
  out << "[t " << _t << "] env " << (_paused ? "paused" : "") << "\n";
  if (_objs.size() > 0) {
    for (auto &obj : _objs) {
      out << " obj id " << obj.first << " " << obj.second << "\n";
    }
  }
}

double Environment::computeEnergy() const {
  double result = 0;
  for (auto &obj : _objs) {
    result += obj.second.kenergy() + obj.second.penergy();
  }
  return result;
}