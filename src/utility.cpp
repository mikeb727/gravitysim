#include "utility.h"
#include "ball.h"
#include "bbox.h"
#include "control.h"
#include "env3d.h"

#include <chrono>
#include <iostream>
#include <mb-libs/colors.h>

namespace simUtils {

Vec3 remapGlfwCursor(Vec3 vec, GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  assert(env);
  return Vec3(
      GraphicsTools::remap(vec.x(), 0, env->bbox().w(), 0, win->width()),
      GraphicsTools::remap(vec.y(), 0, env->bbox().h(), win->height(), 0));
}

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

void createObj(GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlSet"));
  ObjMap *om = static_cast<ObjMap *>(win->userPointer("ballObjMap"));
  GraphicsTools::ShaderProgram *shader =
      static_cast<GraphicsTools::ShaderProgram *>(
          win->userPointer("ballShader"));
  CursorEmulator *cursorEmu =
      static_cast<CursorEmulator *>(win->userPointer("cursorEmu"));

  bool objAtCandPos = false;

  Vec3 candidateObjPos(cursorEmu->current.ballX, cursorEmu->current.ballY,
                       cursorEmu->current.ballZ);

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
                   (*ctrls)["radius"] * 2.0 * simParams.environment_unitsPerMeter))) {
        noOverlap = false;
        break;
      }
    }
    if (noOverlap &&
        env->bbox().containsBBox(BBox(
            candidateObjPos, (*ctrls)["radius"] * 2.0 * simParams.environment_unitsPerMeter))) {
      env->addObj(Ball(
          BBox(candidateObjPos, (*ctrls)["radius"] * 2.0 * simParams.environment_unitsPerMeter),
          pow((*ctrls)["radius"] * simParams.environment_unitsPerMeter, 3), candidateObjPos,
          Vec3((*ctrls)["velx"], (*ctrls)["vely"], (*ctrls)["velz"]) *
              simParams.environment_unitsPerMeter,
          1, Vec3()));
      GraphicsTools::Material mat = {GraphicsTools::randomColor(), NULL,
                                     0.5 * GraphicsTools::Colors::White, 4};
      om->emplace(env->lastObjId(), GraphicsTools::RenderObject());
      om->at(env->lastObjId()).setShader(shader);
      om->at(env->lastObjId()).setMaterial(mat);
      om->at(env->lastObjId())
          .genSphere((*ctrls)["radius"] * simParams.environment_unitsPerMeter, 16, 16);
      om->at(env->lastObjId())
          .setPos(glm::vec3(candidateObjPos.x(), candidateObjPos.y(),
                            candidateObjPos.z()));
      win->activeScene()->addRenderObject(&om->at(env->lastObjId()));
    }
  }
}

void drawUserCursor(GraphicsTools::Window *win, simUtils::UserCursor *uc,
                    bool insideEnv, int objIdAtCursor) {
  simUtils::ObjMap *staticObjs =
      (simUtils::ObjMap *)(win->userPointer("staticObjMap"));
  int ballId = uc->baseRenderObjId;
  int arrowId = uc->baseRenderObjId + 1;
  int spinId = uc->baseRenderObjId + 2;
  int crosshairIdX = uc->baseRenderObjId + 3;
  int crosshairIdY = uc->baseRenderObjId + 4;
  int crosshairIdZ = uc->baseRenderObjId + 5;

  GraphicsTools::Material cursorMat = {
      GraphicsTools::blend(GraphicsTools::Colors::Blue, 0.4,
                           GraphicsTools::Colors::White, 0.6),
      NULL, GraphicsTools::Colors::White, 2};
  GraphicsTools::Material cursorMatOutside = {
      GraphicsTools::blend(GraphicsTools::Colors::Red, 0.4,
                           GraphicsTools::Colors::White, 0.6),
      NULL, GraphicsTools::Colors::White, 2};

  staticObjs->at(ballId).clearGeometry();
  staticObjs->at(arrowId).clearGeometry();
  staticObjs->at(spinId).clearGeometry();
  staticObjs->at(crosshairIdX).clearGeometry();
  staticObjs->at(crosshairIdY).clearGeometry();
  staticObjs->at(crosshairIdZ).clearGeometry();

  staticObjs->at(ballId).setMaterial(insideEnv ? cursorMat : cursorMatOutside);
  staticObjs->at(arrowId).setMaterial(insideEnv ? cursorMat : cursorMatOutside);
  staticObjs->at(spinId).setMaterial(insideEnv ? cursorMat : cursorMatOutside);
  staticObjs->at(crosshairIdX)
      .setMaterial(insideEnv ? cursorMat : cursorMatOutside);
  staticObjs->at(crosshairIdY)
      .setMaterial(insideEnv ? cursorMat : cursorMatOutside);
  staticObjs->at(crosshairIdZ)
      .setMaterial(insideEnv ? cursorMat : cursorMatOutside);
  CursorData cur = uc->data;

  float crosshairHalfLength = (objIdAtCursor != -1 || uc->forwardObjId != -1) ? 1.8 : 0.3;
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
    staticObjs->at(ballId).genSphere(cur.radius * simParams.environment_unitsPerMeter * 0.9, 16,
                                     16);
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
  }
}

