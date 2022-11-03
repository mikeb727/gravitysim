#include "2DPhysEnv.h"
#include <iostream>

Environment::Environment() : t(0) {}

Environment::Environment(double width, double height, const Vec &gravity)
    : _bbox(new Rectangle(Vec(0, 0), width, height)), t(0), _g(gravity), _paused(false),
      nextObjId(0) {
  std::cerr << "env create dim " << _bbox->w() << "x" << _bbox->h()
            << " gravity " << _g << "\n";
}

Environment::~Environment() {
  _objs.clear();
  std::cerr << "env delete\n";
};

void Environment::moveObjs() {
  // Check for collisions
  for (auto &obj1 : _objs) {
    for (auto &obj2 : _objs) {
      if (obj1.first != obj2.first &&
          obj1.second.bbox()->distanceFrom(*obj2.second.bbox()) < 200) {
        if (obj1.second.collidesWith(obj2.second)) {
          obj1.second.resolveCollision(obj2.second);
        }
      }
    }
  }
  // move unselected objects
  for (auto &obj : _objs) {
    if (!obj.second.selected()) {
      obj.second.setAccel(_g);
      /* reverse if any part of the object would leave the environment */
      obj.second.move(!(_bbox->containsPoint(obj.second.nextBBox()->left()) &&
                        _bbox->containsPoint(obj.second.nextBBox()->right())),
                      !(_bbox->containsPoint(obj.second.nextBBox()->top()) &&
                        _bbox->containsPoint(obj.second.nextBBox()->bottom())));
    }
  }
}

void Environment::update() {
  if (!_paused) {
    moveObjs();
  }
  t++;
}

void Environment::print(std::ostream &out) const {
  out << "[" << t << "] env " << (_paused ? "paused" : "") << "\n";
  if (_objs.size() > 0) {
    for (auto &obj : _objs) {
      out << " obj id " << obj.first << " ";
      obj.second.print(out);
      out << "\n";
    }
  }
}
