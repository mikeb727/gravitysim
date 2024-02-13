#include "2DPhysEnv.h"
#include <iostream>

// global simulation parameters (from XML config file)
extern SimParameters simParams;

Environment::Environment() : _dt(0), _t(0) {}

Environment::Environment(double width, double height, const Vec2 &gravity,
                         double timeStep)
    : _bbox(new Rectangle(Vec2(0, 0), width, height)), _dt(timeStep),
      _g(gravity), _nextObjId(0), _paused(false), _t(0) {
  std::cerr << "env create dim " << _bbox->w() << "x" << _bbox->h()
            << " gravity " << _g << " dt " << _dt << "\n";
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
          obj1.second.bbox()->distanceFrom(*obj2.second.bbox()) <
              2.0 * simParams.ctrlRadius[1] * simParams.envScale) {
        if (obj1.second.collidesWith(_dt, obj2.second)) {
          simParams.squishy ? obj1.second.resolveCollision2(obj2.second, _dt)
                            : obj1.second.resolveCollision(obj2.second);
        }
      }
    }
  }
  // move unselected objects
  for (auto &obj : _objs) {
    if (obj.second.selected()){
      obj.second.setNetForce(Vec2());
    }
    else {
      obj.second.setAccel(_g);
      // record x and y offsets of object outside the environment
      simParams.squishy
          ? obj.second.move2(
                _dt, Vec2(fmin(obj.second.nextBBox(_dt)->left().x() -
                                   _bbox->left().x(),
                               0) +
                              fmax(obj.second.nextBBox(_dt)->right().x() -
                                       _bbox->right().x(),
                                   0),
                          fmin(obj.second.nextBBox(_dt)->bottom().y() -
                                   _bbox->top().y(),
                               0) +
                              fmax(obj.second.nextBBox(_dt)->top().y() -
                                       _bbox->bottom().y(),
                                   0)))
          : obj.second.move(
                _dt,
                !(_bbox->containsPoint(obj.second.nextBBox(_dt)->left()) &&
                  _bbox->containsPoint(obj.second.nextBBox(_dt)->right())),
                !(_bbox->containsPoint(obj.second.nextBBox(_dt)->top()) &&
                  _bbox->containsPoint(obj.second.nextBBox(_dt)->bottom())));
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
    result += obj.second.energy();
  }
  return result;
}