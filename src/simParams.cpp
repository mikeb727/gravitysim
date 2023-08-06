#include "simParams.h"

#include <tinyxml2.h>

#include <iostream>

double getAttributeDouble(tinyxml2::XMLDocument *doc,
                          std::vector<std::string> elementChain,
                          std::string attribute) {
  tinyxml2::XMLElement *el = doc->RootElement();
  for (std::string elName : elementChain) {
    el = el->FirstChildElement(elName.c_str());
  }
  return el->DoubleAttribute(attribute.c_str());
}

bool getAttributeBool(tinyxml2::XMLDocument *doc,
                      std::vector<std::string> elementChain,
                      std::string attribute) {
  tinyxml2::XMLElement *el = doc->RootElement();
  for (std::string elName : elementChain) {
    el = el->FirstChildElement(elName.c_str());
  }
  return el->BoolAttribute(attribute.c_str());
}

int getAttributeInt(tinyxml2::XMLDocument *doc,
                    std::vector<std::string> elementChain,
                    std::string attribute) {
  tinyxml2::XMLElement *el = doc->RootElement();
  for (std::string elName : elementChain) {
    el = el->FirstChildElement(elName.c_str());
  }
  return el->BoolAttribute(attribute.c_str());
}

SimParameters parseXmlConfig(std::string fileName) {
  SimParameters result = defaultParams;

  tinyxml2::XMLDocument paramsXml;
  if (paramsXml.LoadFile(fileName.c_str())) {
    std::cerr << "warning: could not open \"" << fileName
              << "\"; using default parameters\n";
    return result;
  };
  result.frameRate =
      getAttributeDouble(&paramsXml, {"environment", "frameRate"}, "value");
  result.envDimensions = Vec2(
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "width"),
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "height"));
  result.envGravity =
      Vec2(getAttributeDouble(&paramsXml, {"environment", "gravity"}, "x"),
          getAttributeDouble(&paramsXml, {"environment", "gravity"}, "y"));
  result.envPauseState =
      getAttributeBool(&paramsXml, {"environment", "paused"}, "value");
  result.collisionCorrectionMultiplier = getAttributeDouble(
      &paramsXml, {"tuning", "collisionCorrectionMultiplier"}, "value");
  result.ctrlRadius = {
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "default")};
  result.ctrlVelX = {
      getAttributeDouble(&paramsXml, {"controls", "velocityx"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityx"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityx"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityx"}, "default")};
  result.ctrlVelY = {
      getAttributeDouble(&paramsXml, {"controls", "velocityy"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityy"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityy"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityy"}, "default")};
  return result;
}