#include "cursor.h"
#include "env3d.h"
#include "utility.h"

#include <algorithm>
#include <cstdlib>
#include <random>

#include <chrono>

typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

// clang-format off
double actionStateMatrix[6][6] = {0.0, 0.96, 0.97, 0.98, 0.99, 1.0,
                                  0.0, 0.01, 0.97, 0.98, 0.99, 1.0,
                                  0.0, 0.01, 0.02, 0.98, 0.99, 1.0,
                                  0.0, 0.01, 0.02, 0.03, 0.99, 1.0,
                                  0.0, 0.01, 0.02, 0.03, 0.04, 1.0,
                                  0.0, 0.96, 0.97, 0.98, 0.99, 1.0};
// clang-format on

Vec3 bezier(Vec3 p0, Vec3 p1, Vec3 p2, double t) {
  return ((1 - t) * (1 - t) * p0) + (2 * (1 - t) * t * p1) + (t * t * p2);
}

double smoothStep(double lo, double hi, double x) {
  double y = std::clamp((x - lo) / (hi - lo), 0.0, 1.0);
  return y * y * (3 - (2 * y));
}

CursorData::CursorData()
    : arrowX(0), arrowY(0), arrowZ(0), ballX(0), ballY(0), ballZ(0), deltaX(0),
      deltaY(0), deltaZ(0), isGlfw(false) {}

CursorEmulator::CursorEmulator(GraphicsTools::WindowBase *win)
    : _win(win), tNextActionEnd(0), tNextActionStart(0) {
  using namespace std::chrono;
  Environment *env = static_cast<Environment *>(_win->userPointer("env"));
  current.ballX = 0.0;
  current.ballY = 0.0;
  current.ballZ = 0.0;
  current.arrowX = 0.0;
  current.arrowY = 0.0;
  current.arrowZ = 0.0;
  current.deltaX = 0;
  current.deltaY = 0;
  current.deltaZ = 0;
  current.radius = 0;
  prev = current;
  current.action = Null;
  current.angularAxis = Vec3(1, 0, 0);
}

void CursorEmulator::update() {
  if (active) {
    std::uniform_real_distribution<double> actionDist(0, 1);
    double tNow = utils::computeTNow();
    // if finished with current action
    if (tNow > tNextActionEnd) {
      prev = current;
      // override: rectify difference between arrow positions
      prev.arrowX = current.ballX + current.deltaX;
      prev.arrowY = current.ballY + current.deltaY;
      prev.arrowZ = current.ballZ + current.deltaZ;

      // ControlSet *ctrls =
      //     static_cast<ControlSet *>(_win->userPointer("ctrlSet"));
      // (*ctrls)("velx").setValue(current.deltaX);
      // (*ctrls)("vely").setValue(current.deltaY);
      // (*ctrls)("velz").setValue(current.deltaZ);
      // (*ctrls)("radius").setValue(current.radius);
      // (*ctrls)("vela").setValue(current.angularSpeed);
      // (*ctrls)("angularAxisX").setValue(current.angularAxis.x());
      // (*ctrls)("angularAxisY").setValue(current.angularAxis.y());
      // (*ctrls)("angularAxisZ").setValue(current.angularAxis.z());

      Environment *env = static_cast<Environment *>(_win->userPointer("env"));
      if (env->objs().size() > 20) {
        double deleteObjs = actionDist(rng);
        if (deleteObjs < (1 - std::exp(-0.015 * env->objs().size())))
          clearEnv();
      }
      double nextAction = actionDist(rng);
      for (int i = 0; i < 6; ++i) {
        if (nextAction < actionStateMatrix[current.action][i]) {
          target.action = (Action)i;
          doAction();
          break;
        }
      }
      std::exponential_distribution<double> startTimeDist(0.25);
      std::uniform_real_distribution<double> durationDist(1.0, 2.0);
      tNextActionStart = tNow + (10 * startTimeDist(rng));
      tNextActionEnd = tNextActionStart + (1000 * durationDist(rng));
    }
    computeVel(tNow);
    computePos(tNow);
    computeRadius(tNow);
    computeAngularVel(tNow);
  }
}

void CursorEmulator::computePos(double tNow) {
  double tBlend = smoothStep(tNextActionStart, tNextActionEnd, tNow);
  // set midpoint to existing point if not moving cursor
  // (prevents cursor from going in circle when performing other actions)
  if (current.action == ChangePosition) {
    Vec3 cursorPosNew =
        bezier(Vec3(prev.ballX, prev.ballY, prev.ballZ), bezierP1Ball,
               Vec3(target.ballX, target.ballY, target.ballZ), tBlend);
    current.ballX = cursorPosNew.x();
    current.ballY = cursorPosNew.y();
    current.ballZ = cursorPosNew.z();
  } else {
    current.ballX = prev.ballX;
    current.ballY = prev.ballY;
    current.ballZ = prev.ballZ;
  }
}

