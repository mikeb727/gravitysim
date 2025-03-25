/* Three-dimensional bouncing ball simulator playground*/

#include <argparse/argparse.hpp>
#include <iostream>
#include <mb-libs/colors.h>
#include <mb-libs/mbgfx.h>

#include "control.h"
#include "cursor.h"
#include "env3d.h"
#include "utility.h"

#include <chrono>

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

void drawSim(GraphicsTools::Window *win) {

  Environment *env = (Environment *)(win->userPointer("env"));
  CursorEmulator *cursorEmu = (CursorEmulator *)(win->userPointer("cursorEmu"));
  utils::ObjMap *staticObjs =
      (utils::ObjMap *)(win->userPointer("staticObjMap"));
  utils::ObjMap *envObjs = (utils::ObjMap *)(win->userPointer("ballObjMap"));

  int *cursorEmuId = (int *)(win->userPointer("cursorEmuObjId"));

  staticObjs->at(*cursorEmuId).clearGeometry();
  staticObjs->at(*cursorEmuId)
      .genSphere(cursorEmu->current.radius * simParams.envScale * 0.5, 16, 16);
  staticObjs->at(*cursorEmuId)
      .setPos(glm::vec3(cursorEmu->current.ballX, cursorEmu->current.ballY,
                        cursorEmu->current.ballZ));

  staticObjs->at(*cursorEmuId + 1).clearGeometry();
  staticObjs->at(*cursorEmuId + 1)
      .genArrow(1, 20, cursorEmu->current.ballX, cursorEmu->current.ballY,
                cursorEmu->current.ballZ,
                cursorEmu->current.ballX + cursorEmu->current.deltaX,
                cursorEmu->current.ballY + cursorEmu->current.deltaY,
                cursorEmu->current.ballZ + cursorEmu->current.deltaZ);

  staticObjs->at(*cursorEmuId + 2).clearGeometry();
  float spinPoints[30];
  for (int i = 0; i < 10; ++i) {
    float pointAngle = (cursorEmu->current.angularSpeed > 0 ? 1 : -1) * (1.8 * M_PI * i / 10) +
                       (cursorEmu->current.angularSpeed * 0.05 * glfwGetTime());
    float rotationAngle =
        glm::dot(glm::normalize(glm::vec3(cursorEmu->current.angularAxis.x(),
                                          cursorEmu->current.angularAxis.y(),
                                          cursorEmu->current.angularAxis.z())),
                 {1, 0, 0});
    glm::vec3 rotationAxis = glm::cross(
        glm::normalize(glm::vec3(cursorEmu->current.angularAxis.x(),
                                 cursorEmu->current.angularAxis.y(),
                                 cursorEmu->current.angularAxis.z())),
        {1, 0, 0});
    glm::vec3 arrowPoint((0.5 + fabs(cursorEmu->current.angularSpeed / 50) + cursorEmu->current.radius) * cos(pointAngle),
                         (0.5 + fabs(cursorEmu->current.angularSpeed / 50) + cursorEmu->current.radius) * -sin(pointAngle), 0);

    arrowPoint =
        glm::rotate(rotationAngle, rotationAxis) * glm::vec4(arrowPoint, 1.0);

    spinPoints[3 * i] = cursorEmu->current.ballX + arrowPoint.x;
    spinPoints[3 * i + 1] = cursorEmu->current.ballY + arrowPoint.y;
    spinPoints[3 * i + 2] = cursorEmu->current.ballZ + arrowPoint.z;
  }
  staticObjs->at(*cursorEmuId + 2)
      .genMultiArrow(fmin(1, fabs(cursorEmu->current.angularSpeed / 25)), 20, 10, spinPoints);

  for (auto &obj : env->objs()) {
    Vec3 drawPos = obj.second.bbox().pos();
    Vec3 drawAxis;
    double drawAngle;
    obj.second.rot().toAxisAngle(drawAxis, drawAngle);
    envObjs->at(obj.first).setPos(
        glm::vec3(drawPos.x(), drawPos.y(), drawPos.z()));
    // envObjs->at(obj.first).setRotation(
    //     glm::quat(obj.second.rot().w(), obj.second.rot().x(),
    //               obj.second.rot().y(), obj.second.rot().z()));
    envObjs->at(obj.first).setRotation(
        glm::vec3(drawAxis.x(), drawAxis.y(), drawAxis.z()), drawAngle);
  }

  win->clear();
  win->activeScene()->render(); // all objs
  win->update();
}

