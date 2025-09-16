/* A two-dimensional physics environment. */

#ifndef ENV3D_H
#define ENV3D_H

#include <cmath>
#include <map>

#include "ball.h"
#include "simParams.h"
#include "vec3d.h"

extern SimParameters simParams;

// each object has a unique ID (allows per-object colors
// if visualization is used)
typedef std::map<int, Ball> EnvObjSet;

class Environment {
public:
  // ctor, dtor
  Environment(); // dimensions and gravity zero
  Environment(double width, double height, double depth, const Vec3 &gravity,
              double timeStep);
  ~Environment();

  // getters
  const BBox bbox() const { return _bbox; };
  double dt() const { return _dt; };
  const Vec3 &gravity() const { return _g; };
  const Vec3 &wind() const { return _wind; };
  const double &airDensity() const { return _airDensity; };
  EnvObjSet &objs() { return _objs; };
  int time() const { return _t; };

  // setters
  void setWind(Vec3 w) { _wind = w; };
  void setAirDensity(double d) { _airDensity = d; };

  // object operations
  void addObj(const Ball &obj) { _objs[_nextObjId++] = obj; };
  void clearObjs() { _objs.clear(); };
  int lastObjId() const { return _nextObjId - 1; };
  void removeObj(int id) { _objs.erase(id); };
  void setNextId(int id) {
    _nextObjId = id;
  }; // to handle issues with non-ball renderobject deletion

  // simulation operations
  void moveObjs();
  void togglePause() { _paused = _paused ? false : true; };
  void update(); // move objects and increment time (scale
                 // factor to account for frame rates)

  // debug
  void print(std::ostream &out) const;

  // tuning
  // kinetic and potential of all objects
  double computeEnergy() const;

private:
  BBox _bbox; // boundary (dimensions)
  double _dt; // time step
  Vec3 _g;    // gravity vector
  Vec3 _wind;
  double _airDensity;
  int _nextObjId;
  EnvObjSet _objs; // set of objects
  bool _paused;    // run state (running or paused)
  int _t;          // simulation time
};

// print compatibility with cout/cerr
inline std::ostream &operator<<(std::ostream &out, const Environment &env) {
  env.print(out);
  return out;
}

#endif
