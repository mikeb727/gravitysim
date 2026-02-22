#include "utility.h"
#include "ball.h"
#include "bbox.h"
#include "control.h"
#include "env3d.h"

#include <chrono>
#include <format>
#include <iostream>
#include <mb-libs/colors.h>

namespace simUtils {

double computeTNow() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

double scalarEuler(const double &v, const double &dv, double dt) {
  return v + (dv * dt);
}

double scalarRk4(const double &v, const double &dv, const double &ddv,
                 double dt) {
  double k1 = dv;
  double k2 = scalarEuler(k1, dv + (0.5 * dt * ddv), 0.5 * dt);
  double k3 = scalarEuler(k2, dv + (0.5 * dt * ddv), 0.5 * dt);
  double k4 = scalarEuler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 2 * k2 + 2 * k3 + k4) / 6));
}

// only allow obj creation in bounds with no intersections
void createObj(GraphicsTools::Window &win, Vec3 candidateObjPos,
               Vec3 candidateObjVel) {
  Environment *env = static_cast<Environment *>(win.userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(win.userPointer("ctrlSet"));
  simUtils::ObjMap *objMap =
      static_cast<simUtils::ObjMap *>(win.userPointer("ballObjMap"));
  GraphicsTools::ShaderProgram *shader =
      static_cast<GraphicsTools::ShaderProgram *>(
          win.userPointer("ballShader"));
  if (objIdAtEnvPos(candidateObjPos, *env,
                    (*ctrls)["radius"] * 2.0 *
                        simParams.environment_unitsPerMeter) == -1 &&
      env->computeOutsideEnv(candidateObjPos,
                             (*ctrls)["radius"] * 2.0 *
                                 simParams.environment_unitsPerMeter)
              .mag() <= 0) {
    env->addObj(
        Ball(BBox(candidateObjPos, (*ctrls)["radius"] * 2.0 *
                                       simParams.environment_unitsPerMeter),
             pow((*ctrls)["radius"] * simParams.environment_unitsPerMeter, 3),
             candidateObjPos,
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
        .genSphere((*ctrls)["radius"] * simParams.environment_unitsPerMeter, 16,
                   16);
    objMap->at(env->lastObjId())
        .setPos(glm::vec3(candidateObjPos.x(), candidateObjPos.y(),
                          candidateObjPos.z()));
    win.activeScene()->addRenderObject(&objMap->at(env->lastObjId()));
  }
}

void drawUserCursor(GraphicsTools::Window &win, simUtils::UserCursor *uc,
                    bool insideEnv, int objIdAtCursor) {
  simUtils::ObjMap *staticObjs =
      (simUtils::ObjMap *)(win.userPointer("staticObjMap"));
  int ballId = uc->baseRenderObjId;
  int arrowId = uc->baseRenderObjId + 1;
  int spinId = uc->baseRenderObjId + 2;
  int crosshairIdX = uc->baseRenderObjId + 3;
  int crosshairIdY = uc->baseRenderObjId + 4;
  int crosshairIdZ = uc->baseRenderObjId + 5;

  GraphicsTools::ColorRgba grabColor{0.9, 0.9, 0.9, 1.0};
  GraphicsTools::ColorRgba sizeColor{0.0, 0.0, 0.7, 1.0};
  GraphicsTools::ColorRgba speedColor{0.7, 0.0, 0.0, 1.0};
  GraphicsTools::ColorRgba spinColor{0.0, 0.7, 0.0, 1.0};
  GraphicsTools::ColorRgba kickColor{0.7, 0.7, 0.0, 1.0};
  GraphicsTools::ColorRgba outsideColor{0.4, 0.4, 0.4, 1.0};

  GraphicsTools::Material cursorMat = {outsideColor, NULL,
                                       GraphicsTools::Colors::White, 2};

  switch (uc->activeTool) {
  case simUtils::Tool::GrabTool:
    cursorMat.diffuse = grabColor;
    break;
  case simUtils::Tool::SizeTool:
    cursorMat.diffuse = sizeColor;
    break;
  case simUtils::Tool::SpeedTool:
    cursorMat.diffuse = speedColor;
    break;
  case simUtils::Tool::SpinTool:
    cursorMat.diffuse = spinColor;
    break;
  case simUtils::Tool::PushTool:
    cursorMat.diffuse = kickColor;
    break;
  }

  if (!insideEnv) {
    cursorMat.diffuse = outsideColor;
  }

  staticObjs->at(ballId).clearGeometry();
  staticObjs->at(arrowId).clearGeometry();
  staticObjs->at(spinId).clearGeometry();
  staticObjs->at(crosshairIdX).clearGeometry();
  staticObjs->at(crosshairIdY).clearGeometry();
  staticObjs->at(crosshairIdZ).clearGeometry();

  staticObjs->at(ballId).setMaterial(cursorMat);
  staticObjs->at(arrowId).setMaterial(cursorMat);
  staticObjs->at(spinId).setMaterial(cursorMat);
  staticObjs->at(crosshairIdX).setMaterial(cursorMat);
  staticObjs->at(crosshairIdY).setMaterial(cursorMat);
  staticObjs->at(crosshairIdZ).setMaterial(cursorMat);
  CursorData cur = uc->data;

  float crosshairHalfLength =
      (objIdAtCursor != -1 || uc->forwardObjId != -1) ? 1.8 : 0.3;
  staticObjs->at(crosshairIdX)
      .genLine(0.15, 20, cur.ballX - crosshairHalfLength, cur.ballY, cur.ballZ,
               cur.ballX + crosshairHalfLength, cur.ballY, cur.ballZ);
  staticObjs->at(crosshairIdY)
      .genLine(0.15, 20, cur.ballX, cur.ballY - crosshairHalfLength, cur.ballZ,
               cur.ballX, cur.ballY + crosshairHalfLength, cur.ballZ);
  staticObjs->at(crosshairIdZ)
      .genLine(0.15, 20, cur.ballX, cur.ballY, cur.ballZ - crosshairHalfLength,
               cur.ballX, cur.ballY, cur.ballZ + crosshairHalfLength);

  switch (uc->activeTool) {
  case simUtils::Tool::SizeTool:
    staticObjs->at(ballId).genSphere(
        cur.radius * simParams.environment_unitsPerMeter * 0.9, 16, 16);
    staticObjs->at(ballId).setPos(glm::vec3(cur.ballX, cur.ballY, cur.ballZ));
    break;
  case simUtils::Tool::SpeedTool:
    staticObjs->at(arrowId).genArrow(
        1, 20, cur.ballX, cur.ballY, cur.ballZ, cur.ballX + cur.deltaX,
        cur.ballY + cur.deltaY, cur.ballZ + cur.deltaZ);
    break;
  case simUtils::Tool::SpinTool:
    float spinRingPoints[30];
    populateSpinRingPoints(cur.angularSpeed, cur.radius,
                           Vec3(cur.ballX, cur.ballY, cur.ballZ),
                           cur.angularAxis, spinRingPoints, 30);
    staticObjs->at(spinId).genMultiArrow(fmin(1, fabs(cur.angularSpeed / 25)),
                                         20, 10, spinRingPoints);
    break;
  case simUtils::Tool::PushTool:
    staticObjs->at(arrowId).genArrow(
        1, 20, cur.ballX, cur.ballY, cur.ballZ, cur.ballX + cur.deltaX,
        cur.ballY + cur.deltaY, cur.ballZ + cur.deltaZ);
    break;
  }
}

void drawCursor(GraphicsTools::Window &win, CursorData cur, int curGfxId) {
  simUtils::ObjMap *staticObjs =
      (simUtils::ObjMap *)(win.userPointer("staticObjMap"));
  int ballId = curGfxId;
  int arrowId = curGfxId + 1;
  int spinId = curGfxId + 2;

  staticObjs->at(ballId).clearGeometry();
  staticObjs->at(arrowId).clearGeometry();
  staticObjs->at(spinId).clearGeometry();

  staticObjs->at(ballId).genSphere(
      cur.radius * simParams.environment_unitsPerMeter * 0.5, 16, 16);
  staticObjs->at(ballId).setPos(glm::vec3(cur.ballX, cur.ballY, cur.ballZ));

  staticObjs->at(arrowId).genArrow(
      1, 20, cur.ballX, cur.ballY, cur.ballZ, cur.ballX + cur.deltaX,
      cur.ballY + cur.deltaY, cur.ballZ + cur.deltaZ);

  float spinRingPoints[30];
  populateSpinRingPoints(cur.angularSpeed, cur.radius,
                         Vec3(cur.ballX, cur.ballY, cur.ballZ), cur.angularAxis,
                         spinRingPoints, 30);
  staticObjs->at(spinId).genMultiArrow(fmin(1, fabs(cur.angularSpeed / 25)), 20,
                                       10, spinRingPoints);
}

void populateSpinRingPoints(float spinSpeed, float ringRadius, Vec3 ringOrigin,
                            Vec3 axis, float *destArray, int arraySize) {
  for (int i = 0; i < 10; ++i) {
    float pointAngle = (spinSpeed > 0 ? 1 : -1) * (1.8 * M_PI * i / 10) +
                       (spinSpeed * 0.05 * glfwGetTime());
    float rotationAngle = glm::dot(
        {1, 0, 0}, glm::normalize(glm::vec3(axis.x(), axis.y(), axis.z())));

    glm::vec3 rotationAxis = glm::cross(
        {1, 0, 0}, glm::normalize(glm::vec3(axis.x(), axis.y(), axis.z())));

    glm::vec3 arrowPoint(
        0, (0.5 + fabs(spinSpeed / 50) + ringRadius) * cos(pointAngle),
        (0.5 + fabs(spinSpeed / 50) + ringRadius) * sin(pointAngle));

    if (glm::length(rotationAxis) > 0) {
      arrowPoint = glm::rotate(std::acos(rotationAngle), rotationAxis) *
                   glm::vec4(arrowPoint, 1.0);
    }

    destArray[3 * i] = ringOrigin.x() + arrowPoint.x;
    destArray[3 * i + 1] = ringOrigin.y() + arrowPoint.y;
    destArray[3 * i + 2] = ringOrigin.z() + arrowPoint.z;
  }
}

void drawSim(GraphicsTools::Window &win) {
  simUtils::ObjMap *staticObjs =
      (simUtils::ObjMap *)(win.userPointer("staticObjMap"));

  Environment *env = (Environment *)(win.userPointer("env"));
  CursorEmulator *cursorEmu = (CursorEmulator *)(win.userPointer("cursorEmu"));
  simUtils::UserCursor *uc =
      (simUtils::UserCursor *)(win.userPointer("userCursor"));
  simUtils::ObjMap *envObjs =
      (simUtils::ObjMap *)(win.userPointer("ballObjMap"));
  ControlSet *ctrls = static_cast<ControlSet *>(win.userPointer("ctrlSet"));
  GraphicsTools::Font *font =
      static_cast<GraphicsTools::Font *>(win.userPointer("font"));

  bool insideEnv =
      env->computeOutsideEnv(
             Vec3(uc->data.ballX, uc->data.ballY, uc->data.ballZ), 0.0)
          .mag() == 0;

  int objIdAtCursor =
      objIdAtEnvPos(Vec3(uc->data.ballX, uc->data.ballY, uc->data.ballZ), *env);

  simUtils::drawUserCursor(
      win, uc,
      insideEnv &&
          (uc->activeTool == Tool::PushTool ||
           objIdAtEnvPos(Vec3(uc->data.ballX, uc->data.ballY, uc->data.ballZ),
                         *env,
                         (*ctrls)["radius"] * 2.0 *
                             simParams.environment_unitsPerMeter) == -1),
      objIdAtCursor);
  simUtils::drawCursor(win, cursorEmu->current,
                       *(int *)(win.userPointer("cursorEmuObjId")));

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

  win.clear();
  win.activeScene()->render(); // all objs
  std::string toolText = "press (key) to de/activate tool\n'r' to reset\n";
  toolText +=
      std::format("{}grab\n", uc->activeTool == Tool::GrabTool ? "> " : "");
  toolText +=
      std::format("{}size (1)\n", uc->activeTool == Tool::SizeTool ? "> " : "");
  toolText += std::format("{}speed (2)\n",
                          uc->activeTool == Tool::SpeedTool ? "> " : "");
  toolText +=
      std::format("{}spin (3)\n", uc->activeTool == Tool::SpinTool ? "> " : "");
  toolText +=
      std::format("{}kick (4)", uc->activeTool == Tool::PushTool ? "> " : "");

  if (font) {
    win.drawText(toolText, font, GraphicsTools::Colors::Yellow,
                 GraphicsTools::Colors::Black, win.width() - 50, 400, 0, -1,
                 GraphicsTools::TextAlignModeH::Right);
  }

  win.update();
}

void removeEnvObj(GraphicsTools::Window &win, int objId) {
  if (objId == -1)
    return;
  Environment *env = static_cast<Environment *>(win.userPointer("env"));
  ObjMap *objMap = static_cast<ObjMap *>(win.userPointer("ballObjMap"));
  env->removeObj(objId);
  objMap->erase(objId);
  win.activeScene()->removeRenderObject(objId);
}

void clearEnvObjs(GraphicsTools::Window &win) {
  Environment *env = static_cast<Environment *>(win.userPointer("env"));
  ObjMap *objMap = static_cast<ObjMap *>(win.userPointer("ballObjMap"));
  auto firstObjKey = objMap->begin()->first;
  env->clearObjs();
  objMap->clear();
  // assume all obj IDs >= 1000
  auto renderObjList = win.activeScene()->objs();
  renderObjList->erase(renderObjList->find(firstObjKey), renderObjList->end());
}

void setupUserCursors(GraphicsTools::Window &win, UserCursor *uc) {
  simUtils::ObjMap *staticObjMap =
      (simUtils::ObjMap *)(win.userPointer("staticObjMap"));
  GraphicsTools::ShaderProgram *phong =
      (GraphicsTools::ShaderProgram *)(win.userPointer("phongShader"));

  uc->activeTool = simUtils::Tool::GrabTool;

  uc->baseRenderObjId = staticObjMap->size();

  GraphicsTools::RenderObject userSizeDisplay;
  userSizeDisplay.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId, userSizeDisplay);
  win.activeScene()->addRenderObject(&staticObjMap->at(uc->baseRenderObjId));

  GraphicsTools::RenderObject userSpeedDisplay;
  userSpeedDisplay.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 1, userSpeedDisplay);
  win.activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 1));

  GraphicsTools::RenderObject userSpinDisplay;
  userSpinDisplay.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 2, userSpinDisplay);
  win.activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 2));

  // small 3D crosshair if no tool is active
  GraphicsTools::RenderObject userCrosshairX;
  userCrosshairX.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 3, userCrosshairX);
  win.activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 3));
  GraphicsTools::RenderObject userCrosshairY;
  userCrosshairY.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 4, userCrosshairY);
  win.activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 4));
  GraphicsTools::RenderObject userCrosshairZ;
  userCrosshairZ.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 5, userCrosshairZ);
  win.activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 5));
}

