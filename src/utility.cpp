#include "utility.h"
#include "ball.h"
#include "bbox.h"

#include <chrono>
#include <mb-libs/colors.h>

namespace utils {

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
                   (*ctrls)["radius"] * 2.0 * simParams.envScale))) {
        noOverlap = false;
        break;
      }
    }
    if (noOverlap &&
        env->bbox().containsBBox(BBox(
            candidateObjPos, (*ctrls)["radius"] * 2.0 * simParams.envScale))) {
      env->addObj(Ball(
          BBox(candidateObjPos, (*ctrls)["radius"] * 2.0 * simParams.envScale),
          pow((*ctrls)["radius"] * simParams.envScale, 3), candidateObjPos,
          Vec3((*ctrls)["velx"], (*ctrls)["vely"], (*ctrls)["velz"]) *
              simParams.envScale,
          1, Vec3()));
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
      win->activeScene()->addRenderObject(&om->at(env->lastObjId()));
    }
  }
}

} // namespace utils