#include "cursor.h"
#include "2DPhysEnv.h"
#include "utility.h"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <mb-libs/window.h>
#include <random>

typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

// clang-format off
double actionStateMatrix[6][6] = {0.0, 0.96, 0.97, 0.98, 0.99, 1.0,
                                  0.0, 0.01, 0.97, 0.98, 0.99, 1.0,
                                  0.0, 0.01, 0.02, 0.98, 0.99, 1.0,
                                  0.0, 0.01, 0.02, 0.03, 0.99, 1.0,
                                  0.0, 0.01, 0.02, 0.03, 0.04, 1.0,
                                  0.0, 0.96, 0.97, 0.98, 0.99, 1.0};
// clang-format on

Vec2 bezier2(Vec2 p0, Vec2 p1, Vec2 p2, double t) {
  return ((1 - t) * (1 - t) * p0) + (2 * (1 - t) * t * p1) + (t * t * p2);
}

double smoothStep(double lo, double hi, double x) {
  double y = std::clamp((x - lo) / (hi - lo), 0.0, 1.0);
  return y * y * (3 - (2 * y));
}

CursorData::CursorData()
    : arrowX(0), arrowY(0), ballX(0), ballY(0), deltaX(0), deltaY(0),
      isGlfw(false) {}

CursorEmulator::CursorEmulator(GraphicsTools::WindowBase *win)
    : _win(win), tNextActionEnd(0), tNextActionStart(0) {
  using namespace std::chrono;
  Environment *env = static_cast<Environment *>(_win->userPointer("env"));
  current.ballX = env->bbox()->w() * 0.5;
  current.ballY = env->bbox()->h() * 0.5;
  current.arrowX = env->bbox()->w() * 0.5;
  current.arrowY = env->bbox()->h() * 0.5;
  current.deltaX = 0;
  current.deltaY = 0;
  current.radius = 0.4;
  prev = current;
  current.action = Null;
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
      ControlSet *ctrls =
          static_cast<ControlSet *>(_win->userPointer("ctrlset"));
      Environment *env = static_cast<Environment *>(_win->userPointer("env"));
      (*ctrls)("velx").setValue(current.deltaX / simParams.envScale);
      (*ctrls)("vely").setValue(-current.deltaY / simParams.envScale);
      (*ctrls)("radius").setValue(current.radius);
      (*ctrls)("vela").setValue(current.angularVel);
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
      std::uniform_real_distribution<double> durationDist(0.5, 2.0);
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
    Vec2 cursorPosNew = bezier2(Vec2(prev.ballX, prev.ballY), bezierP1Ball,
                                Vec2(target.ballX, target.ballY), tBlend);
    current.ballX = cursorPosNew.x();
    current.ballY = cursorPosNew.y();
  } else {
    current.ballX = prev.ballX;
    current.ballY = prev.ballY;
  }
}

void CursorEmulator::generatePos() {
  current.action = ChangePosition;
  Environment *env = static_cast<Environment *>(_win->userPointer("env"));
  std::uniform_real_distribution<float> xDist(150, env->bbox()->w() - 150);
  std::uniform_real_distribution<float> yDist(env->bbox()->h() / 2.0,
                                              env->bbox()->h() - 100);
  target.ballX = xDist(rng);
  target.ballY = yDist(rng);
  Vec2 midpoint(0.5 * (target.ballX + prev.ballX),
                0.5 * (target.ballY + prev.ballY));
  bezierP1Ball = midpoint + 0.2 * (Vec2(xDist(rng), yDist(rng)));
}

void CursorEmulator::computeVel(double tNow) {
  double tBlend = smoothStep(tNextActionStart, tNextActionEnd, tNow);
  if (current.action == ChangeVelocity) {
    Vec2 arrowPosNew = bezier2(Vec2(prev.arrowX, prev.arrowY), bezierP1Arrow,
                               Vec2(target.arrowX, target.arrowY), tBlend);
    current.arrowX = arrowPosNew.x();
    current.arrowY = arrowPosNew.y();
    current.deltaX = current.arrowX - current.ballX;
    current.deltaY = current.arrowY - current.ballY;
  } else {
    current.arrowX = prev.arrowX;
    current.arrowY = prev.arrowY;
  }
}

void CursorEmulator::generateVel() {
  current.action = ChangeVelocity;
  using namespace std::chrono;
  std::uniform_real_distribution<float> deltaDist(-6, 6);
  target.deltaX = deltaDist(rng) * simParams.envScale;
  target.deltaY = deltaDist(rng) * simParams.envScale;
  target.arrowX = target.ballX + target.deltaX;
  target.arrowY = target.ballY + target.deltaY;
  Vec2 midpoint = Vec2(0.5 * (target.arrowX + prev.arrowX),
                       0.5 * (target.arrowY + prev.arrowY));
  bezierP1Arrow = midpoint + 0.2 * Vec2(deltaDist(rng), deltaDist(rng));
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
  std::uniform_real_distribution<float> rDist(0.2, 0.4);
  target.radius = rDist(rng);
}

void CursorEmulator::computeAngularVel(double tNow) {
  double blend = smoothStep(tNextActionStart, tNextActionEnd, tNow);
  double angularVelNew =
      current.action == ChangeAngularVel
          ? (1 - blend) * prev.angularVel + blend * target.angularVel
          : prev.angularVel;
  current.angularVel = angularVelNew;
}

void CursorEmulator::generateAngularVel() {
  current.action = ChangeAngularVel;
  std::uniform_real_distribution<float> rngDist(-50, 50);
  target.angularVel = rngDist(rng);
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