void setupControls(ControlSet &ctrlSet) {
  ctrlSet.addCtrl("radius", Control("Radius", simParams.controls_radius[0],
                                    simParams.controls_radius[1],
                                    simParams.controls_radius[2],
                                    simParams.controls_radius[3]));
  ctrlSet.addCtrl("velPitch", Control("Forward velocity",
                                      simParams.controls_velocityPitch[0],
                                      simParams.controls_velocityPitch[1],
                                      simParams.controls_velocityPitch[2],
                                      simParams.controls_velocityPitch[3]));
  ctrlSet.addCtrl("velYaw",
                  Control("X velocity", simParams.controls_velocityYaw[0],
                          simParams.controls_velocityYaw[1],
                          simParams.controls_velocityYaw[2],
                          simParams.controls_velocityYaw[3]));
  ctrlSet.addCtrl("velMag",
                  Control("Y velocity", simParams.controls_velocityMag[0],
                          simParams.controls_velocityMag[1],
                          simParams.controls_velocityMag[2],
                          simParams.controls_velocityMag[3]));
  ctrlSet.addCtrl("vela", Control("Angular velocity",
                                  simParams.controls_velocityAngular[0],
                                  simParams.controls_velocityAngular[1],
                                  simParams.controls_velocityAngular[2],
                                  simParams.controls_velocityAngular[3]));
  ctrlSet.addCtrl("angularAxisX", Control("Angular axis X", -4, 4, 0.01,
                                          simParams.controls_angularAxis.x()));
  ctrlSet.addCtrl("angularAxisY", Control("Angular axis Y", -4, 4, 0.01,
                                          simParams.controls_angularAxis.y()));
  ctrlSet.addCtrl("angularAxisZ", Control("Angular axis Z", -4, 4, 0.01,
                                          simParams.controls_angularAxis.z()));
  ctrlSet.addCtrl("kickPitch",
                  Control("Kick force", simParams.controls_kickPitch[0],
                          simParams.controls_kickPitch[1],
                          simParams.controls_kickPitch[2],
                          simParams.controls_kickPitch[3]));
  ctrlSet.addCtrl("kickYaw",
                  Control("Kick angle (up/down)", simParams.controls_kickYaw[0],
                          simParams.controls_kickYaw[1],
                          simParams.controls_kickYaw[2],
                          simParams.controls_kickYaw[3]));
  ctrlSet.addCtrl("kickMag",
                  Control("Kick direction", simParams.controls_kickMag[0],
                          simParams.controls_kickMag[1],
                          simParams.controls_kickMag[2],
                          simParams.controls_kickMag[3]));
}

