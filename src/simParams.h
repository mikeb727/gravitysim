#ifndef SIM_PARAMS_H
#define SIM_PARAMS_H

#include "vec3d.h"
#include <string>

#include <vector>

struct SimParameters {
  double envFrameRate;
  double windowFrameRate;
  Vec3 windowDimensions;
  bool envPauseState;
  Vec3 envDimensions;
  Vec3 envGravity;
  double envScale; // pixels per meter
  double objSpringCoeff;
  double objSpringDamping;
  double objFrictionCoeff;
  std::vector<double> ctrlRadius;
  std::vector<double> ctrlElast;
  std::vector<double> ctrlVelForward;
  std::vector<double> ctrlVelX;
  std::vector<double> ctrlVelY;
  std::vector<double> ctrlVelZ;
  std::vector<double> ctrlVelAngular;
  std::vector<double> ctrlAngularAxis;
  bool disableUserInput;
  bool fullscreenMode;
};

const SimParameters defaultParams = {60,
                                     60,
                                     Vec3(800, 600, 0),
                                     false,
                                     Vec3(10, 10, 10),
                                     Vec3(0, -9.8, 0),
                                     4,
                                     1e6,
                                     1e6,
                                     1e-3,
                                     {10, 100, 1, 30},
                                     {0, 1, 0.01, 1},
                                     {-100, 100, 5, 0},
                                     {-100, 100, 5, 0},
                                     {-100, 100, 5, 0},
                                     {-100, 100, 5, 0},
                                     {-50, 50, 0.5, 0},
                                     {0, 0, 0},
                                     false,
                                     false};
SimParameters parseXmlConfig(std::string fileName);

#endif