void drawCursor(GraphicsTools::Window *win, CursorData cur, int curGfxId) {
  simUtils::ObjMap *staticObjs =
      (simUtils::ObjMap *)(win->userPointer("staticObjMap"));
  int ballId = curGfxId;
  int arrowId = curGfxId + 1;
  int spinId = curGfxId + 2;

  staticObjs->at(ballId).clearGeometry();
  staticObjs->at(arrowId).clearGeometry();
  staticObjs->at(spinId).clearGeometry();

  staticObjs->at(ballId).genSphere(cur.radius * simParams.environment_unitsPerMeter * 0.5, 16,
                                   16);
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

void drawSim(GraphicsTools::Window *win) {
  simUtils::ObjMap *staticObjs =
      (simUtils::ObjMap *)(win->userPointer("staticObjMap"));

  Environment *env = (Environment *)(win->userPointer("env"));
  CursorEmulator *cursorEmu = (CursorEmulator *)(win->userPointer("cursorEmu"));
  simUtils::UserCursor *uc =
      (simUtils::UserCursor *)(win->userPointer("userCursor"));
  simUtils::ObjMap *envObjs =
      (simUtils::ObjMap *)(win->userPointer("ballObjMap"));
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlSet"));

  bool insideEnv = env->bbox().containsPoint(
      Vec3(uc->data.ballX, uc->data.ballY, uc->data.ballZ));

  int objIdAtCursor =
      objIdAtEnvPos(Vec3(uc->data.ballX, uc->data.ballY, uc->data.ballZ), env);

  simUtils::drawUserCursor(
      win, uc,
      insideEnv && cubeOverlapAtEnvPos(
                       Vec3(uc->data.ballX, uc->data.ballY, uc->data.ballZ),
                       env, (*ctrls)["radius"] * 2.0 * simParams.environment_unitsPerMeter),
      objIdAtCursor);
  simUtils::drawCursor(win, cursorEmu->current,
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
  simUtils::ObjMap *staticObjMap =
      (simUtils::ObjMap *)(win->userPointer("staticObjMap"));
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
  staticObjMap->emplace(staticObjMap->size(), floor);

  GraphicsTools::RenderObject wall1;
  wall1.setMaterial(*wallMat);
  wall1.setTexture(wallTex);
  wall1.setShader(shader);
  wall1.genPlane(env->bbox().h(), env->bbox().d());
  wall1.setPos(glm::vec3(-0.5 * env->bbox().w(), 0, 0));
  wall1.setRotation(glm::vec3(0, 0, 1), -0.5 * M_PI);
  staticObjMap->emplace(staticObjMap->size(), wall1);

  GraphicsTools::RenderObject wall2;
  wall2.setMaterial(*wallMat);
  wall2.setTexture(wallTex);
  wall2.setShader(shader);
  wall2.genPlane(env->bbox().h(), env->bbox().d());
  wall2.setPos(glm::vec3(0.5 * env->bbox().w(), 0, 0.0));
  wall2.setRotation(glm::vec3(0, 0, 1), 0.5 * M_PI);
  staticObjMap->emplace(staticObjMap->size(), wall2);

  GraphicsTools::RenderObject wall3;
  wall3.setMaterial(*wallMat);
  wall3.setTexture(wallTex);
  wall3.setShader(shader);
  wall3.genPlane(env->bbox().w(), env->bbox().h());
  wall3.setPos(glm::vec3(0.0, 0, -0.5 * env->bbox().d()));
  wall3.setRotation(glm::vec3(1, 0, 0), 0.5 * M_PI);
  staticObjMap->emplace(staticObjMap->size(), wall3);

  GraphicsTools::RenderObject wall4;
  wall4.setMaterial(*wallMat);
  wall4.setTexture(wallTex);
  wall4.setShader(shader);
  wall4.genPlane(env->bbox().w(), env->bbox().h());
  wall4.setPos(glm::vec3(0.0, 0.0, 0.5 * env->bbox().d()));
  wall4.setRotation(glm::vec3(1, 0, 0), -0.5 * M_PI);
  staticObjMap->emplace(staticObjMap->size(), wall4);

  GraphicsTools::RenderObject ceiling;
  ceiling.setMaterial(*wallMat);
  ceiling.setTexture(wallTex);
  ceiling.setShader(shader);
  ceiling.genPlane(env->bbox().w(), env->bbox().d());
  ceiling.setPos(glm::vec3(0.0, 0.5 * env->bbox().h(), 0.0));
  ceiling.setRotation(glm::vec3(1, 0, 0), M_PI);
  staticObjMap->emplace(staticObjMap->size(), ceiling);

  for (int i = 0; i < staticObjMap->size(); ++i) {
    win->activeScene()->addRenderObject(&staticObjMap->at(i));
  }
}

void removeEnvObj(GraphicsTools::Window *win, int objId) {
  if (objId == -1)
    return;
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ObjMap *objMap = static_cast<ObjMap *>(win->userPointer("ballObjMap"));
  env->removeObj(objId);
  objMap->erase(objId);
}

void clearEnvObjs(GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ObjMap *objMap = static_cast<ObjMap *>(win->userPointer("ballObjMap"));
  env->clearObjs();
  objMap->clear();
}

void setupUserCursors(GraphicsTools::Window *win, UserCursor *uc) {
  simUtils::ObjMap *staticObjMap =
      (simUtils::ObjMap *)(win->userPointer("staticObjMap"));
  GraphicsTools::ShaderProgram *phong =
      (GraphicsTools::ShaderProgram *)(win->userPointer("phongShader"));

  uc->activeTool = simUtils::Tool::None;

  uc->baseRenderObjId = staticObjMap->size();

  GraphicsTools::Material cursorMat = {
      GraphicsTools::blend(GraphicsTools::Colors::Blue, 0.4,
                           GraphicsTools::Colors::White, 0.6),
      NULL, GraphicsTools::Colors::White, 2};
  GraphicsTools::Material cursorMatInvalid = {
      GraphicsTools::blend(GraphicsTools::Colors::Red, 0.4,
                           GraphicsTools::Colors::White, 0.6),
      NULL, GraphicsTools::Colors::White, 2};

  GraphicsTools::RenderObject userSizeDisplay;
  userSizeDisplay.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId, userSizeDisplay);
  win->activeScene()->addRenderObject(&staticObjMap->at(uc->baseRenderObjId));

  GraphicsTools::RenderObject userSpeedDisplay;
  userSpeedDisplay.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 1, userSpeedDisplay);
  win->activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 1));

  GraphicsTools::RenderObject userSpinDisplay;
  userSpinDisplay.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 2, userSpinDisplay);
  win->activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 2));

  // small 3D crosshair if no tool is active
  GraphicsTools::RenderObject userCrosshairX;
  userCrosshairX.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 3, userCrosshairX);
  win->activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 3));
  GraphicsTools::RenderObject userCrosshairY;
  userCrosshairY.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 4, userCrosshairY);
  win->activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 4));
  GraphicsTools::RenderObject userCrosshairZ;
  userCrosshairZ.setShader(phong);
  staticObjMap->emplace(uc->baseRenderObjId + 5, userCrosshairZ);
  win->activeScene()->addRenderObject(
      &staticObjMap->at(uc->baseRenderObjId + 5));
}