void setupEnvWalls(GraphicsTools::Window *win) {
  Environment *env = (Environment *)(win->userPointer("env"));
  utils::ObjMap *renderObjSet =
      (utils::ObjMap *)(win->userPointer("staticObjMap"));
  Texture *wallTex = (Texture *)(win->userPointer("checkTex"));
  GraphicsTools::Material *wallMat =
      (GraphicsTools::Material *)(win->userPointer("checkMat"));
  GraphicsTools::ShaderProgram *shader =
      (GraphicsTools::ShaderProgram *)(win->userPointer("phongShader"));
  GraphicsTools::RenderObject floor;
  floor.setMaterial(*wallMat);
  floor.setTexture(wallTex);
  floor.setShader(shader);
  floor.genPlane(env->bbox().w(), env->bbox().d());
  floor.setPos(glm::vec3(0.5 * env->bbox().w(), 0, 0.5 * env->bbox().d()));
  renderObjSet->emplace(renderObjSet->size(), floor);

  GraphicsTools::RenderObject wall1;
  wall1.setMaterial(*wallMat);
  wall1.setTexture(wallTex);
  wall1.setShader(shader);
  wall1.genPlane(env->bbox().h(), env->bbox().d());
  wall1.setPos(glm::vec3(0, 0.5 * env->bbox().h(), 0.5 * env->bbox().d()));
  wall1.setRotation(glm::vec3(0, 0, 1), -0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall1);

  GraphicsTools::RenderObject wall2;
  wall2.setMaterial(*wallMat);
  wall2.setTexture(wallTex);
  wall2.setShader(shader);
  wall2.genPlane(env->bbox().h(), env->bbox().d());
  wall2.setPos(glm::vec3(env->bbox().w(), 0.5 * env->bbox().h(),
                         0.5 * env->bbox().d()));
  wall2.setRotation(glm::vec3(0, 0, 1), 0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall2);

  GraphicsTools::RenderObject wall3;
  wall3.setMaterial(*wallMat);
  wall3.setTexture(wallTex);
  wall3.setShader(shader);
  wall3.genPlane(env->bbox().w(), env->bbox().h());
  wall3.setPos(glm::vec3(0.5 * env->bbox().w(), 0.5 * env->bbox().h(), 0));
  wall3.setRotation(glm::vec3(1, 0, 0), 0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall3);

  GraphicsTools::RenderObject wall4;
  wall4.setMaterial(*wallMat);
  wall4.setTexture(wallTex);
  wall4.setShader(shader);
  wall4.genPlane(env->bbox().w(), env->bbox().h());
  wall4.setPos(glm::vec3(0.5 * env->bbox().w(), 0.5 * env->bbox().h(),
                         env->bbox().d()));
  wall4.setRotation(glm::vec3(1, 0, 0), -0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall4);

  GraphicsTools::RenderObject ceiling;
  ceiling.setMaterial(*wallMat);
  ceiling.setTexture(wallTex);
  ceiling.setShader(shader);
  ceiling.genPlane(env->bbox().w(), env->bbox().d());
  ceiling.setPos(glm::vec3(0.5 * env->bbox().w(), env->bbox().h(),
                           0.5 * env->bbox().d()));
  ceiling.setRotation(glm::vec3(1, 0, 0), M_PI);
  renderObjSet->emplace(renderObjSet->size(), ceiling);
}