// whether ball at pos overlaps with any other ball
// point test for zero radius
int objIdAtEnvPos(Vec3 pos, Environment &env, float radius) {
  for (auto &obj : env.objs()) {
    if (radius == 0.0) {
      if (obj.second.bbox().containsPoint(pos)) {
        return obj.first;
      }
    } else {
      if (obj.second.bbox().intersects(BBox(pos, radius))) {
        return obj.first;
      }
    }
  }
  return -1;
}

Vec3 glmToVec3(glm::vec3 v) { return Vec3(v.x, v.y, v.z); }

void handleUserInput(GraphicsTools::Window &win) {
  GLFWwindow *glfwWin = win.glfwWindow();
  GraphicsTools::Camera *cam =
      static_cast<GraphicsTools::Camera *>(win.userPointer("cam"));
  ControlSet *ctrls = static_cast<ControlSet *>(win.userPointer("ctrlSet"));
  simUtils::UserCursor *userCursor =
      static_cast<simUtils::UserCursor *>(win.userPointer("userCursor"));
  Environment *env = static_cast<Environment *>(win.userPointer("env"));

  if (!cam || !ctrls || !userCursor || !env) {
    std::cerr << "some window user pointers are null!\n"
                 "make sure ALL the window's user pointers are set!\n"
                 "stopping sim\n";
    win.setShouldClose(true);
    return;
  }

  // camera movement
  float panSpeed = glfwGetKey(glfwWin, GLFW_KEY_LEFT_SHIFT) ? 1.0 : 0.5f;
  // up, down
  if (glfwGetKey(glfwWin, simParams.input_up)) {
    cam->setPos(cam->pos() + panSpeed * cam->localUp());
  } else if (glfwGetKey(glfwWin, simParams.input_down)) {
    cam->setPos(cam->pos() - panSpeed * cam->localUp());
  }
  // left, right
  if (glfwGetKey(glfwWin, simParams.input_right)) {
    cam->setPos(cam->pos() + panSpeed * cam->localRight());
  } else if (glfwGetKey(glfwWin, simParams.input_left)) {
    cam->setPos(cam->pos() - panSpeed * cam->localRight());
  }
  // forward, backward
  if (glfwGetKey(glfwWin, simParams.input_forward)) {
    cam->setPos(cam->pos() + panSpeed * cam->localForward());
  } else if (glfwGetKey(glfwWin, simParams.input_backward)) {
    cam->setPos(cam->pos() - panSpeed * cam->localForward());
  }
  // zoom
  if (cam->projection() == GraphicsTools::CameraProjType::Perspective) {
    if (glfwGetKey(glfwWin, simParams.input_zoomIn)) {
      cam->setPerspective(std::clamp(cam->fov() - 0.5f, 5.0f, 60.0f),
                          cam->aspectRatio());
    } else if (glfwGetKey(glfwWin, simParams.input_zoomOut)) {
      cam->setPerspective(std::clamp(cam->fov() + 0.5f, 5.0f, 60.0f),
                          cam->aspectRatio());
    }
  }

  // get cursor velocity
  double newX, newY;
  glfwGetCursorPos(win.glfwWindow(), &newX, &newY);
  // TODO add screen position to cursor and use that for velocity
  userCursor->cursorVel =
      Vec3(newX - userCursor->prev.ballX, userCursor->prev.ballY - newY);

  // use for any vector requiring rotation to camera frame
  glm::vec4 rotatedVector;

  // right-click dragging for tool manipulation
  if (glfwGetMouseButton(win.glfwWindow(), GLFW_MOUSE_BUTTON_RIGHT) ==
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
                                         userCursor->cursorVel.y(), 0, 0});
      (*ctrls)("velPitch").changeValue(0.005 * rotatedVector.y);
      (*ctrls)("velYaw").changeValue(
          (userCursor->speedToolFacingCamera ? 1.0 : -1.0) * 0.005 *
          rotatedVector.x);
      userCursor->candidateObjVel =
          Vec3((*ctrls)["velMag"] * cos((*ctrls)["velPitch"]) *
                   sin((*ctrls)["velYaw"]),
               (*ctrls)["velMag"] * sin((*ctrls)["velPitch"]),
               (*ctrls)["velMag"] * cos((*ctrls)["velPitch"]) *
                   cos((*ctrls)["velYaw"]));
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
    case simUtils::Tool::PushTool:
      // create xy vector from click-drag with z set to zero
      // apply camera rotation matrix
      rotatedVector =
          cam->viewMatrix() * glm::vec4({userCursor->cursorVel.x(),
                                         userCursor->cursorVel.y(), 0, 0});
      (*ctrls)("kickPitch").changeValue(0.005 * rotatedVector.y);
      (*ctrls)("kickYaw").changeValue(
          (userCursor->kickToolFacingCamera ? 1.0 : -1.0) * 0.005 *
          rotatedVector.x);
      userCursor->objKickVel =
          Vec3((*ctrls)["kickMag"] * cos((*ctrls)["kickPitch"]) *
                   sin((*ctrls)["kickYaw"]),
               (*ctrls)["kickMag"] * sin((*ctrls)["kickPitch"]),
               (*ctrls)["kickMag"] * cos((*ctrls)["kickPitch"]) *
                   cos((*ctrls)["kickYaw"]));
      break;
    }
  } else {
    // mouselook if not right-click dragging
    if (cam) {
      cam->setPitch(
          std::clamp(cam->pitch() - 0.1 * std::clamp(-userCursor->cursorVel.y(),
                                                     -40.0, 40.0),
                     -95.0, 95.0));
      cam->setYaw(cam->yaw() -
                  0.1 * std::clamp(userCursor->cursorVel.x(), -40.0, 40.0));
    }
  }

  //
  if (userCursor->selectedObjId != -1) {
    env->objs()
        .at(userCursor->selectedObjId)
        .setPos(Vec3(userCursor->data.ballX, userCursor->data.ballY,
                     userCursor->data.ballZ) +
                userCursor->objSelectionOffset);
  }

  // revisit? newX and newY are 2D
  userCursor->prev.ballX = newX;
  userCursor->prev.ballY = newY;

  // we just changed camera position, so update user's cursor

  // only get the obj ID in front of the camera if using the grab tool
  if (userCursor->activeTool != simUtils::Tool::GrabTool &&
      userCursor->activeTool != simUtils::Tool::PushTool) {
    userCursor->forwardObjId = -1;
    userCursor->closestForwardDistance = 10.0;
  }

  // by default, cursor appears 10 units in front of camera
  // if grab tool is active and ball is in front of camera, snap cursor to the
  // object
  // if object is selected, snap object to cursor
  Vec3 baseCursorPos = simUtils::glmToVec3(cam->pos()) +
                       userCursor->closestForwardDistance *
                           simUtils::glmToVec3(cam->localForward());

  // find the closest ball directly in front of the camera
  if ((userCursor->activeTool == simUtils::Tool::GrabTool ||
       userCursor->activeTool == simUtils::Tool::PushTool) &&
      userCursor->selectedObjId == -1) {
    double maxDistance = 50.0;
    userCursor->forwardObjId = -1;
    for (auto &obj : env->objs()) {
      double dotProductTest =
          simUtils::glmToVec3(cam->localForward())
              .unit()
              .dot((obj.second.bbox().pos() - simUtils::glmToVec3(cam->pos()))
                       .unit());
      double lineSphereTest =
          std::pow(simUtils::glmToVec3(cam->localForward())
                       .unit()
                       .dot(simUtils::glmToVec3(cam->pos()) -
                            obj.second.bbox().pos()),
                   2) -
          std::pow(
              Vec3((simUtils::glmToVec3(cam->pos())) - obj.second.bbox().pos())
                  .mag(),
              2) +
          std::pow(obj.second.bbox().w() * 0.5, 2);
      double dist =
          (obj.second.bbox().pos() - simUtils::glmToVec3(cam->pos())).mag();
      if (lineSphereTest > -3 && dotProductTest > 0.99 && dist < maxDistance) {
        maxDistance = dist;
        userCursor->closestForwardDistance = dist;
        userCursor->forwardObjId = obj.first;
      }
    }

    if ((userCursor->forwardObjId == -1 && userCursor->selectedObjId == -1)) {
      userCursor->closestForwardDistance = 10.0;
    }

    if ((userCursor->selectedObjId == -1 && userCursor->forwardObjId != -1)) {
      baseCursorPos = env->objs().at(userCursor->forwardObjId).bbox().pos();
    }
  }
  userCursor->data.ballX = baseCursorPos.x();
  userCursor->data.ballY = baseCursorPos.y();
  userCursor->data.ballZ = baseCursorPos.z();

  if (userCursor->activeTool == simUtils::Tool::SpeedTool) {
    userCursor->data.arrowX = baseCursorPos.x() + (0.75 * (*ctrls)["velMag"] *
                                                   cos((*ctrls)["velPitch"]) *
                                                   sin((*ctrls)["velYaw"]));
    userCursor->data.arrowY = baseCursorPos.y() + (0.75 * (*ctrls)["velMag"] *
                                                   sin((*ctrls)["velPitch"]));
    userCursor->data.arrowZ = baseCursorPos.z() + (0.75 * (*ctrls)["velMag"] *
                                                   cos((*ctrls)["velPitch"]) *
                                                   cos((*ctrls)["velYaw"]));
  } else if (userCursor->activeTool == simUtils::Tool::PushTool) {
    userCursor->data.arrowX = baseCursorPos.x() + userCursor->objKickVel.x();
    userCursor->data.arrowY = baseCursorPos.y() + userCursor->objKickVel.y();
    userCursor->data.arrowZ = baseCursorPos.z() + userCursor->objKickVel.z();
  }

  userCursor->data.deltaX = userCursor->data.arrowX - userCursor->data.ballX;
  userCursor->data.deltaY = userCursor->data.arrowY - userCursor->data.ballY;
  userCursor->data.deltaZ = userCursor->data.arrowZ - userCursor->data.ballZ;
  userCursor->data.radius = (*ctrls)["radius"];
  userCursor->data.angularAxis =
      Vec3((*ctrls)["angularAxisX"], (*ctrls)["angularAxisY"],
           (*ctrls)["angularAxisZ"]);
  userCursor->data.angularSpeed = (*ctrls)["vela"];

  // If the escape key is pressed, signal to quit simulation
  glfwSetWindowShouldClose(win.glfwWindow(),
                           glfwGetKey(win.glfwWindow(), GLFW_KEY_ESCAPE));

  win.getEvents();
}