void setupControls(ControlSet &ctrlSet) {
  ctrlSet.addCtrl("radius",
                  Control("Radius", simParams.controls_radius[0],
                          simParams.controls_radius[1], simParams.controls_radius[2],
                          simParams.controls_radius[3]));
  ctrlSet.addCtrl("velForward",
                  Control("Forward velocity", simParams.controls_velocityForward[0],
                          simParams.controls_velocityForward[1],
                          simParams.controls_velocityForward[2],
                          simParams.controls_velocityForward[3]));
  ctrlSet.addCtrl("velx", Control("X velocity", simParams.controls_velocityX[0],
                                  simParams.controls_velocityX[1], simParams.controls_velocityX[2],
                                  simParams.controls_velocityX[3]));
  ctrlSet.addCtrl("vely", Control("Y velocity", simParams.controls_velocityY[0],
                                  simParams.controls_velocityY[1], simParams.controls_velocityY[2],
                                  simParams.controls_velocityY[3]));
  ctrlSet.addCtrl("velz", Control("Z velocity", simParams.controls_velocityZ[0],
                                  simParams.controls_velocityZ[1], simParams.controls_velocityZ[2],
                                  simParams.controls_velocityZ[3]));
  ctrlSet.addCtrl(
      "vela", Control("Angular velocity", simParams.controls_velocityAngular[0],
                      simParams.controls_velocityAngular[1], simParams.controls_velocityAngular[2],
                      simParams.controls_velocityAngular[3]));
  ctrlSet.addCtrl("angularAxisX", Control("Angular axis X", -4, 4, 0.01,
                                          simParams.controls_angularAxis.x()));
  ctrlSet.addCtrl("angularAxisY", Control("Angular axis Y", -4, 4, 0.01,
                                          simParams.controls_angularAxis.y()));
  ctrlSet.addCtrl("angularAxisZ", Control("Angular axis Z", -4, 4, 0.01,
                                          simParams.controls_angularAxis.z()));
}

int objIdAtEnvPos(Vec3 pos, Environment *env) {
  for (auto &obj : env->objs()) {
    if (obj.second.bbox().containsPoint(pos)) {
      return obj.first;
    }
  }
  return -1;
}

// whether ball at pos overlaps with any other ball
bool cubeOverlapAtEnvPos(Vec3 pos, Environment *env, float radius) {
  for (auto &obj : env->objs()) {
    if (obj.second.bbox().intersects(BBox(pos, radius))) {
      return false;
    }
  }
  return true;
}

Vec3 glmToVec3(glm::vec3 v){
  return Vec3(v.x, v.y, v.z);
}

} // namespace simUtils