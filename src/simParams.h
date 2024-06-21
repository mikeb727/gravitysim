#ifndef SIM_PARAMS_H
#define SIM_PARAMS_H

#include "2DVec.h"
#include <string>

#include <vector>

struct SimParameters {
  double envFrameRate;
  double windowFrameRate;
  bool envPauseState;
  Vec2 envDimensions;
  Vec2 envGravity;
  double envScale; // pixels per meter
  double collisionCorrectionMultiplier;
  double collisionMultiplierInterObj;
  double collisionMultiplierEnvBoundary;
  std::vector<double> ctrlRadius;
  std::vector<double> ctrlElast;
  std::vector<double> ctrlVelX;
  std::vector<double> ctrlVelY;
  std::vector<double> ctrlVelA;
  bool disableUserInput;
  bool fullscreenMode;
};

const SimParameters defaultParams = {60,
                                     60,
                                     false,
                                     Vec2(800, 600),
                                     Vec2(0, 9.8),
                                     200,
                                     1.0,
                                     1e6,
                                     1e6,
                                     {10, 100, 1, 30},
                                     {0, 1, 0.01, 1},
                                     {-100, 100, 5, 0},
                                     {-100, 100, 5, 0},
                                     {-50, 50, 0.5, 0},
                                     false,
                                     false};
SimParameters parseXmlConfig(std::string fileName);

#endif