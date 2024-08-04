#ifndef CURSOR_H
#define CURSOR_H

#include <mb-libs/mbgfx.h>

#include <random>

#include "vec3d.h"

// rng is global
extern std::default_random_engine rng;

enum Action {
  Null = 0,
  ChangePosition = 1,
  ChangeVelocity = 2,
  ChangeRadius = 3,
  ChangeAngularVel = 4,
  CreateObject = 5,
};

// cursor manipulation
// avoid remapping these to window coords until absolutely necessary!
struct CursorData {
  CursorData();
  double arrowX, arrowY, arrowZ;
  double ballX, ballY, ballZ;
  double deltaX, deltaY, deltaZ;
  double radius;
  Vec3 vel;
  double angularVel;
  bool isGlfw; // remap y component if cursor position is obtained with GLFW
  Action action;
};

struct CursorEmulator {
  CursorEmulator(GraphicsTools::WindowBase *win);
  CursorData prev;
  CursorData current;
  CursorData target;
  Vec3 bezierP1Ball;  // p1
  Vec3 bezierP1Arrow; // p1
  // cursor movement
  double tNextActionStart, tNextActionEnd;
  void doAction();
  void generatePos();
  void generateVel();
  void generateRadius();
  void generateAngularVel();
  void computePos(double tNow);
  void computeVel(double tNow);
  void computeRadius(double tNow);
  void computeAngularVel(double tNow);
  void createObj();
  void clearEnv();
  void update();
  GraphicsTools::WindowBase *_win;
  bool active;
};

#endif