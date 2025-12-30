#ifndef SIM_PARAMS_H
#define SIM_PARAMS_H

#include "vec3d.h"
#include <string>

#include <vector>

struct SimParameters {
  double visualization_frameRate;
  Vec3 visualization_dimensions;
  double environment_frameRate;
  double environment_unitsPerMeter;
  bool environment_paused;
  Vec3 environment_boundary;
  Vec3 environment_gravity;
  Vec3 environment_wind;
  double environment_airDensity;
  bool controls_disableUserInput;
  bool controls_fullscreenMode;
  std::vector<double> controls_radius;
  std::vector<double> controls_velocityForward;
  std::vector<double> controls_velocityX;
  std::vector<double> controls_velocityY;
  std::vector<double> controls_velocityZ;
  std::vector<double> controls_velocityAngular;
  Vec3 controls_angularAxis;
  double tuning_objSpringCoeff;
  double tuning_objSpringDamping;
  double tuning_objFrictionCoeff;
};

const SimParameters defaultParams = {
    60,
    Vec3(1024, 768),
    500,
    4,
    false,
    Vec3(25, 50, 150),
    Vec3(0, -9.8, 0),
    Vec3(0, 0, 0),
    0.005,
    true,
    true,
    {0.2, 0.5, 1e-2, 0.3},
    {-30, 30, 0.05, 0},
    {-30, 30, 0.05, 0},
    {-30, 30, 0.05, 0},
    {-30, 30, 0.05, 0},
    {-100, 100, 0.5, 0},
    Vec3(1, 0, 0),
    1e4,
    1e1,
    2.5e-2,
};

SimParameters parseXmlConfig(std::string fileName);

#endif
