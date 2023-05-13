#include "simParams.h"

#include <tinyxml2.h>

#include <iostream>

SimParameters parseXml(std::string fileName) {
  SimParameters result = defaultParams;

  tinyxml2::XMLDocument paramsXml;
  if (paramsXml.LoadFile(fileName.c_str())) {
    std::cerr << "warning: could not open \"" << fileName
              << "\"; using default parameters\n";
    return result;
  };
  result.frameRate = paramsXml.FirstChildElement("gravitysim")
                         ->FirstChildElement("environment")
                         ->FirstChildElement("frameRate")
                         ->IntAttribute("value");
  tinyxml2::XMLElement *envNode = paramsXml.FirstChildElement("gravitysim")
                                      ->FirstChildElement("environment");
  result.envDimensions =
      Vec(envNode->FirstChildElement("boundary")->DoubleAttribute("width"),
          envNode->FirstChildElement("boundary")->DoubleAttribute("height"));
  result.envGravity =
      Vec(envNode->FirstChildElement("gravity")->DoubleAttribute("x"),
          envNode->FirstChildElement("gravity")->DoubleAttribute("y"));
  result.envPauseState =
      envNode->FirstChildElement("paused")->BoolAttribute("value");
  result.collisionCorrectionMultiplier =
      paramsXml.FirstChildElement("gravitysim")
          ->FirstChildElement("tuning")
          ->FirstChildElement("collisionCorrectionMultiplier")
          ->DoubleAttribute("value");
  return result;
}