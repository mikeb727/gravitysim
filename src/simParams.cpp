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
  return el->IntAttribute(attribute.c_str());
}

SimParameters parseXmlConfig(std::string fileName) {
  SimParameters result = defaultParams;

  tinyxml2::XMLDocument paramsXml;
  if (paramsXml.LoadFile(fileName.c_str())) {
    std::cerr << "warning: could not open \"" << fileName
              << "\"; using default parameters\n";
    return result;
  };
  result.visualization_frameRate =
      getAttributeDouble(&paramsXml, {"visualization", "frameRate"}, "value");
  result.visualization_dimensions = Vec3(
      getAttributeDouble(&paramsXml, {"visualization", "dimensions"}, "width"),
      getAttributeDouble(&paramsXml, {"visualization", "dimensions"},
                         "height"));
  result.environment_frameRate =
      getAttributeDouble(&paramsXml, {"environment", "frameRate"}, "value");
  result.environment_unitsPerMeter =
      getAttributeDouble(&paramsXml, {"environment", "unitsPerMeter"}, "value");
  result.environment_paused =
      getAttributeBool(&paramsXml, {"environment", "paused"}, "value");
  result.environment_boundary = Vec3(
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "width"),
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "height"),
      getAttributeDouble(&paramsXml, {"environment", "boundary"}, "depth"));
  result.environment_gravity =
      Vec3(getAttributeDouble(&paramsXml, {"environment", "gravity"}, "x"),
           getAttributeDouble(&paramsXml, {"environment", "gravity"}, "y"),
           getAttributeDouble(&paramsXml, {"environment", "gravity"}, "z"));
  result.environment_wind =
      Vec3(getAttributeDouble(&paramsXml, {"environment", "wind"}, "x"),
           getAttributeDouble(&paramsXml, {"environment", "wind"}, "y"),
           getAttributeDouble(&paramsXml, {"environment", "wind"}, "z"));
  result.environment_airDensity =
      getAttributeDouble(&paramsXml, {"environment", "airDensity"}, "value");
  result.controls_disableUserInput =
      getAttributeBool(&paramsXml, {"controls", "disableUserInput"}, "value");
  result.controls_fullscreenMode =
      getAttributeBool(&paramsXml, {"controls", "fullscreenMode"}, "value");
  result.controls_radius = {
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "radius"}, "default")};
  result.controls_velocityForward = {
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"},
                         "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityForward"},
                         "default")};
  result.controls_velocityX = {
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityX"}, "default")};
  result.controls_velocityY = {
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityY"}, "default")};
  result.controls_velocityZ = {
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityZ"}, "default")};
  result.controls_velocityAngular = {
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"}, "min"),
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"}, "max"),
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"},
                         "increment"),
      getAttributeDouble(&paramsXml, {"controls", "velocityAngular"},
                         "default")};
  result.controls_angularAxis =
      Vec3(getAttributeDouble(&paramsXml, {"controls", "angularAxis"}, "x"),
           getAttributeDouble(&paramsXml, {"controls", "angularAxis"}, "y"),
           getAttributeDouble(&paramsXml, {"controls", "angularAxis"}, "z"));
  result.tuning_objSpringCoeff =
      getAttributeDouble(&paramsXml, {"tuning", "objSpringCoeff"}, "value");
  result.tuning_objSpringDamping =
      getAttributeDouble(&paramsXml, {"tuning", "objSpringDamping"}, "value");
  result.tuning_objFrictionCoeff =
      getAttributeDouble(&paramsXml, {"tuning", "objFrictionCoeff"}, "value");
  result.input_forward =
      getAttributeInt(&paramsXml, {"input", "forward"}, "value");
  result.input_backward =
      getAttributeInt(&paramsXml, {"input", "backward"}, "value");
  result.input_right = getAttributeInt(&paramsXml, {"input", "right"}, "value");
  result.input_left = getAttributeInt(&paramsXml, {"input", "left"}, "value");
  result.input_down = getAttributeInt(&paramsXml, {"input", "down"}, "value");
  result.input_up = getAttributeInt(&paramsXml, {"input", "up"}, "value");
  result.input_zoomIn =
      getAttributeInt(&paramsXml, {"input", "zoomIn"}, "value");
  result.input_zoomOut =
      getAttributeInt(&paramsXml, {"input", "zoomOut"}, "value");
  result.input_sizeTool =
      getAttributeInt(&paramsXml, {"input", "sizeTool"}, "value");
  result.input_speedTool =
      getAttributeInt(&paramsXml, {"input", "speedTool"}, "value");
  result.input_spinTool =
      getAttributeInt(&paramsXml, {"input", "spinTool"}, "value");
  result.input_pushTool =
      getAttributeInt(&paramsXml, {"input", "pushTool"}, "value");
  result.input_toolReset =
      getAttributeInt(&paramsXml, {"input", "toolReset"}, "value");
  result.input_clearEnv =
      getAttributeInt(&paramsXml, {"input", "clearEnv"}, "value");
  result.input_pause = getAttributeInt(&paramsXml, {"input", "pause"}, "value");
  return result;
}
