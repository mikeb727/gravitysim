/* A two-dimensional physics environment. */

#ifndef ENV2D_H
#define ENV2D_H

#include <cmath>
#include <map>

#include "2DVec.h"
#include "object.h"
#include "rectangle.h"

// each object has a unique ID (allows per-object colors
// if visualization is used)
typedef std::map<int, Object> EnvObjSet;

class Environment {
public:
  // ctor, dtor
  Environment(); // dimensions and gravity zero
  Environment(double width, double height, const Vec &gravity);
  ~Environment();

  // getters
  const Vec &gravity() const { return _g; };
  EnvObjSet &objs() { return _objs; };
  int time() const { return t; };
  const BBox *bbox() const { return _bbox; };

  // object operations
  void addObj(const Object &obj) { _objs.emplace(nextObjId++, obj); };
  void removeObj(int id) { _objs.erase(id); };
  int lastObjId() const { return nextObjId - 1; };
  void clearObjs() { _objs.clear(); };

  // simulation operations
  void togglePause() { _paused = _paused ? false : true; };
  void moveObjs();
  void update(); // move objects and increment time

  // debug
  void print(std::ostream &out) const;

private:
  BBox *_bbox;     // environment boundary
  int t;           // simulation time
  Vec _g;          // environment gravity
  bool _paused;    // environment run state
  EnvObjSet _objs; // set of objects
  int nextObjId;
};

#endif