void handleCameraInput(GraphicsTools::Window *win) {
  GraphicsTools::Camera *cam =
      (GraphicsTools::Camera *)(win->userPointer("cam"));
  if (cam) {
    GLFWwindow *glfwWin = win->glfwWindow();
    GraphicsTools::Scene *winScene =
        (GraphicsTools::Scene *)glfwGetWindowUserPointer(glfwWin);
    float panSpeed = glfwGetKey(glfwWin, GLFW_KEY_SPACE) ? 2.0f : 0.5f;
    // zoom
    if (cam->projection() == GraphicsTools::CameraProjType::Perspective) {
      if (glfwGetKey(glfwWin, GLFW_KEY_Z)) {
        cam->setPerspective(std::clamp(cam->fov() - 0.5f, 5.0f, 60.0f),
                            cam->aspectRatio());
      } else if (glfwGetKey(glfwWin, GLFW_KEY_X)) {
        cam->setPerspective(std::clamp(cam->fov() + 0.5f, 5.0f, 60.0f),
                            cam->aspectRatio());
      }
    }

    // pitch and yaw
    if (glfwGetKey(glfwWin, GLFW_KEY_UP)) {
      cam->setPitch(cam->pitch() + (cam->fov() / 45.0f));
    } else if (glfwGetKey(glfwWin, GLFW_KEY_DOWN)) {
      cam->setPitch(cam->pitch() - (cam->fov() / 45.0f));
    }
    if (glfwGetKey(glfwWin, GLFW_KEY_LEFT)) {
      cam->setYaw(cam->yaw() + (cam->fov() / 45.0f));
    } else if (glfwGetKey(glfwWin, GLFW_KEY_RIGHT)) {
      cam->setYaw(cam->yaw() - (cam->fov() / 45.0f));
    }
    // up, down
    if (glfwGetKey(glfwWin, GLFW_KEY_Q)) {
      cam->setPos(cam->pos() + panSpeed * cam->localUp());
    } else if (glfwGetKey(glfwWin, GLFW_KEY_E)) {
      cam->setPos(cam->pos() - panSpeed * cam->localUp());
    }
    // left, right
    if (glfwGetKey(glfwWin, GLFW_KEY_D)) {
      cam->setPos(cam->pos() + panSpeed * cam->localRight());
    } else if (glfwGetKey(glfwWin, GLFW_KEY_A)) {
      cam->setPos(cam->pos() - panSpeed * cam->localRight());
    }
    // forward, backward
    if (glfwGetKey(glfwWin, GLFW_KEY_W)) {
      cam->setPos(cam->pos() + panSpeed * cam->localForward());
    } else if (glfwGetKey(glfwWin, GLFW_KEY_S)) {
      cam->setPos(cam->pos() - panSpeed * cam->localForward());
    }
  }
  // cam->debugPrint(std::cerr);
}

bool handleKeyStates(GraphicsTools::Window *win) {

  // If the escape key is pressed, signal to quit simulation
  glfwSetWindowShouldClose(win->glfwWindow(),
                           glfwGetKey(win->glfwWindow(), GLFW_KEY_ESCAPE));

  return false;
}

