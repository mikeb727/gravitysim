/* Three-dimensional bouncing ball simulator playground*/

#include <algorithm>
#include <argparse/argparse.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mb-libs/mbgfx.h>

#include "control.h"
#include "cursor.h"
#include "env3d.h"
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

enum class Tool { None = 0, SizeTool = 1, SpeedTool = 2, SpinTool = 3 };

// cursor data and visibility for now
struct UserCursor {
  CursorData data;
  Tool activeTool;
};

double lastCursorX = 0, lastCursorY = 0;

void drawUserCursor(GraphicsTools::Window *win, UserCursor *uc, int curId) {
  utils::ObjMap *staticObjs =
      (utils::ObjMap *)(win->userPointer("staticObjMap"));
  int ballId = curId;
  int arrowId = curId + 1;
  int spinId = curId + 2;

  CursorData cur = uc->data;
  staticObjs->at(ballId).clearGeometry();
  staticObjs->at(arrowId).clearGeometry();
  staticObjs->at(spinId).clearGeometry();

  if (uc->activeTool == Tool::SizeTool) {
    staticObjs->at(ballId).genSphere(cur.radius * simParams.envScale * 0.9, 16,
                                     16);
    staticObjs->at(ballId).setPos(glm::vec3(cur.ballX, cur.ballY, cur.ballZ));
  } else if (uc->activeTool == Tool::SpeedTool) {
    staticObjs->at(arrowId).genArrow(
        1, 20, cur.ballX, cur.ballY, cur.ballZ, cur.ballX + cur.deltaX,
        cur.ballY + cur.deltaY, cur.ballZ + cur.deltaZ);
  }

  else if (uc->activeTool == Tool::SpinTool) {
    glm::vec3 arb1 = glm::normalize(glm::vec3(0.5, 0.7, 0.9));
    float spinPoints[30];
    for (int i = 0; i < 10; ++i) {
      float pointAngle =
          (cur.angularSpeed > 0 ? 1 : -1) * (1.8 * M_PI * i / 10) +
          (cur.angularSpeed * 0.05 * glfwGetTime());
      float rotationAngle =
          glm::dot({1, 0, 0}, glm::normalize(glm::vec3(cur.angularAxis.x(),
                                                       cur.angularAxis.y(),
                                                       cur.angularAxis.z())));

      glm::vec3 rotationAxis =
          glm::cross({1, 0, 0}, glm::normalize(glm::vec3(cur.angularAxis.x(),
                                                         cur.angularAxis.y(),
                                                         cur.angularAxis.z())));
      std::cerr << rotationAngle << "\n";

      glm::vec3 arrowPoint(
          0, (0.5 + fabs(cur.angularSpeed / 50) + cur.radius) * cos(pointAngle),
          (0.5 + fabs(cur.angularSpeed / 50) + cur.radius) * sin(pointAngle));

      if (glm::length(rotationAxis) > 0) {
        arrowPoint = glm::rotate(std::acos(rotationAngle), rotationAxis) *
                     glm::vec4(arrowPoint, 1.0);
      }

      spinPoints[3 * i] = cur.ballX + arrowPoint.x;
      spinPoints[3 * i + 1] = cur.ballY + arrowPoint.y;
      spinPoints[3 * i + 2] = cur.ballZ + arrowPoint.z;
    }
    staticObjs->at(spinId).genMultiArrow(fmin(1, fabs(cur.angularSpeed / 25)),
                                         20, 10, spinPoints);
  }
}

