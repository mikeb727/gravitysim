#ifndef CURSOR_H
#define CURSOR_H

#include <map>
#include <mb-libs/mbgfx.h>

#include <chrono>
#include <random>

#include "2DPhysEnv.h"
#include "2DVec.h"
#include "control.h"

// rng is global
extern std::default_random_engine generator;

enum Action {
  Null = 0,
  ChangePosition = 1,
  ChangeVelocity = 2,
  ChangeRadius = 3,
  CreateObject = 4,
};

// cursor manipulation
// avoid remapping these to window coords until absolutely necessary!
struct CursorData {
  CursorData();
  double arrowX, arrowY;
  double ballX, ballY;
  double deltaX, deltaY;
  double radius;
  bool isGlfw; // remap y component if cursor position is obtained with GLFW
  Action action;
  Vec2 vel;
};

struct CursorEmulator {
  CursorEmulator(GraphicsTools::WindowBase *win);
  CursorData prev;
  CursorData current;
  CursorData target;
  Vec2 bezierP1Ball;  // p1
  Vec2 bezierP1Arrow; // p1
  // cursor movement
  double tNextActionStart, tNextActionEnd;
  void doAction();
  void generatePos();
  void generateVel();
  void generateRadius();
  void computePos(double tNow);
  void computeVel(double tNow);
  void computeRadius(double tNow);
  void createObj();
  void clearEnv();
  void update();
  GraphicsTools::WindowBase *_win;
  bool active;
};

#endif