int main(int argc, char *argv[]) {

  rng.seed(std::chrono::system_clock::now().time_since_epoch().count());

  argparse::ArgumentParser argParser("gravity_sim");
  argParser.add_argument("-c", "--config").default_value("").nargs(1);
  argParser.parse_args(argc, argv);
  simParams = parseXmlConfig(argParser.get<std::string>("--config"));

  // timing vars
  double tLastWindow = 0;
  double tLastEnv = 0;
  double tLastPrint = 0;

  GraphicsTools::InitGraphics();
  GraphicsTools::Window window("Gravity Sim 3D!",
                               simParams.windowDimensions.x(),
                               simParams.windowDimensions.y());

#ifdef COMPILE_TIME_SHADERS
  GraphicsTools::ShaderProgram phong(phongVs, phongFs, true);
#else
  GraphicsTools::ShaderProgram phong("assets/phong_shadows_vs.glsl",
                                     "assets/phong_shadows_fs.glsl");
  GraphicsTools::ShaderProgram stripes("assets/stripes_vs.glsl",
                                       "assets/stripes_fs.glsl");
#endif
  window.setUserPointer("phongShader", &phong);
  window.setUserPointer("ballShader", &stripes);

  GraphicsTools::Scene sc;
  GraphicsTools::Camera cam;
  GraphicsTools::DirectionalLight light = {
      glm::normalize(glm::vec3(0, -1, 0)), 0.1 * GraphicsTools::Colors::White,
      GraphicsTools::Colors::White, 0.1 * GraphicsTools::Colors::White};
  light._shaders.push_back(&phong);
  light._shaders.push_back(&stripes);

  cam.setPerspective(35.0, (double)window.width() / window.height());

  // have addCamera return added camera's ID?
  sc.addCamera(&cam);
  window.setUserPointer("cam", &cam);
  sc.setActiveCamera(0);
  sc.setDirLight(&light);

  window.attachScene(&sc);
  window.setClearColor({0.2, 0.3, 0.4, 1.0});

  Environment env(simParams.envDimensions.x(), simParams.envDimensions.y(),
                  simParams.envDimensions.z(),
                  simParams.envGravity * simParams.envScale,
                  1.0 / simParams.envFrameRate);
  window.setUserPointer("env", &env);

  ControlSet ctrlSet;
  ctrlSet.addCtrl("radius",
                  Control("Radius", simParams.ctrlRadius[0],
                          simParams.ctrlRadius[1], simParams.ctrlRadius[2],
                          simParams.ctrlRadius[3]));
  ctrlSet.addCtrl("velx", Control("X velocity", simParams.ctrlVelX[0],
                                  simParams.ctrlVelX[1], simParams.ctrlVelX[2],
                                  simParams.ctrlVelX[3]));
  ctrlSet.addCtrl("vely", Control("Y velocity", simParams.ctrlVelY[0],
                                  simParams.ctrlVelY[1], simParams.ctrlVelY[2],
                                  simParams.ctrlVelY[3]));
  ctrlSet.addCtrl("velz", Control("Z velocity", simParams.ctrlVelZ[0],
                                  simParams.ctrlVelZ[1], simParams.ctrlVelZ[2],
                                  simParams.ctrlVelZ[3]));
  ctrlSet.addCtrl(
      "vela", Control("Angular velocity", simParams.ctrlVelAngular[0],
                      simParams.ctrlVelAngular[1], simParams.ctrlVelAngular[2],
                      simParams.ctrlVelAngular[3]));
  ctrlSet.addCtrl("angularAxisX", Control("Angular axis X", -1, 1, 0.01, 0));
  ctrlSet.addCtrl("angularAxisY", Control("Angular axis Y", -1, 1, 0.01, 0));
  ctrlSet.addCtrl("angularAxisZ", Control("Angular axis Z", -1, 1, 0.01, 0));
  window.setUserPointer("ctrlSet", &ctrlSet);

  Texture checkerboardTex("assets/check.png");
  GraphicsTools::Material checkerboard = {GraphicsTools::Colors::White,
                                          &checkerboardTex,
                                          GraphicsTools::Colors::White, 32};
  utils::ObjMap staticObjs;
  utils::ObjMap ballObjs;
  window.setUserPointer("checkTex", &checkerboardTex);
  window.setUserPointer("checkMat", &checkerboard);
  window.setUserPointer("staticObjMap", &staticObjs);
  window.setUserPointer("ballObjMap", &ballObjs);

  setupEnvWalls(&window);
  for (int i = 0; i < staticObjs.size(); ++i) {
    sc.addRenderObject(&staticObjs.at(i));
  }

  cam.setPos(glm::vec3(0.5 * env.bbox().w(), 0.5 * env.bbox().h(),
                       1.75 * env.bbox().d()));

  CursorEmulator cursorEmu(&window);
  cursorEmu.active = true;
  window.setUserPointer("cursorEmu", &cursorEmu);
  int cursorObjId = staticObjs.size();
  window.setUserPointer("cursorEmuObjId", &cursorObjId);

  GraphicsTools::RenderObject cursorDisplay;
  cursorDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Blue, 0.2, GraphicsTools::Colors::White, 0.8), NULL, GraphicsTools::Colors::White, 2}));

  cursorDisplay.setShader(&phong);
  staticObjs.emplace(cursorObjId, cursorDisplay);
  sc.addRenderObject(&staticObjs.at(cursorObjId));

  GraphicsTools::RenderObject arrowDisplay;
  arrowDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Red, 0.2, GraphicsTools::Colors::White, 0.8), NULL, GraphicsTools::Colors::White, 2}));
  arrowDisplay.setShader(&phong);
  staticObjs.emplace(cursorObjId + 1, arrowDisplay);
  sc.addRenderObject(&staticObjs.at(cursorObjId + 1));

  GraphicsTools::RenderObject spinDisplay;
  spinDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Green, 0.2, GraphicsTools::Colors::White, 0.8), NULL, GraphicsTools::Colors::White, 2}));
  spinDisplay.setShader(&phong);
  staticObjs.emplace(cursorObjId + 2, spinDisplay);
  sc.addRenderObject(&staticObjs.at(cursorObjId + 2));

  env.setNextId(staticObjs.size()); // keep walls from being deleted

  sc.setupShadows();

  while (!window.shouldClose()) {
    double tNow = utils::computeTNow();

    if ((tNow - tLastEnv) / 1000.0 > (1.0 / simParams.envFrameRate)) {
      env.update();
      tLastEnv = tNow;
    }

    // only update window at framerate
    if ((tNow - tLastWindow) / 1000.0 > (1.0 / simParams.windowFrameRate)) {
      cursorEmu.update();
      handleCameraInput(&window);
      handleKeyStates(&window);
      window.getEvents();
      drawSim(&window);
      tLastWindow = tNow;
    }
  }

  GraphicsTools::CloseGraphics();

  return 0;
}