void CursorEmulator::generatePos() {
  current.action = ChangePosition;
  Environment *env = static_cast<Environment *>(_win->userPointer("env"));
  std::uniform_real_distribution<float> xDist(3 - (0.5 * env->bbox().w()),
                                              (0.5 * env->bbox().w()) - 3);
  std::uniform_real_distribution<float> yDist(0, (0.5 * env->bbox().h()) - 3);
  std::uniform_real_distribution<float> zDist(3 - (0.5 * env->bbox().d()),
                                              (0.5 * env->bbox().d()) - 3);

  target.ballX = xDist(rng);
  target.ballY = yDist(rng);
  target.ballZ = zDist(rng);
  Vec3 midpoint(0.5 * (target.ballX + prev.ballX),
                0.5 * (target.ballY + prev.ballY),
                0.5 * (target.ballZ + prev.ballZ));
  bezierP1Ball = midpoint + 0.2 * (Vec3(xDist(rng), yDist(rng), zDist(rng)));
}

void CursorEmulator::computeVel(double tNow) {
  double tBlend = smoothStep(tNextActionStart, tNextActionEnd, tNow);
  if (current.action == ChangeVelocity) {
    Vec3 arrowPosNew =
        bezier(Vec3(prev.arrowX, prev.arrowY, prev.arrowZ), bezierP1Arrow,
               Vec3(target.arrowX, target.arrowY, target.arrowZ), tBlend);
    current.arrowX = arrowPosNew.x();
    current.arrowY = arrowPosNew.y();
    current.arrowZ = arrowPosNew.z();
    current.deltaX = current.arrowX - current.ballX;
    current.deltaY = current.arrowY - current.ballY;
    current.deltaZ = current.arrowZ - current.ballZ;
  } else {
    current.arrowX = prev.arrowX;
    current.arrowY = prev.arrowY;
    current.arrowZ = prev.arrowZ;
  }
}

void CursorEmulator::generateVel() {
  current.action = ChangeVelocity;
  using namespace std::chrono;
  std::uniform_real_distribution<float> deltaDist(-18, 18);
  target.deltaX = deltaDist(rng);
  target.deltaY = deltaDist(rng);
  target.deltaZ = deltaDist(rng);
  target.arrowX = target.ballX + target.deltaX;
  target.arrowY = target.ballY + target.deltaY;
  target.arrowZ = target.ballZ + target.deltaZ;
  Vec3 midpoint = Vec3(0.5 * (target.arrowX + prev.arrowX),
                       0.5 * (target.arrowY + prev.arrowY),
                       0.5 * (target.arrowZ + prev.arrowZ));
  bezierP1Arrow =
      midpoint + 0.2 * Vec3(deltaDist(rng), deltaDist(rng), deltaDist(rng));
}

void CursorEmulator::computeRadius(double tNow) {
  double blend = smoothStep(tNextActionStart, tNextActionEnd, tNow);
  double radiusNew = current.action == ChangeRadius
                         ? (1 - blend) * prev.radius + blend * target.radius
                         : prev.radius;
  current.radius = radiusNew;
}

void CursorEmulator::generateRadius() {
  current.action = ChangeRadius;
  std::uniform_real_distribution<float> rDist(0.2, 0.5);
  target.radius = rDist(rng);
}

void CursorEmulator::computeAngularVel(double tNow) {
  float blend = smoothStep(tNextActionStart, tNextActionEnd, tNow);
  double angularSpeedNew =
      current.action == ChangeAngularVel
          ? (1 - blend) * prev.angularSpeed + blend * target.angularSpeed
          : prev.angularSpeed;
  current.angularSpeed = angularSpeedNew;
  if (current.action == ChangeAngularVel) {
    glm::vec3 prevAxis(prev.angularAxis.x(), prev.angularAxis.y(),
                       prev.angularAxis.z());
    glm::vec3 targetAxis(target.angularAxis.x(), target.angularAxis.y(),
                         target.angularAxis.z());
    glm::vec3 newAxis =
        glm::rotate(
            blend * std::acos(glm::dot(glm::normalize(prevAxis),
                                       glm::normalize(targetAxis))),
            glm::cross(glm::normalize(prevAxis), glm::normalize(targetAxis))) *
        glm::vec4(prevAxis, 1.0);
    current.angularAxis = Vec3(newAxis.x, newAxis.y, newAxis.z);
  } else {
    current.angularAxis = prev.angularAxis;
  }
}

void CursorEmulator::generateAngularVel() {
  current.action = ChangeAngularVel;
  std::uniform_real_distribution<float> rngDist(-50, 50);
  target.angularSpeed = rngDist(rng);
  std::uniform_real_distribution<float> axisDist(-1, 1);
  target.angularAxis = Vec3(axisDist(rng), axisDist(rng), axisDist(rng)).unit();
  std::cerr << "new angular axis " << target.angularAxis << "\n";
}

void CursorEmulator::createObj() {
  current.action = CreateObject;
  utils::createObj((GraphicsTools::Window *)_win);
}

void CursorEmulator::clearEnv() {
  Environment *env = static_cast<Environment *>(_win->userPointer("env"));
  for (auto &obj : env->objs()) {
    _win->activeScene()->removeRenderObject(obj.first);
  }
  env->clearObjs();
}

void CursorEmulator::doAction() {
  switch (target.action) {
  case ChangePosition:
    generatePos();
    break;
  case ChangeVelocity:
    generateVel();
    break;
  case ChangeRadius:
    generateRadius();
    break;
  case ChangeAngularVel:
    generateAngularVel();
    break;
  case CreateObject:
    createObj();
    break;
  default:
    clearEnv();
  }
}
