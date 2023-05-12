#ifndef SIM_PARAMS_H
#define SIM_PARAMS_H

#include "2DVec.h"
#include <string>

struct SimParameters {
  int frameRate;
  bool envPauseState;
  Vec envDimensions;
  Vec envGravity;
  double envScale; // pixels per meter
};

const SimParameters defaultParams = {60, false, Vec(800, 600), Vec(0, 9.8),
                                     120};
SimParameters parseXml(std::string fileName);

#endif