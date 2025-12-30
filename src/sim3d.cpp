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

void handleUserInput(GraphicsTools::Window *win) {
  GraphicsTools::Camera *cam =
      static_cast<GraphicsTools::Camera *>(win->userPointer("cam"));
  GLFWwindow *glfwWin = win->glfwWindow();
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlSet"));
  simUtils::UserCursor *userCursor =
      static_cast<simUtils::UserCursor *>(win->userPointer("userCursor"));
  Environment *env = static_cast<Environment *>(win->userPointer("env"));

  // camera movement
  if (cam) {
    float panSpeed = glfwGetKey(glfwWin, GLFW_KEY_LEFT_SHIFT) ? 1.0 : 0.5f;
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

  // update cursor position deltas
  double newX, newY;
  glfwGetCursorPos(win->glfwWindow(), &newX, &newY);
  userCursor->cursorVel =
      Vec3(newX - userCursor->prev.ballX, userCursor->prev.ballY - newY);
  // use for any vector requiring rotation to camera frame
  glm::vec4 rotatedVector;

  if (glfwGetMouseButton(win->glfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) ==
      GLFW_PRESS) {
    userCursor->rightClickDragDelta =
        Vec3(newX - userCursor->lastRightClick.x(),
             userCursor->lastRightClick.y() - newY);
    switch (userCursor->activeTool) {
    case simUtils::Tool::SizeTool:
      (*ctrls)("radius").changeValue(0.001 * userCursor->cursorVel.y());
      break;
    case simUtils::Tool::SpeedTool:
      // create xy vector from click-drag with z set to zero
      // apply camera rotation matrix
      rotatedVector =
          cam->viewMatrix() * glm::vec4({userCursor->cursorVel.x(),
                                         -userCursor->cursorVel.y(), 0, 0});

      (*ctrls)("velx").changeValue(0.05 * rotatedVector.x);
      (*ctrls)("vely").changeValue(0.05 * rotatedVector.y);
      (*ctrls)("velz").changeValue(0.05 * rotatedVector.z);
      break;
    case simUtils::Tool::SpinTool:
      // create xy vector from click-drag with z set to zero
      // apply camera rotation matrix
      rotatedVector = cam->viewMatrix() *
                      glm::vec4({-userCursor->rightClickDragDelta.y(),
                                 userCursor->rightClickDragDelta.x(), 0, 0});

      (*ctrls)("vela").setValue(0.1 * glm::length(rotatedVector));
      (*ctrls)("angularAxisX").setValue(0.01 * rotatedVector.x);
      (*ctrls)("angularAxisY").setValue(0.01 * rotatedVector.y);
      (*ctrls)("angularAxisZ").setValue(0.01 * rotatedVector.z);
      break;
    }
  } else {
    // mouselook
    if (cam) {
      cam->setPitch(
          std::clamp(cam->pitch() - 0.1 * std::clamp(-userCursor->cursorVel.y(),
                                                     -40.0, 40.0),
                     -95.0, 95.0));
      cam->setYaw(cam->yaw() -
                  0.1 * std::clamp(userCursor->cursorVel.x(), -40.0, 40.0));
    }
  }

  if (userCursor->selectedObjId != -1) {
    env->objs()
        .at(userCursor->selectedObjId)
        .setPos(Vec3(userCursor->data.ballX, userCursor->data.ballY,
                     userCursor->data.ballZ) +
                userCursor->objSelectionOffset);
  }

  userCursor->prev.ballX = newX;
  userCursor->prev.ballY = newY;

  // we just changed camera position, so update user's cursor

  userCursor->data.ballX = cam->pos().x + 10.0 * cam->localForward().x;
  userCursor->data.ballY = cam->pos().y + 10.0 * cam->localForward().y;
  userCursor->data.ballZ = cam->pos().z + 10.0 * cam->localForward().z;
  userCursor->data.arrowX =
      cam->pos().x + (0.2 * (*ctrls)["velx"] * (*ctrls)["velForward"]) +
      (10.0 + 2.0 * ((*ctrls)["velForward"])) * cam->localForward().x;
  userCursor->data.arrowY =
      cam->pos().y + (0.2 * (*ctrls)["vely"] * (*ctrls)["velForward"]) +
      (10.0 + 2.0 * ((*ctrls)["velForward"])) * cam->localForward().y;
  userCursor->data.arrowZ =
      cam->pos().z + (0.2 * (*ctrls)["velz"] * (*ctrls)["velForward"]) +
      (10.0 + 2.0 * ((*ctrls)["velForward"])) * cam->localForward().z;
  userCursor->data.deltaX = userCursor->data.arrowX - userCursor->data.ballX;
  userCursor->data.deltaY = userCursor->data.arrowY - userCursor->data.ballY;
  userCursor->data.deltaZ = userCursor->data.arrowZ - userCursor->data.ballZ;
  userCursor->data.radius = (*ctrls)["radius"];
  userCursor->data.angularAxis =
      Vec3((*ctrls)["angularAxisX"], (*ctrls)["angularAxisY"],
           (*ctrls)["angularAxisZ"]);
  userCursor->data.angularSpeed = (*ctrls)["vela"];
}

// keypresses, NOT key states
void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  simUtils::UserCursor *uc =
      (simUtils::UserCursor *)(mbWin->userPointer("userCursor"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlSet"));

  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    uc->activeTool = uc->activeTool == simUtils::Tool::SizeTool
                         ? simUtils::Tool::None
                         : simUtils::Tool::SizeTool;
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    uc->activeTool = uc->activeTool == simUtils::Tool::SpeedTool
                         ? simUtils::Tool::None
                         : simUtils::Tool::SpeedTool;
  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    uc->activeTool = uc->activeTool == simUtils::Tool::SpinTool
                         ? simUtils::Tool::None
                         : simUtils::Tool::SpinTool;
  }
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    switch (uc->activeTool) {
    case simUtils::Tool::SizeTool:
      (*ctrls)("radius").reset();
      break;
    case simUtils::Tool::SpeedTool:
      (*ctrls)("velForward").reset();
      (*ctrls)("velx").reset();
      (*ctrls)("vely").reset();
      (*ctrls)("velz").reset();
      break;
    case simUtils::Tool::SpinTool:
      (*ctrls)("vela").reset();
      break;
    case simUtils::Tool::None:
    default:
      std::cerr << "note: no active tool to reset\n";
    }
  }
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
    simUtils::clearEnvObjs(mbWin);
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
    env->togglePause();
  }
}