void drawCursor(GraphicsTools::Window *win, CursorData cur, int curId) {
  utils::ObjMap *staticObjs =
      (utils::ObjMap *)(win->userPointer("staticObjMap"));

  int ballId = curId;
  int arrowId = curId + 1;
  int spinId = curId + 2;

  staticObjs->at(ballId).clearGeometry();
  staticObjs->at(ballId).genSphere(cur.radius * simParams.envScale * 0.5, 16,
                                   16);
  staticObjs->at(ballId).setPos(glm::vec3(cur.ballX, cur.ballY, cur.ballZ));

  staticObjs->at(arrowId).clearGeometry();
  staticObjs->at(arrowId).genArrow(
      1, 20, cur.ballX, cur.ballY, cur.ballZ, cur.ballX + cur.deltaX,
      cur.ballY + cur.deltaY, cur.ballZ + cur.deltaZ);

  staticObjs->at(spinId).clearGeometry();
  float spinPoints[30];
  for (int i = 0; i < 10; ++i) {
    float pointAngle = (cur.angularSpeed > 0 ? 1 : -1) * (1.8 * M_PI * i / 10) +
                       (cur.angularSpeed * 0.05 * glfwGetTime());
    float rotationAngle = glm::dot(
        glm::normalize(glm::vec3(cur.angularAxis.x(), cur.angularAxis.y(),
                                 cur.angularAxis.z())),
        {1, 0, 0});
    glm::vec3 rotationAxis = glm::cross(
        glm::normalize(glm::vec3(cur.angularAxis.x(), cur.angularAxis.y(),
                                 cur.angularAxis.z())),
        {1, 0, 0});
    glm::vec3 arrowPoint(
        (0.5 + fabs(cur.angularSpeed / 50) + cur.radius) * cos(pointAngle),
        (0.5 + fabs(cur.angularSpeed / 50) + cur.radius) * -sin(pointAngle), 0);

    arrowPoint =
        glm::rotate(rotationAngle, rotationAxis) * glm::vec4(arrowPoint, 1.0);

    spinPoints[3 * i] = cur.ballX + arrowPoint.x;
    spinPoints[3 * i + 1] = cur.ballY + arrowPoint.y;
    spinPoints[3 * i + 2] = cur.ballZ + arrowPoint.z;
  }
  staticObjs->at(spinId).genMultiArrow(fmin(1, fabs(cur.angularSpeed / 25)), 20,
                                       10, spinPoints);
}

void drawSim(GraphicsTools::Window *win) {
  utils::ObjMap *staticObjs =
      (utils::ObjMap *)(win->userPointer("staticObjMap"));

  Environment *env = (Environment *)(win->userPointer("env"));
  CursorEmulator *cursorEmu = (CursorEmulator *)(win->userPointer("cursorEmu"));
  UserCursor *uc = (UserCursor *)(win->userPointer("userCursor"));
  utils::ObjMap *envObjs = (utils::ObjMap *)(win->userPointer("ballObjMap"));

  drawUserCursor(win, uc, *(int *)(win->userPointer("userCursorObjId")));

  drawCursor(win, cursorEmu->current,
             *(int *)(win->userPointer("cursorEmuObjId")));

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
  floor.setPos(glm::vec3(0.0, -0.5 * env->bbox().h(), 0.0));
  renderObjSet->emplace(renderObjSet->size(), floor);

  GraphicsTools::RenderObject wall1;
  wall1.setMaterial(*wallMat);
  wall1.setTexture(wallTex);
  wall1.setShader(shader);
  wall1.genPlane(env->bbox().h(), env->bbox().d());
  wall1.setPos(glm::vec3(-0.5 * env->bbox().w(), 0, 0));
  wall1.setRotation(glm::vec3(0, 0, 1), -0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall1);

  GraphicsTools::RenderObject wall2;
  wall2.setMaterial(*wallMat);
  wall2.setTexture(wallTex);
  wall2.setShader(shader);
  wall2.genPlane(env->bbox().h(), env->bbox().d());
  wall2.setPos(glm::vec3(0.5 * env->bbox().w(), 0, 0.0));
  wall2.setRotation(glm::vec3(0, 0, 1), 0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall2);

  GraphicsTools::RenderObject wall3;
  wall3.setMaterial(*wallMat);
  wall3.setTexture(wallTex);
  wall3.setShader(shader);
  wall3.genPlane(env->bbox().w(), env->bbox().h());
  wall3.setPos(glm::vec3(0.0, 0, -0.5 * env->bbox().d()));
  wall3.setRotation(glm::vec3(1, 0, 0), 0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall3);

  GraphicsTools::RenderObject wall4;
  wall4.setMaterial(*wallMat);
  wall4.setTexture(wallTex);
  wall4.setShader(shader);
  wall4.genPlane(env->bbox().w(), env->bbox().h());
  wall4.setPos(glm::vec3(0.0, 0.0, 0.5 * env->bbox().d()));
  wall4.setRotation(glm::vec3(1, 0, 0), -0.5 * M_PI);
  renderObjSet->emplace(renderObjSet->size(), wall4);

  GraphicsTools::RenderObject ceiling;
  ceiling.setMaterial(*wallMat);
  ceiling.setTexture(wallTex);
  ceiling.setShader(shader);
  ceiling.genPlane(env->bbox().w(), env->bbox().d());
  ceiling.setPos(glm::vec3(0.0, 0.5 * env->bbox().h(), 0.0));
  ceiling.setRotation(glm::vec3(1, 0, 0), M_PI);
  renderObjSet->emplace(renderObjSet->size(), ceiling);
}

