#include "env3d.h"
#include <iostream>
#include <vector>

#include "bbox.h"
#include "vec3d.h"

// global simulation parameters (from XML config file)
extern SimParameters simParams;

Environment::Environment() : _dt(0), _t(0) {}

Environment::Environment(const Vec3 &gravity, double timeStep)
    : _dt(timeStep), _g(gravity), _nextObjId(0), _paused(false), _t(0) {
  std::cerr << "env create gravity " << _g << " dt " << _dt << "\n";
}

Environment::~Environment() {
  _objs.clear();
  std::cerr << "env delete\n";
};

void Environment::moveObjs() {
  // TODO delete objects very far from the origin (they probably fell off the edge)
  // check for collisions
  for (auto &obj1 : _objs) {
    for (auto &obj2 : _objs) {
      // ignore object pairs where the objects are farther apart than twice the
      // largest radius possible (since it's impossible for them to be
      // colliding)
      if (obj1.first != obj2.first &&
          obj1.second.bbox().distanceFrom(obj2.second.bbox()) <
              2.0 * simParams.controls_radius[1] *
                  simParams.environment_unitsPerMeter) {
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
      obj.second.applyForce(
          drag * simParams.environment_unitsPerMeter); // air resistance
      obj.second.applyTorque(pow(obj.second.bbox().w() * 0.5, 2) *
                             -obj.second.aVel() * _airDensity *
                             simParams.environment_unitsPerMeter);
      obj.second.applyForce(
          obj.second.aVel().cross(obj.second.vel().unit() - _wind) *
          _airDensity * simParams.environment_unitsPerMeter);
      // record x and y offsets of object outside the environment
      Vec3 outsideEnv = computeOutsideEnv(obj.second.bbox().pos(),
                                      obj.second.bbox().w() * 0.5);
      // std::cerr << outsideEnv << "\n";
      obj.second.move(_dt, outsideEnv);
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

Vec3 Environment::computeOutsideEnv(Vec3 pos, double radius) const {
  Vec3 result;
  std::vector<float> meshData = _meshBounds->vertexData();
  std::vector<Vec3> verts_v, normals_v;
  for (int i = 0; i < meshData.size() / 8; ++i) {
    verts_v.push_back(
        Vec3(meshData[8 * i], meshData[8 * i + 1], meshData[8 * i + 2]));
    normals_v.push_back(
        Vec3(meshData[8 * i + 3], meshData[8 * i + 4], meshData[8 * i + 5]));
  }
  for (int i = 0; i < verts_v.size() / 3; ++i) {
    double distToPlane = normals_v[3 * i].dot(Vec3(pos - verts_v[3 * i]));
    if (distToPlane < radius) {
      Vec3 pointOnPlane = pos - (normals_v[3 * i] * distToPlane);
      double abCheck = ((verts_v[3 * i + 1] - verts_v[3 * i])
                            .cross(pointOnPlane - verts_v[3 * i]))
                           .dot(normals_v[3 * i]);

      if (((verts_v[3 * i + 1] - verts_v[3 * i])
               .cross(pointOnPlane - verts_v[3 * i]))
                  .dot(normals_v[3 * i]) > 0 &&
          ((verts_v[3 * i + 2] - verts_v[3 * i + 1])
               .cross(pointOnPlane - verts_v[3 * i + 1]))
                  .dot(normals_v[3 * i]) > 0 &&
          ((verts_v[3 * i] - verts_v[3 * i + 2])
               .cross(pointOnPlane - verts_v[3 * i + 2]))
                  .dot(normals_v[3 * i]) > 0) {
        result += normals_v[3 * i] * (distToPlane - radius);
      }
    }
  }
  return result;
}