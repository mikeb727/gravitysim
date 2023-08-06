#ifndef SIM_PARAMS_H
#define SIM_PARAMS_H

#include "2DVec.h"
#include <string>

#include <vector>

struct SimParameters {
  double frameRate;
  bool envPauseState;
  Vec2 envDimensions;
  Vec2 envGravity;
  double envScale; // pixels per meter
  double collisionCorrectionMultiplier;
  std::vector<double> ctrlRadius;
  std::vector<double> ctrlElast;
  std::vector<double> ctrlVelX;
  std::vector<double> ctrlVelY;
};

const SimParameters defaultParams = {60,
                                     false,
                                     Vec2(800, 600),
                                     Vec2(0, 9.8),
                                     120,
                                     1.0,
                                     {10, 100, 1, 30},
                                     {0, 1, 0.01, 1},
                                     {-100, 100, 5, 0},
                                     {-100, 100, 5, 0}};
SimParameters parseXmlConfig(std::string fileName);

#endif