void handleUserInput(GraphicsTools::Window *win) {
  GraphicsTools::Camera *cam =
      (GraphicsTools::Camera *)(win->userPointer("cam"));
  GLFWwindow *glfwWin = win->glfwWindow();
  if (cam) {
    GraphicsTools::Scene *winScene =
        (GraphicsTools::Scene *)glfwGetWindowUserPointer(glfwWin);
    float panSpeed = glfwGetKey(glfwWin, GLFW_KEY_P) ? 2.0f : 0.5f;
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
      cam->setPitch(
          std::clamp(cam->pitch() + (cam->fov() / 45.0), -85.0, 85.0));
    } else if (glfwGetKey(glfwWin, GLFW_KEY_DOWN)) {
      cam->setPitch(
          std::clamp(cam->pitch() - (cam->fov() / 45.0), -85.0, 85.0));
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
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlSet"));

  // if (glfwGetKey(glfwWin, GLFW_KEY_2)) {
  //   (*ctrls)("vela")++;
  //   std::cerr << (*ctrls)["vela"] << "\n";
  // }
  // if (glfwGetKey(glfwWin, GLFW_KEY_1)) {
  //   (*ctrls)("vela")--;
  //   std::cerr << (*ctrls)["vela"] << "\n";
  // }

  UserCursor *userCursor = (UserCursor *)win->userPointer("userCursor");
  userCursor->data.ballX = cam->pos().x + 10.0 * cam->localForward().x;
  userCursor->data.ballY = cam->pos().y + 10.0 * cam->localForward().y;
  userCursor->data.ballZ = cam->pos().z + 10.0 * cam->localForward().z;
  userCursor->data.arrowX =
      cam->pos().x + (10.0 + 5.0 * (*ctrls)["velx"]) * cam->localForward().x;
  userCursor->data.arrowY =
      cam->pos().y + (10.0 + 5.0 * (*ctrls)["velx"]) * cam->localForward().y;
  userCursor->data.arrowZ =
      cam->pos().z + (10.0 + 5.0 * (*ctrls)["velx"]) * cam->localForward().z;
  userCursor->data.deltaX = userCursor->data.arrowX - userCursor->data.ballX;
  userCursor->data.deltaY = userCursor->data.arrowY - userCursor->data.ballY;
  userCursor->data.deltaZ = userCursor->data.arrowZ - userCursor->data.ballZ;
  userCursor->data.radius = (*ctrls)["radius"];
  userCursor->data.angularAxis =
      Vec3((*ctrls)["angularAxisX"], (*ctrls)["angularAxisY"],
           (*ctrls)["angularAxisZ"]);
  userCursor->data.angularSpeed = (*ctrls)["vela"];
}

void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  CursorData *userCursor = (CursorData *)mbWin->userPointer("userCursor");
  UserCursor *uc = (UserCursor *)(mbWin->userPointer("userCursor"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlSet"));

  if (key == GLFW_KEY_1 && action == GLFW_RELEASE) {
    uc->activeTool =
        uc->activeTool == Tool::SizeTool ? Tool::None : Tool::SizeTool;
  }
  if (key == GLFW_KEY_2 && action == GLFW_RELEASE) {
    uc->activeTool =
        uc->activeTool == Tool::SpeedTool ? Tool::None : Tool::SpeedTool;
  }
  if (key == GLFW_KEY_3 && action == GLFW_RELEASE) {
    uc->activeTool =
        uc->activeTool == Tool::SpinTool ? Tool::None : Tool::SpinTool;
  }
  if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
    switch (uc->activeTool) {
    case Tool::SizeTool:
      (*ctrls)("radius").reset();
      break;
    case Tool::SpeedTool:
      (*ctrls)("velx").reset();
      (*ctrls)("vely").reset();
      (*ctrls)("velz").reset();
      break;
      case Tool::SpinTool:
      (*ctrls)("vela").reset();
      break;
    }
  }
}

void cursorPosCallback(GLFWwindow *win, double x, double y) {}

void scrollCallback(GLFWwindow *win, double x, double y) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlSet"));
  UserCursor *userCursor = (UserCursor *)mbWin->userPointer("userCursor");
  if (userCursor->activeTool == Tool::SizeTool) {
    (*ctrls)("radius").changeValue(0.01 * y);
  }
  if (userCursor->activeTool == Tool::SpeedTool) {
    (*ctrls)("velx").changeValue(y);
    if ((*ctrls)["velx"] < 0) {
      (*ctrls)("velx").setValue(0);
    }
  }
  if (userCursor->activeTool == Tool::SpinTool) {
    (*ctrls)("vela").changeValue(y);
  }
}

