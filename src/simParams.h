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
  std::string environment_boundary;
  Vec3 environment_gravity;
  Vec3 environment_wind;
  double environment_airDensity;
  bool controls_disableUserInput;
  bool controls_fullscreenMode;
  std::vector<double> controls_radius;
  std::vector<double> controls_velocityPitch;
  std::vector<double> controls_velocityYaw;
  std::vector<double> controls_velocityMag;
  std::vector<double> controls_velocityAngular;
  Vec3 controls_angularAxis;
  std::vector<double> controls_kickPitch;
  std::vector<double> controls_kickYaw;
  std::vector<double> controls_kickMag;
  double tuning_objSpringCoeff;
  double tuning_objSpringDamping;
  double tuning_objFrictionCoeff;
  int input_forward;
  int input_backward;
  int input_right;
  int input_left;
  int input_down;
  int input_up;
  int input_zoomIn;
  int input_zoomOut;
  int input_sizeTool;
  int input_speedTool;
  int input_spinTool;
  int input_pushTool;
  int input_toolReset;
  int input_clearEnv;
  int input_pause;
};

const SimParameters defaultParams = {
    60,
    Vec3(1024, 768),
    500,
    4,
    false,
    "assets/cube.obj",
    Vec3(0, -9.8, 0),
    Vec3(0, 0, 0),
    0.005,
    true,
    true,
    {0.2, 0.5, 1e-2, 0.3},
    {-1.571, 1.571, 1e-1, 0},
    {-0.1, 3.15, 1e-1, 0},
    {0, 100, 1e-1, 0},
    {-100, 100, 0.5, 0},
    Vec3(1, 0, 0),
    {-1.571, 1.571, 1e-1, 0},
    {-0.1, 3.15, 1e-1, 0},
    {0, 20, 1e-1, 0},
    1e4,
    1e1,
    2.5e-2,
    87,
    88,
    67,
    90,
    69,
    81,
    65,
    68,
    49,
    50,
    51,
    52,
    48,
    75,
    32,
};

SimParameters parseXmlConfig(std::string fileName);

#endif
