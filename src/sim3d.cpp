/* Three-dimensional bouncing ball simulator playground */

#include <argparse/argparse.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mb-libs/mbgfx.h>

#include "bbox.h"
#include "control.h"
#include "cursor.h"
#include "env3d.h"
#include "simParams.h"
#include "utility.h"

// switch for compile-time vs runtime shaders
// #define COMPILE_TIME_SHADERS
#ifdef COMPILE_TIME_SHADERS
const char *phongVs =

#include "../assets/phong_shadows_vs_ct.glsl"

    ;
const char *phongFs =

#include "../assets/phong_shadows_fs_ct.glsl"
    ;
const char *stripesVs =

#include "../assets/stripes_vs_ct.glsl"
    ;
const char *stripesFs =

#include "../assets/stripes_fs_ct.glsl"
    ;
const char *cursorVs =

#include "../assets/stripes2_vs_ct.glsl"
    ;
const char *cursorFs =

#include "../assets/stripes2_fs_ct.glsl"
    ;
#endif

// global parameters; used by all objects after initialization
SimParameters simParams;
std::default_random_engine rng; // for random ball colors

int main(int argc, char *argv[]) {

  rng.seed(std::chrono::system_clock::now().time_since_epoch().count());

  argparse::ArgumentParser argParser("gravity_sim");
  argParser.add_argument("-c", "--config").default_value("").nargs(1);
  argParser.parse_args(argc, argv);
  simParams = parseXmlConfig(argParser.get<std::string>("--config"));

  Environment env(simParams.environment_gravity * simParams.environment_unitsPerMeter,
      1.0 / simParams.environment_frameRate);

  env.setWind(simParams.environment_wind);
  env.setAirDensity(simParams.environment_airDensity);

  ControlSet ctrlSet;
  simUtils::setupControls(ctrlSet);

  GraphicsTools::InitGraphics();
  GraphicsTools::Window window("Gravity Sim 3D!",
                               simParams.visualization_dimensions.x(),
                               simParams.visualization_dimensions.y());
  GraphicsTools::Font font("assets/font.ttf", 20);

#ifdef COMPILE_TIME_SHADERS
  GraphicsTools::ShaderProgram phong(phongVs, phongFs, true);
#else
  GraphicsTools::ShaderProgram phong("assets/phong_shadows_vs.glsl",
                                     "assets/phong_shadows_fs.glsl");
  GraphicsTools::ShaderProgram stripes("assets/stripes_vs.glsl",
                                       "assets/stripes_fs.glsl");
#endif

  GraphicsTools::Scene sc;
  GraphicsTools::Camera cam;
  cam.setPos(glm::vec3(0.0, 0.0, 0.0));
  GraphicsTools::DirectionalLight light = {
      glm::normalize(glm::vec3(0, -1, 0.4)), 0.1 * GraphicsTools::Colors::White,
      GraphicsTools::Colors::White, 0.1 * GraphicsTools::Colors::White};
  light._shaders.push_back(&phong);
  light._shaders.push_back(&stripes);

  cam.setPerspective(55.0, (double)window.width() / window.height());

  // have addCamera return added camera's ID?
  sc.addCamera(&cam);
  sc.setActiveCamera(0);
  sc.setDirLight(&light);
  sc.setupShadows();

  window.attachScene(&sc);
  window.setClearColor({0.2, 0.3, 0.5, 1.0});

  Texture checkerboardTex("assets/check.png");
  Texture woodTex("assets/wood.png");
  GraphicsTools::Material checkerboard = {1.5 * GraphicsTools::Colors::White,
                                          &checkerboardTex,
                                          GraphicsTools::Colors::White, 4};
  GraphicsTools::Material woodFloor = {GraphicsTools::Colors::White, &woodTex,
                                       GraphicsTools::Colors::White, 32};
  simUtils::ObjMap staticObjs;
  simUtils::ObjMap ballObjs;

  window.setUserPointer("env", &env);
  window.setUserPointer("phongShader", &phong);
  window.setUserPointer("checkTex", &checkerboardTex);
  window.setUserPointer("woodTex", &woodTex);
  window.setUserPointer("checkMat", &checkerboard);
  window.setUserPointer("woodMat", &woodFloor);
  window.setUserPointer("staticObjMap", &staticObjs);

  GraphicsTools::RenderObject envBounds;
  envBounds.loadModel(simParams.environment_boundary);
  envBounds.setMaterial({GraphicsTools::Colors::Green, nullptr,
                    GraphicsTools::Colors::White, 16});
                    envBounds.setShader(&phong);
  sc.addRenderObject(&envBounds);
  env.setBounds(&envBounds);
  // simUtils::setupEnvWalls(window);

  CursorEmulator cursorEmu(&window);
  int cursorEmuObjId = staticObjs.size();
  cursorEmu.active = false;

  GraphicsTools::RenderObject cursorEmuDisplay;
  cursorEmuDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Black, 0.8,
                            GraphicsTools::Colors::White, 0.2),
       NULL, GraphicsTools::Colors::White, 2}));
  cursorEmuDisplay.setShader(&phong);
  staticObjs.emplace(cursorEmuObjId, cursorEmuDisplay);
  sc.addRenderObject(&staticObjs.at(cursorEmuObjId));

  GraphicsTools::RenderObject arrowDisplay;
  arrowDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Black, 0.8,
                            GraphicsTools::Colors::White, 0.2),
       NULL, GraphicsTools::Colors::White, 2}));
  arrowDisplay.setShader(&phong);
  staticObjs.emplace(cursorEmuObjId + 1, arrowDisplay);
  sc.addRenderObject(&staticObjs.at(cursorEmuObjId + 1));

  GraphicsTools::RenderObject spinDisplay;
  spinDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Black, 0.8,
                            GraphicsTools::Colors::White, 0.2),
       NULL, GraphicsTools::Colors::White, 2}));
  spinDisplay.setShader(&phong);
  staticObjs.emplace(cursorEmuObjId + 2, spinDisplay);
  sc.addRenderObject(&staticObjs.at(cursorEmuObjId + 2));

  simUtils::UserCursor uc;
  simUtils::setupUserCursors(window, &uc);

  // deconflict wall and object renderobject IDs so walls aren't accidentally
  // deleted
  env.setNextId(window.activeScene()->objs()->size());

  glfwSetKeyCallback(window.glfwWindow(), simUtils::keyCallback);
  glfwSetScrollCallback(window.glfwWindow(), simUtils::scrollCallback);
  glfwSetMouseButtonCallback(window.glfwWindow(),
                             simUtils::mouseButtonCallback);
  glfwSetInputMode(window.glfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  window.setUserPointer("ballObjMap", &ballObjs);
  window.setUserPointer("ctrlSet", &ctrlSet);
  window.setUserPointer("ballShader", &stripes);
  window.setUserPointer("cam", &cam);
  window.setUserPointer("cursorEmu", &cursorEmu);
  window.setUserPointer("cursorEmuObjId", &cursorEmuObjId);
  window.setUserPointer("userCursor", &uc);
  window.setUserPointer("font", &font);

  // timing vars
  double tLastWindow = 0;
  double tLastEnv = 0;
  double tLastPrint = 0;

  while (!window.shouldClose()) {
    double tNow = simUtils::computeTNow();

    if ((tNow - tLastEnv) / 1000.0 > (1.0 / simParams.environment_frameRate)) {
      env.update();
      tLastEnv = tNow;

      // only update window at framerate
      if ((tNow - tLastWindow) / 1000.0 >
          (1.0 / simParams.visualization_frameRate)) {
        cursorEmu.update();
        simUtils::handleUserInput(window);
        simUtils::drawSim(window);
        tLastWindow = tNow;
      }
    }
  }

  GraphicsTools::CloseGraphics();

  return 0;
}