void mouseButtonCallback(GLFWwindow *win, int button, int action, int m) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  CursorData *userCursor = (CursorData *)mbWin->userPointer("userCursor");

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    ControlSet *ctrls =
        static_cast<ControlSet *>(mbWin->userPointer("ctrlSet"));
    utils::ObjMap *om =
        static_cast<utils::ObjMap *>(mbWin->userPointer("ballObjMap"));
    GraphicsTools::ShaderProgram *shader =
        static_cast<GraphicsTools::ShaderProgram *>(
            mbWin->userPointer("ballShader"));

    // place relative to camera pos
    GraphicsTools::Camera *cam =
        (GraphicsTools::Camera *)mbWin->userPointer("cam");

    bool objAtCandPos = false;

    Vec3 candidateObjPos(userCursor->ballX, userCursor->ballY,
                         userCursor->ballZ);

    Vec3 candidateObjVel((*ctrls)["velx"] * cam->localForward().x,
                         (*ctrls)["velx"] * cam->localForward().y,
                         (*ctrls)["velx"] * cam->localForward().z);

    for (auto &obj : env->objs()) {
      if (obj.second.bbox().containsPoint(candidateObjPos)) {
        objAtCandPos = true;
      }
      obj.second.setSelectState(false);
    }

    if (!objAtCandPos) {
      bool noOverlap = true;
      for (auto &obj : env->objs()) {
        if (obj.second.bbox().intersects(
                BBox(candidateObjPos,
                     (*ctrls)["radius"] * 2.0 * simParams.envScale))) {
          noOverlap = false;
          break;
        }
      }
      if (noOverlap && env->bbox().containsBBox(
                           BBox(candidateObjPos, (*ctrls)["radius"] * 2.0 *
                                                     simParams.envScale))) {
        env->addObj(Ball(BBox(candidateObjPos,
                              (*ctrls)["radius"] * 2.0 * simParams.envScale),
                         pow((*ctrls)["radius"] * simParams.envScale, 3),
                         candidateObjPos, candidateObjVel * simParams.envScale,
                         1,
                         (*ctrls)["vela"] * Vec3((*ctrls)["angularAxisX"],
                                                 (*ctrls)["angularAxisY"],
                                                 (*ctrls)["angularAxisZ"])));
        GraphicsTools::Material mat = {GraphicsTools::randomColor(), NULL,
                                       0.5 * GraphicsTools::Colors::White, 4};
        om->emplace(env->lastObjId(), GraphicsTools::RenderObject());
        om->at(env->lastObjId()).setShader(shader);
        om->at(env->lastObjId()).setMaterial(mat);
        om->at(env->lastObjId())
            .genSphere((*ctrls)["radius"] * simParams.envScale, 16, 16);
        om->at(env->lastObjId())
            .setPos(glm::vec3(candidateObjPos.x(), candidateObjPos.y(),
                              candidateObjPos.z()));
        mbWin->activeScene()->addRenderObject(&om->at(env->lastObjId()));
      }
    }
  }
}

bool handleKeyStates(GraphicsTools::Window *win) {

  // If the escape key is pressed, signal to quit simulation
  glfwSetWindowShouldClose(win->glfwWindow(),
                           glfwGetKey(win->glfwWindow(), GLFW_KEY_ESCAPE));

  return false;
}