// keypresses, NOT key states
void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  simUtils::UserCursor *uc =
      (simUtils::UserCursor *)(mbWin->userPointer("userCursor"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlSet"));
  GraphicsTools::Camera *cam =
      static_cast<GraphicsTools::Camera *>(mbWin->userPointer("cam"));

  if (key == simParams.input_sizeTool && action == GLFW_PRESS) {
    uc->activeTool = uc->activeTool == simUtils::Tool::SizeTool
                         ? simUtils::Tool::GrabTool
                         : simUtils::Tool::SizeTool;
  }
  if (key == simParams.input_speedTool && action == GLFW_PRESS) {
    uc->activeTool = uc->activeTool == simUtils::Tool::SpeedTool
                         ? simUtils::Tool::GrabTool
                         : simUtils::Tool::SpeedTool;
    if (uc->activeTool == simUtils::Tool::SpeedTool) {
      (*ctrls)("velPitch").setValue(cam->pitch() * M_PI / 180.0);
      (*ctrls)("velYaw").setValue(M_PI + cam->yaw() * M_PI / 180.0);
      (*ctrls)("velMag").setValue(10);
      uc->candidateObjVel =
          Vec3((*ctrls)["velMag"] * cos((*ctrls)["velPitch"]) *
                   sin((*ctrls)["velYaw"]),
               (*ctrls)["velMag"] * sin((*ctrls)["velPitch"]),
               (*ctrls)["velMag"] * cos((*ctrls)["velPitch"]) *
                   cos((*ctrls)["velYaw"]));
    }
  }
  if (key == simParams.input_spinTool && action == GLFW_PRESS) {
    uc->activeTool = uc->activeTool == simUtils::Tool::SpinTool
                         ? simUtils::Tool::GrabTool
                         : simUtils::Tool::SpinTool;
  }
  if (key == simParams.input_pushTool && action == GLFW_PRESS) {
    uc->activeTool = uc->activeTool == simUtils::Tool::PushTool
                         ? simUtils::Tool::GrabTool
                         : simUtils::Tool::PushTool;
    if (uc->activeTool == simUtils::Tool::PushTool) {
      (*ctrls)("kickPitch").setValue(cam->pitch() * M_PI / 180.0);
      (*ctrls)("kickYaw").setValue(M_PI + cam->yaw() * M_PI / 180.0);
      (*ctrls)("kickMag").setValue(10);
      uc->objKickVel = Vec3((*ctrls)["kickMag"] * cos((*ctrls)["kickPitch"]) *
                                sin((*ctrls)["kickYaw"]),
                            (*ctrls)["kickMag"] * sin((*ctrls)["kickPitch"]),
                            (*ctrls)["kickMag"] * cos((*ctrls)["kickPitch"]) *
                                cos((*ctrls)["kickYaw"]));
    }
  }
  if (key == simParams.input_toolReset && action == GLFW_PRESS) {
    switch (uc->activeTool) {
    case simUtils::Tool::SizeTool:
      (*ctrls)("radius").reset();
      break;
    case simUtils::Tool::SpeedTool:
      (*ctrls)("velMag").reset();
      break;
    case simUtils::Tool::SpinTool:
      (*ctrls)("vela").reset();
      break;
    case simUtils::Tool::GrabTool:
    default:
      std::cerr << "note: no active tool to reset, or tool does not support "
                   "reset\n";
    }
  }
  if (key == simParams.input_clearEnv && action == GLFW_PRESS) {
    Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
    simUtils::clearEnvObjs(*mbWin);
    uc->selectedObjId = -1;
  }
  if (key == simParams.input_pause && action == GLFW_PRESS) {
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
    (*ctrls)("velMag").changeValue(y);
    userCursor->candidateObjVel =
        Vec3((*ctrls)["velMag"] * cos((*ctrls)["velPitch"]) *
                 sin((*ctrls)["velYaw"]),
             (*ctrls)["velMag"] * sin((*ctrls)["velPitch"]),
             (*ctrls)["velMag"] * cos((*ctrls)["velPitch"]) *
                 cos((*ctrls)["velYaw"]));
  }
  if (userCursor->activeTool == simUtils::Tool::SpinTool) {
    (*ctrls)("vela").changeValue(y);
  }
  if (userCursor->activeTool == simUtils::Tool::PushTool) {
    (*ctrls)("kickMag").changeValue(y);
    userCursor->objKickVel =
        Vec3((*ctrls)["kickMag"] * cos((*ctrls)["kickPitch"]) *
                 sin((*ctrls)["kickYaw"]),
             (*ctrls)["kickMag"] * sin((*ctrls)["kickPitch"]),
             (*ctrls)["kickMag"] * cos((*ctrls)["kickPitch"]) *
                 cos((*ctrls)["kickYaw"]));
  }
}

void mouseButtonCallback(GLFWwindow *win, int button, int action, int m) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  simUtils::UserCursor *uc =
      (simUtils::UserCursor *)(mbWin->userPointer("userCursor"));

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    GraphicsTools::Camera *cam =
        static_cast<GraphicsTools::Camera *>(mbWin->userPointer("cam"));
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    uc->lastRightClick = Vec3(x, y, 0);
    uc->speedToolFacingCamera =
        simUtils::glmToVec3(cam->localForward()).dot(uc->candidateObjVel) < 0;
    uc->kickToolFacingCamera =
        simUtils::glmToVec3(cam->localForward()).dot(uc->objKickVel) < 0;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    Vec3 candidateObjPos(uc->data.ballX, uc->data.ballY, uc->data.ballZ);
    int objIdAtCandPos = simUtils::objIdAtEnvPos(candidateObjPos, *env);
    if (objIdAtCandPos != -1 && uc->activeTool != simUtils::Tool::PushTool) {
      simUtils::removeEnvObj(*mbWin, objIdAtCandPos);
    }
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    Vec3 candidateObjPos(uc->data.ballX, uc->data.ballY, uc->data.ballZ);

    int objIdAtCandPos = simUtils::objIdAtEnvPos(candidateObjPos, *env);
    if (objIdAtCandPos != -1) {
      if (uc->activeTool != simUtils::Tool::PushTool) {
        env->objs().at(objIdAtCandPos).setSelectState(true);
        uc->selectedObjId = objIdAtCandPos;
        uc->objSelectionOffset =
            env->objs().at(objIdAtCandPos).bbox().pos() - candidateObjPos;
      }
    }
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    Environment *env = (Environment *)mbWin->userPointer("env");
    // place relative to camera pos
    GraphicsTools::Camera *cam =
        (GraphicsTools::Camera *)mbWin->userPointer("cam");

    Vec3 candidateObjPos(uc->data.ballX, uc->data.ballY, uc->data.ballZ);

    if (uc->activeTool == simUtils::Tool::PushTool) {
      int objIdAtCandPos = simUtils::objIdAtEnvPos(candidateObjPos, *env);
      if (objIdAtCandPos != -1) {
        env->objs().at(objIdAtCandPos).applyForce(10000 * uc->objKickVel);
      }
    } else {
      // no object at cursor and no object selected: create a new object
      if (uc->selectedObjId == -1) {
        simUtils::createObj(*mbWin, candidateObjPos, uc->candidateObjVel);
      }
    }
    if (uc->selectedObjId != -1) {
      Ball &selected = env->objs().at(uc->selectedObjId);
      // push obj back inside if outside
      selected.setPos(selected.bbox().pos() -
                      env->computeOutsideEnv(selected.bbox().pos(),
                                             selected.bbox().w() * 0.5));
      // obj selected: unselect it
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

} // namespace simUtils