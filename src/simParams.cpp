#include "simParams.h"

#include <tinyxml2.h>

#include <iostream>

SimParameters parseXml(std::string fileName) {
  SimParameters result = defaultParams;

  tinyxml2::XMLDocument paramsXml;
  paramsXml.LoadFile(fileName.c_str());
  result.frameRate = paramsXml.FirstChildElement("gravitysim")
                         ->FirstChildElement("environment")
                         ->FirstChildElement("frameRate")
                         ->IntAttribute("value");
  result.envDimensions = Vec(paramsXml.FirstChildElement("gravitysim")
                                 ->FirstChildElement("environment")
                                 ->FirstChildElement("boundary")
                                 ->DoubleAttribute("width"),
                             paramsXml.FirstChildElement("gravitysim")
                                 ->FirstChildElement("environment")
                                 ->FirstChildElement("boundary")
                                 ->DoubleAttribute("height"));
  return result;
}