void handleMouseLook(GraphicsTools::Window *win) {
  double newX, newY;
  glfwGetCursorPos(win->glfwWindow(), &newX, &newY);
  GraphicsTools::Camera *cam = (GraphicsTools::Camera *)win->userPointer("cam");

  if (glfwGetMouseButton(win->glfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) ==
      GLFW_PRESS) {

  } else {
    if (cam) {
      cam->setPitch(std::clamp(
          cam->pitch() - 0.1 * std::clamp(newY - lastCursorY, -40.0, 40.0),
          -85.0, 85.0));
      cam->setYaw(cam->yaw() -
                  0.1 * std::clamp(newX - lastCursorX, -40.0, 40.0));
    }
  }

  lastCursorX = newX;
  lastCursorY = newY;
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

  cam.setPerspective(55.0, (double)window.width() / window.height());

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
  ctrlSet.addCtrl("angularAxisX", Control("Angular axis X", -1, 1, 0.01,
                                          simParams.ctrlAngularAxis[0]));
  ctrlSet.addCtrl("angularAxisY", Control("Angular axis Y", -1, 1, 0.01,
                                          simParams.ctrlAngularAxis[1]));
  ctrlSet.addCtrl("angularAxisZ", Control("Angular axis Z", -1, 1, 0.01,
                                          simParams.ctrlAngularAxis[2]));
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

  cam.setPos(glm::vec3(0.0, 0.0, 1.25 * env.bbox().d()));

  CursorEmulator cursorEmu(&window);
  window.setUserPointer("cursorEmu", &cursorEmu);
  int cursorEmuObjId = staticObjs.size();
  window.setUserPointer("cursorEmuObjId", &cursorEmuObjId);
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

  UserCursor uc;
  uc.activeTool = Tool::None;

  window.setUserPointer("userCursor", &uc);
  int userCursorObjId = staticObjs.size();
  window.setUserPointer("userCursorObjId", &userCursorObjId);

  GraphicsTools::RenderObject userCursorDisplay;
  userCursorDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Blue, 0.2,
                            GraphicsTools::Colors::White, 0.8),
       NULL, GraphicsTools::Colors::White, 2}));
  userCursorDisplay.setShader(&phong);
  staticObjs.emplace(userCursorObjId, userCursorDisplay);
  sc.addRenderObject(&staticObjs.at(userCursorObjId));

  GraphicsTools::RenderObject userArrowDisplay;
  userArrowDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Red, 0.2,
                            GraphicsTools::Colors::White, 0.8),
       NULL, GraphicsTools::Colors::White, 2}));
  userArrowDisplay.setShader(&phong);
  staticObjs.emplace(userCursorObjId + 1, userArrowDisplay);
  sc.addRenderObject(&staticObjs.at(userCursorObjId + 1));

  GraphicsTools::RenderObject userSpinDisplay;
  userSpinDisplay.setMaterial(GraphicsTools::Material(
      {GraphicsTools::blend(GraphicsTools::Colors::Green, 0.2,
                            GraphicsTools::Colors::White, 0.8),
       NULL, GraphicsTools::Colors::White, 2}));
  userSpinDisplay.setShader(&phong);
  staticObjs.emplace(userCursorObjId + 2, userSpinDisplay);
  sc.addRenderObject(&staticObjs.at(userCursorObjId + 2));

  env.setNextId(1000); // keep walls from being deleted

  sc.setupShadows();

  glfwSetKeyCallback(window.glfwWindow(), keyCallback);
  glfwSetScrollCallback(window.glfwWindow(), scrollCallback);
  glfwSetMouseButtonCallback(window.glfwWindow(), mouseButtonCallback);

  glfwSetInputMode(window.glfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  while (!window.shouldClose()) {
    double tNow = utils::computeTNow();

    if ((tNow - tLastEnv) / 1000.0 > (1.0 / simParams.envFrameRate)) {
      env.update();
      tLastEnv = tNow;

      // only update window at framerate
      if ((tNow - tLastWindow) / 1000.0 > (1.0 / simParams.windowFrameRate)) {
        cursorEmu.update();
        handleUserInput(&window);
        handleKeyStates(&window);
        handleMouseLook(&window);
        window.getEvents();
        drawSim(&window);
        tLastWindow = tNow;
      }
    }
  }

  GraphicsTools::CloseGraphics();

  return 0;
}