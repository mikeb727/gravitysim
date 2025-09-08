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
  result.envFrameRate =
      getAttributeDouble(&paramsXml, {"environment", "frameRate"}, "value");
  result.windowFrameRate =
      getAttributeDouble(&paramsXml, {"visualization", "frameRate"}, "value");
  result.windowDimensions = Vec3(
      getAttributeDouble(&paramsXml, {"visualization", "dimensions"}, "width"),
      getAttributeDouble(&paramsXml, {"visualization", "dimensions"},
                         "height"));
  result.envDimensions = Vec3(
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "width"),
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "height"),
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "depth"));
  result.envGravity =
      Vec3(getAttributeDouble(&paramsXml, {"environment", "gravity"}, "x"),
           getAttributeDouble(&paramsXml, {"environment", "gravity"}, "y"),
           getAttributeDouble(&paramsXml, {"environment", "gravity"}, "z"));
  result.envScale =
      getAttributeDouble(&paramsXml, {"environment", "unitsPerMeter"}, "value");
  result.envPauseState =
      getAttributeBool(&paramsXml, {"environment", "paused"}, "value");
  result.objSpringCoeff =
      getAttributeDouble(&paramsXml, {"tuning", "objSpringCoeff"}, "value");
  result.objSpringDamping =
      getAttributeDouble(&paramsXml, {"tuning", "objSpringDamping"}, "value");
  result.objFrictionCoeff =
      getAttributeDouble(&paramsXml, {"tuning", "objFrictionCoeff"}, "value");
  result.ctrlRadius = {
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "default")};
  result.ctrlVelForward = {
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"}, "default")};
  result.ctrlVelX = {
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "default")};
  result.ctrlVelY = {
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "default")};
  result.ctrlVelY = {
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "default")};
  result.ctrlVelAngular = {
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"},
                         "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"},
                         "default")};
  result.ctrlAngularAxis = {
      getAttributeDouble(&paramsXml, {"controls", "angularAxis"}, "x"),
      getAttributeDouble(&paramsXml, {"controls", "angularAxis"}, "y"),
      getAttributeDouble(&paramsXml, {"controls", "angularAxis"}, "z"),
  };
  result.disableUserInput =
      getAttributeBool(&paramsXml, {"controls", "disableUserInput"}, "value");
  result.fullscreenMode =
      getAttributeBool(&paramsXml, {"controls", "fullscreenMode"}, "value");
  return result;
}