void scrollCallback(GLFWwindow *win, double x, double y) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlSet"));
  simUtils::UserCursor *userCursor =
      (simUtils::UserCursor *)mbWin->userPointer("userCursor");
  if (userCursor->activeTool == simUtils::Tool::SizeTool) {
    (*ctrls)("radius").changeValue(0.01 * y);
  }
  if (userCursor->activeTool == simUtils::Tool::SpeedTool) {
    (*ctrls)("velForward").changeValue(y);
    if ((*ctrls)["velForward"] < 0) {
      (*ctrls)("velForward").setValue(0);
    }
  }
  if (userCursor->activeTool == simUtils::Tool::SpinTool) {
    (*ctrls)("vela").changeValue(y);
  }
}

void mouseButtonCallback(GLFWwindow *win, int button, int action, int m) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  simUtils::UserCursor *uc =
      (simUtils::UserCursor *)(mbWin->userPointer("userCursor"));

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    uc->lastRightClick = Vec3(x, y, 0);
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    Vec3 candidateObjPos(uc->data.ballX, uc->data.ballY, uc->data.ballZ);
    int objIdAtCandPos = simUtils::objIdAtEnvPos(candidateObjPos, env);
    if (objIdAtCandPos != -1) {
      simUtils::removeEnvObj(mbWin, objIdAtCandPos);
      env->removeObj(objIdAtCandPos);
    }
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    Vec3 candidateObjPos(uc->data.ballX, uc->data.ballY, uc->data.ballZ);

    int objIdAtCandPos = simUtils::objIdAtEnvPos(candidateObjPos, env);
    if (objIdAtCandPos != -1) {
      env->objs().at(objIdAtCandPos).setSelectState(true);
      uc->selectedObjId = objIdAtCandPos;
      uc->objSelectionOffset =
          env->objs().at(objIdAtCandPos).bbox().pos() - candidateObjPos;
    }
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    GraphicsTools::Camera *cam =
        (GraphicsTools::Camera *)mbWin->userPointer("cam");

    Vec3 candidateObjPos(uc->data.ballX, uc->data.ballY, uc->data.ballZ);
    int objIdAtCandPos = simUtils::objIdAtEnvPos(candidateObjPos, env);
    if (objIdAtCandPos != -1) {
      env->objs()
          .at(objIdAtCandPos)
          .applyForce(100000 * Vec3(cam->localForward().x,
                                    cam->localForward().y,
                                    cam->localForward().z));
    }
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    ControlSet *ctrls =
        static_cast<ControlSet *>(mbWin->userPointer("ctrlSet"));
    simUtils::ObjMap *objMap =
        static_cast<simUtils::ObjMap *>(mbWin->userPointer("ballObjMap"));
    GraphicsTools::ShaderProgram *shader =
        static_cast<GraphicsTools::ShaderProgram *>(
            mbWin->userPointer("ballShader"));

    // place relative to camera pos
    GraphicsTools::Camera *cam =
        (GraphicsTools::Camera *)mbWin->userPointer("cam");

    Vec3 candidateObjPos(uc->data.ballX, uc->data.ballY, uc->data.ballZ);

    Vec3 candidateObjVel(
        (*ctrls)["velx"] + (*ctrls)["velForward"] * cam->localForward().x,
        (*ctrls)["vely"] + (*ctrls)["velForward"] * cam->localForward().y,
        (*ctrls)["velz"] + (*ctrls)["velForward"] * cam->localForward().z);

    // no object at cursor
    if (simUtils::cubeOverlapAtEnvPos(candidateObjPos, env,
                                      (*ctrls)["radius"] * 2.0 *
                                          simParams.environment_unitsPerMeter) &&
        env->bbox().containsBBox(BBox(
            candidateObjPos, (*ctrls)["radius"] * 2.0 * simParams.environment_unitsPerMeter))) {
      env->addObj(Ball(
          BBox(candidateObjPos, (*ctrls)["radius"] * 2.0 * simParams.environment_unitsPerMeter),
          pow((*ctrls)["radius"] * simParams.environment_unitsPerMeter, 3), candidateObjPos,
          candidateObjVel * simParams.environment_unitsPerMeter, 1,
          (*ctrls)["vela"] * Vec3((*ctrls)["angularAxisX"],
                                  (*ctrls)["angularAxisY"],
                                  (*ctrls)["angularAxisZ"])));
      GraphicsTools::Material mat = {GraphicsTools::randomColor(), NULL,
                                     0.5 * GraphicsTools::Colors::White, 4};
      objMap->emplace(env->lastObjId(), GraphicsTools::RenderObject());
      objMap->at(env->lastObjId()).setShader(shader);
      objMap->at(env->lastObjId()).setMaterial(mat);
      objMap->at(env->lastObjId())
          .genSphere((*ctrls)["radius"] * simParams.environment_unitsPerMeter, 16, 16);
      objMap->at(env->lastObjId())
          .setPos(glm::vec3(candidateObjPos.x(), candidateObjPos.y(),
                            candidateObjPos.z()));
      mbWin->activeScene()->addRenderObject(&objMap->at(env->lastObjId()));
    }
    if (uc->selectedObjId != -1) {
      env->objs().at(uc->selectedObjId).setSelectState(false);
      // glm::vec4 rotatedVector =
      //     cam->viewMatrix() *
      //     glm::vec4({uc->cursorVel.x(), uc->cursorVel.y(), 0, 0});
      glm::vec4 rotatedVector;
      rotatedVector =
          glm::rotate(glm::identity<glm::mat4>(), glm::radians(cam->yaw()),
                      glm::vec3(0.0f, 1.0f, 0.0f)) *
          glm::rotate(glm::identity<glm::mat4>(), glm::radians(cam->pitch()),
                      glm::vec3(1.0f, 0.0f, 0.0f)) *
          glm::vec4({uc->cursorVel.x(), uc->cursorVel.y(), 0, 0});
      std::cerr << uc->cursorVel << " \n";
      std::cerr << rotatedVector.x << " " << rotatedVector.y << "  "
                << rotatedVector.z << "\n";
      env->objs()
          .at(uc->selectedObjId)
          .setVel(Vec3(rotatedVector.x, rotatedVector.y, rotatedVector.z));
      uc->selectedObjId = -1;
    }
  }
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

  Environment env(simParams.environment_boundary.x(), simParams.environment_boundary.y(),
                  simParams.environment_boundary.z(),
                  simParams.environment_gravity * simParams.environment_unitsPerMeter,
                  1.0 / simParams.environment_frameRate);

  env.setWind(simParams.environment_wind);
  env.setAirDensity(simParams.environment_airDensity);

  ControlSet ctrlSet;
  simUtils::setupControls(ctrlSet);

  GraphicsTools::InitGraphics();
  GraphicsTools::Window window("Gravity Sim 3D!",
                               simParams.visualization_dimensions.x(),
                               simParams.visualization_dimensions.y());

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
  cam.setPos(glm::vec3(0.0, 0.0, 0.45 * env.bbox().d()));
  GraphicsTools::DirectionalLight light = {
      glm::normalize(glm::vec3(0, -1, 0)), 0.1 * GraphicsTools::Colors::White,
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
  GraphicsTools::Material checkerboard = {GraphicsTools::Colors::White,
                                          &checkerboardTex,
                                          GraphicsTools::Colors::White, 32};
  simUtils::ObjMap staticObjs;
  simUtils::ObjMap ballObjs;

  window.setUserPointer("env", &env);
  window.setUserPointer("phongShader", &phong);
  window.setUserPointer("checkTex", &checkerboardTex);
  window.setUserPointer("checkMat", &checkerboard);
  window.setUserPointer("staticObjMap", &staticObjs);

  simUtils::setupEnvWalls(&window);

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
  simUtils::setupUserCursors(&window, &uc);

  env.setNextId(1000); // keep walls from being deleted

  glfwSetKeyCallback(window.glfwWindow(), keyCallback);
  glfwSetScrollCallback(window.glfwWindow(), scrollCallback);
  glfwSetMouseButtonCallback(window.glfwWindow(), mouseButtonCallback);
  glfwSetInputMode(window.glfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  window.setUserPointer("ballObjMap", &ballObjs);
  window.setUserPointer("ctrlSet", &ctrlSet);
  window.setUserPointer("ballShader", &stripes);
  window.setUserPointer("cam", &cam);
  window.setUserPointer("cursorEmu", &cursorEmu);
  window.setUserPointer("cursorEmuObjId", &cursorEmuObjId);
  window.setUserPointer("userCursor", &uc);

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
      if ((tNow - tLastWindow) / 1000.0 > (1.0 / simParams.visualization_frameRate)) {
        cursorEmu.update();
        handleUserInput(&window);
        handleKeyStates(&window);
        window.getEvents();
        simUtils::drawSim(&window);
        tLastWindow = tNow;
      }
    }
  }

  GraphicsTools::CloseGraphics();

  return 0;
}