/* A two-dimensional physics environment. */

#ifndef ENV2D_H
#define ENV2D_H

#include <cmath>
#include <map>

#include "2DVec.h"
#include "object.h"
#include "rectangle.h"
#include "simParams.h"

extern SimParameters simParams;

// each object has a unique ID (allows per-object colors
// if visualization is used)
typedef std::map<int, Object> EnvObjSet;

class Environment {
public:
  // ctor, dtor
  Environment(); // dimensions and gravity zero
  Environment(double width, double height, const Vec2 &gravity,
              double timeStep);
  ~Environment();

  // getters
  const BBox *bbox() const { return _bbox; };
  double dt() const { return _dt; };
  const Vec2 &gravity() const { return _g; };
  EnvObjSet &objs() { return _objs; };
  int time() const { return _t; };

  // object operations
  void addObj(const Object &obj) { _objs.emplace(_nextObjId++, obj); };
  void clearObjs() { _objs.clear(); };
  int lastObjId() const { return _nextObjId - 1; };
  void removeObj(int id) { _objs.erase(id); };

  // simulation operations
  void moveObjs();
  void togglePause() { _paused = _paused ? false : true; };
  void update(); // move objects and increment time (scale
                 // factor to account for frame rates)

  // debug
  void print(std::ostream &out) const;

private:
  BBox *_bbox;     // boundary (dimensions)
  double _dt;      // time step
  Vec2 _g;         // gravity vector
  int _nextObjId;
  EnvObjSet _objs; // set of objects
  bool _paused;    // run state (running or paused)
  int _t;          // simulation time
};

// print compatibility with cout/cerr
inline std::ostream &operator<<(std::ostream &out, Environment &env) {
  env.print(out);
  return out;
}

#endif
