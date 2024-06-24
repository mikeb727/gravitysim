#include "utility.h"
#include "object.h"

#include <chrono>
#include <mb-libs/colors.h>

namespace utils {

Vec2 remapGlfwCursor(Vec2 vec, GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  assert(env);
  return Vec2(
      GraphicsTools::remap(vec.x(), 0, env->bbox()->w(), 0, win->width()),
      GraphicsTools::remap(vec.y(), 0, env->bbox()->h(), win->height(), 0));
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
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlset"));
  ObjMap *om = static_cast<ObjMap *>(win->userPointer("objmap"));
  GraphicsTools::ShaderProgram *shader =
      static_cast<GraphicsTools::ShaderProgram *>(win->userPointer("stripeshader"));
  CursorEmulator *cursor =
      static_cast<CursorEmulator *>(win->userPointer("cursor"));
  CursorEmulator *cursorEmu =
      static_cast<CursorEmulator *>(win->userPointer("cursorEmu"));

  bool objAtCursor = false;

  Vec2 envObjPos;
  if (simParams.disableUserInput) {
    envObjPos = Vec2(cursorEmu->current.ballX, cursorEmu->current.ballY);

  } else {
    envObjPos = remapGlfwCursor(
        Vec2(cursor->current.ballX, cursor->current.ballY), win);
  }

  for (auto &obj : env->objs()) {
    if (obj.second.bbox()->containsPoint(envObjPos)) {
      objAtCursor = true;
      obj.second.setVel(cursor->current.vel);
    }
    obj.second.setSelectState(false);
  }
  if (!objAtCursor) {
    bool noOverlap = true;
    for (auto &obj : env->objs()) {
      if (obj.second.bbox()->intersects(
              Circle(envObjPos, (*ctrls)["radius"] * simParams.envScale))) {
        noOverlap = false;
        break;
      }
    }
    if (noOverlap && env->bbox()->containsBBox(Circle(
                         envObjPos, (*ctrls)["radius"] * simParams.envScale))) {
      env->addObj(Object(
          new Circle(envObjPos, (*ctrls)["radius"] * simParams.envScale),
          pow((*ctrls)["radius"] * simParams.envScale, 3) / 1000, envObjPos,
          Vec2((*ctrls)["velx"], -(*ctrls)["vely"]) * simParams.envScale,
          (*ctrls)["elast"], (*ctrls)["vela"]));
      Vec2 drawPos(
          GraphicsTools::remap(envObjPos.x(), 0, env->bbox()->w(),
                               -0.5 * win->width(), 0.5 * win->width()),
          GraphicsTools::remap(envObjPos.y(), 0, env->bbox()->h(),
                               -0.5 * win->height(), 0.5 * win->height()));
      GraphicsTools::Material mat = {GraphicsTools::ColorRgba({0.1, 0.7, 0.2, 1.0}), NULL,
                                     0.5 * GraphicsTools::Colors::White, 4};
      om->emplace(env->lastObjId(), GraphicsTools::RenderObject());
      om->at(env->lastObjId()).setShader(shader);
      om->at(env->lastObjId()).setMaterial(mat);
      om->at(env->lastObjId())
          .genSphere((*ctrls)["radius"] * simParams.envScale, 16, 16);
      om->at(env->lastObjId()).setPos(glm::vec3(drawPos.x(), drawPos.y(), 0));
      win->activeScene()->addRenderObject(&om->at(env->lastObjId()));
    }
  }
}

} // namespace utils