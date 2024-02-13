/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left-clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include <argparse/argparse.hpp>
#include <mb-libs/mbgfx.h>

#include "2DPhysEnv.h"
#include "2DVec.h"
#include "control.h"
#include "cursor.h"
#include "simParams.h"
#include "utility.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <sstream>

// set of all balls to be rendered in environment
typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

// switch for compile-time vs runtime shaders
#define COMPILE_TIME_SHADERS
#ifdef COMPILE_TIME_SHADERS
const char *phongVs =

#include "../assets/phong_shadows_vs_ct.glsl"

    ;
const char *phongFs =

#include "../assets/phong_shadows_fs_ct.glsl"
    ;
#endif

// global parameters; used by all objects after initialization
SimParameters simParams;
std::default_random_engine generator; // for random ball colors

Vec2 remapGlfwCursor(Vec2 v, GraphicsTools::Window *w) {
  Environment *e = static_cast<Environment *>(w->userPointer("env"));
  return Vec2(GraphicsTools::remap(v.x(), 0, e->bbox()->w(), 0, w->width()),
              GraphicsTools::remap(v.y(), 0, e->bbox()->h(), w->height(), 0));
}

void drawCursor(GraphicsTools::Window *win, CursorData cur) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlset"));
  if (!env || !ctrls) {
    std::cerr << "drawCursor error: null pointers!\n";
    return;
  }
  // custom cursor - circle showing where new ball is placed
  // red if ball cannot be placed at cursor
  GraphicsTools::ColorRgba cursorColor =
      env->bbox()->containsBBox(
          Circle(Vec2(cur.ballX, cur.ballY), cur.radius * simParams.envScale))
          ? GraphicsTools::ColorRgba({0.4, 0.4, 0.4, 0.3})
          : GraphicsTools::ColorRgba({0.8, 0.4, 0.4, 0.3});
  // cursor to environment/window/scene coords
  Vec2 cursorDrawPos = cur.isGlfw
                           ? remapGlfwCursor(Vec2(cur.ballX, cur.ballY), win)
                           : Vec2(cur.ballX, cur.ballY);
  // delta remains constant when F1 is not pressed; use it instead of
  // subtracting
  Vec2 cursorArrowOffset(cur.deltaX, -cur.isGlfw * cur.deltaY);
  // draw ball outline
  win->drawCircle(cursorColor, cursorDrawPos.x(), cursorDrawPos.y(),
                  cur.radius * simParams.envScale);
  // draw velocity arrow
  // find point on surface of ball in direction of arrowhead
  Vec2 arrowBasePos = cursorDrawPos + (cur.radius * simParams.envScale *
                                       cursorArrowOffset.unit());
  Vec2 arrowHeadPos = cursorDrawPos + cursorArrowOffset;
  if (cursorArrowOffset.mag() > cur.radius * simParams.envScale) {
    win->drawArrow(cursorColor, arrowBasePos.x(), arrowBasePos.y(),
                   arrowHeadPos.x(), arrowHeadPos.y(),
                   fmin(cur.radius * simParams.envScale,
                        0.4 * (arrowHeadPos - arrowBasePos).mag()));
  }
}

void drawSim(GraphicsTools::Window *win, CursorData cur) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ObjMap *ocm = static_cast<ObjMap *>(win->userPointer("objmap"));
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlset"));
  GraphicsTools::Font *font =
      static_cast<GraphicsTools::Font *>(win->userPointer("font"));

  // update renderobject positions
  for (auto &obj : env->objs()) {
    Vec2 drawPos = obj.second.bbox()->pos();
    ocm->at(obj.first).setPos(glm::vec3(drawPos.x(), drawPos.y(), 0.0f));
  }

  win->clear();
  win->activeScene()->render(); // all objs
  drawCursor(win, cur);         // cursor
  // clang-format off
  // generate on-screen status text
  // std::ostringstream ctrlStatus;
  // ctrlStatus << "radius " << (*ctrls)["radius"] << "\nvelocity "
  //            << (*ctrls)["velx"] << " " << (*ctrls)["vely"] << "\nelasticity "
  //            << (*ctrls)["elast"] << "\n" << *env;
  // win->drawText(ctrlStatus.str(), font, GraphicsTools::Colors::Black, 50,
  // 250,
  //                 -1, GraphicsTools::Left); // status text
  // clang-format on
  win->update();
}

void updateSim(Environment *env) { env->update(); }

void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlset"));
  ObjMap *om = static_cast<ObjMap *>(mbWin->userPointer("objmap"));
  CursorEmulator *cursor =
      static_cast<CursorEmulator *>(mbWin->userPointer("cursor"));
  CursorEmulator *cursorEmu =
      static_cast<CursorEmulator *>(mbWin->userPointer("cursorEmu"));

  if (!simParams.disableUserInput) {

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      cursorEmu->active = cursorEmu->active ? false : true;
    }

    if (!cursorEmu->active) {

      // c: clear all objects
      if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        for (auto &obj : env->objs()) {
          mbWin->activeScene()->removeRenderObject(obj.first);
        }
        env->clearObjs();
      }
      // space: pause environment
      if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        env->togglePause();
      }
      // f1 (release): return hidden cursor to ball position (from arrow tip
      // position)
      if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
        glfwSetCursorPos(mbWin->glfwWindow(), cursor->current.ballX,
                         cursor->current.ballY);
      }
      // f1 (press): freeze ball position, reset arrow tip position to ball
      // position, set velocity controls
      if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        cursor->current.arrowX = cursor->current.ballX;
        cursor->current.arrowY = cursor->current.ballY;
        cursor->current.deltaX = cursor->current.deltaY = 0;
        (*ctrls)("velx").setValue(cursor->current.deltaX);
        (*ctrls)("vely").setValue(cursor->current.deltaY);
      }
    }
  }
}

void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlset"));
  ObjMap *om = static_cast<ObjMap *>(mbWin->userPointer("objmap"));
  ShaderProgram *shader =
      static_cast<ShaderProgram *>(mbWin->userPointer("phongshader"));
  CursorEmulator *cursor =
      static_cast<CursorEmulator *>(mbWin->userPointer("cursor"));
  CursorEmulator *cursorEmu =
      static_cast<CursorEmulator *>(mbWin->userPointer("cursorEmu"));

  if (!simParams.disableUserInput || !cursorEmu->active) {

    bool objAtCursor = false;
    // cursor to environment coords
    Vec2 envObjPos = remapGlfwCursor(
        Vec2(cursor->current.ballX, cursor->current.ballY), mbWin);

    // left mouse press: select object under cursor
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      for (auto &obj : env->objs()) {
        if (obj.second.bbox()->containsPoint(envObjPos)) {
          objAtCursor = true;
          obj.second.setSelectState(true);
          break;
        }
      }
    }
    // left mouse release: create object if none present at cursor
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
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
        if (noOverlap &&
            env->bbox()->containsBBox(
                Circle(envObjPos, (*ctrls)["radius"] * simParams.envScale))) {
          env->addObj(Object(
              new Circle(envObjPos, (*ctrls)["radius"] * simParams.envScale),
              pow((*ctrls)["radius"] * simParams.envScale, 2) / 1000, envObjPos,
              Vec2((*ctrls)["velx"], -(*ctrls)["vely"]) * simParams.envScale,
              (*ctrls)["elast"]));
          Vec2 drawPos(GraphicsTools::remap(envObjPos.x(), 0, env->bbox()->w(),
                                            -0.5 * mbWin->width(),
                                            0.5 * mbWin->width()),
                       GraphicsTools::remap(envObjPos.y(), 0, env->bbox()->h(),
                                            -0.5 * mbWin->height(),
                                            0.5 * mbWin->height()));
          GraphicsTools::Material mat = {GraphicsTools::randomColor(), NULL,
                                         0.5 * GraphicsTools::Colors::White, 4};
          om->emplace(env->lastObjId(), GraphicsTools::RenderObject());
          om->at(env->lastObjId()).setShader(shader);
          om->at(env->lastObjId()).setMaterial(mat);
          om->at(env->lastObjId())
              .genSphere((*ctrls)["radius"] * simParams.envScale, 16, 16);
          om->at(env->lastObjId())
              .setPos(glm::vec3(drawPos.x(), drawPos.y(), 0));
          mbWin->activeScene()->addRenderObject(&om->at(env->lastObjId()));
        }
      }
    }
    // right mouse press: remove object at cursor
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
      for (auto &obj : env->objs()) {
        if (obj.second.bbox()->containsPoint(envObjPos)) {
          objAtCursor = true;
          mbWin->activeScene()->removeRenderObject(obj.first);
          env->removeObj(obj.first);
          break;
        }
      }
    }
  }
}

void cursorPosCallback(GLFWwindow *win, double x, double y) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlset"));
  CursorEmulator *cursor =
      static_cast<CursorEmulator *>(mbWin->userPointer("cursor"));
  CursorEmulator *cursorEmu =
      static_cast<CursorEmulator *>(mbWin->userPointer("cursorEmu"));

  if (!simParams.disableUserInput || !cursorEmu->active) {

    Vec2 cursorPos(cursor->current.ballX, cursor->current.ballY);
    std::cerr << x << " " << y << " \n";
    if (glfwGetKey(win, GLFW_KEY_F1) == GLFW_PRESS) {
      cursor->current.arrowX = x;
      cursor->current.arrowY = y;
      cursor->current.deltaX = cursor->current.arrowX - cursor->current.ballX;
      cursor->current.deltaY = cursor->current.arrowY - cursor->current.ballY;
      (*ctrls)("velx").setValue(cursor->current.deltaX * 2.0 /
                                simParams.envScale);
      (*ctrls)("vely").setValue(cursor->current.deltaY * 2.0 /
                                simParams.envScale);
    } else {
      cursor->current.ballX = x;
      cursor->current.ballY = y;
    }

    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT)) {
      // cursor to environment coords
      Vec2 envObjPos(GraphicsTools::remap(cursorPos.x(), 0, env->bbox()->w(), 0,
                                          mbWin->width()),
                     GraphicsTools::remap(cursorPos.y(), 0, env->bbox()->h(),
                                          mbWin->height(), 0));
      for (auto &obj : env->objs()) {
        if (obj.second.bbox()->containsPoint(envObjPos) &&
            obj.second.selected()) {
          obj.second.setPos(envObjPos);
          obj.second.setVel(Vec2(0, 0));
          break;
        }
      }
    }
  }
}

bool handleKeyStates(GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlset"));
  CursorEmulator *cursor =
      static_cast<CursorEmulator *>(win->userPointer("cursor"));
  CursorEmulator *cursorEmu =
      static_cast<CursorEmulator *>(win->userPointer("cursorEmu"));

  cursor->current.radius = (*ctrls)["radius"];
  cursor->current.deltaX = (*ctrls)["velx"] * simParams.envScale / 2.0;
  cursor->current.deltaY = (*ctrls)["vely"] * simParams.envScale / 2.0;

  GraphicsTools::Scene *sc = win->activeScene();
  if (!simParams.disableUserInput || !cursorEmu->active) {
    // If the W key is pressed, increase radius of Objects created
    if (glfwGetKey(win->glfwWindow(), GLFW_KEY_W)) {
      (*ctrls)("radius")++;
#ifdef VERBOSE
      std::cerr << "ctrl radius " << (*ctrls)["radius"] << "\n";
#endif
    }
    // If the Q key is pressed, decrease radius of Objects created
    if (glfwGetKey(win->glfwWindow(), GLFW_KEY_Q)) {
      (*ctrls)("radius")--;
#ifdef VERBOSE
      std::cerr << "ctrl radius " << (*ctrls)["radius"] << "\n";
#endif
    }
    //  velocity
    if (glfwGetKey(win->glfwWindow(), GLFW_KEY_LEFT)) {
      (*ctrls)("velx")--;
#ifdef VERBOSE
      std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
                << "\n";
#endif
    }
    if (glfwGetKey(win->glfwWindow(), GLFW_KEY_RIGHT)) {
      (*ctrls)("velx")++;
#ifdef VERBOSE
      std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
                << "\n";
#endif
    }
    if (glfwGetKey(win->glfwWindow(), GLFW_KEY_UP)) {
      (*ctrls)("vely")--;
#ifdef VERBOSE
      std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
                << "\n";
#endif
    }
    if (glfwGetKey(win->glfwWindow(), GLFW_KEY_DOWN)) {
      (*ctrls)("vely")++;
#ifdef VERBOSE
      std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
                << "\n";
#endif
    }
  }

  // If the escape key is pressed, signal to quit simulation
  glfwSetWindowShouldClose(win->glfwWindow(),
                           glfwGetKey(win->glfwWindow(), GLFW_KEY_ESCAPE));

  return false;
}

int main(int argc, char *argv[]) {

  // timing vars
  double tLastWindow = 0;
  double tLastEnv = 0;
  double tLastEnergy = 0;

  generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

  argparse::ArgumentParser argParser("gravity_sim");
  argParser.add_argument("-c", "--config").default_value("").nargs(1);
  argParser.parse_args(argc, argv);
  simParams = parseXmlConfig(argParser.get<std::string>("--config"));

  GraphicsTools::InitGraphics();
  GraphicsTools::Window window(
      "Gravity Sim", simParams.envDimensions.x(), simParams.envDimensions.y(),
      simParams.fullscreenMode ? GraphicsTools::WindowMode::ScreensaverMode
                               : GraphicsTools::WindowMode::Null);
#ifdef COMPILE_TIME_SHADERS
  ShaderProgram phong(phongVs, phongFs, true);
#else
  ShaderProgram phong("assets/phong_shadows_vs.glsl",
                      "assets/phong_shadows_fs.glsl");
#endif
  GraphicsTools::Font windowFont("assets/font.ttf", 12);

  GraphicsTools::Scene sc;
  GraphicsTools::Camera cam1;
  GraphicsTools::DirectionalLight light2 = {
      glm::normalize(glm::vec3(0, -0.2, -1)),
      0.5 * GraphicsTools::Colors::White, GraphicsTools::Colors::White,
      0.1 * GraphicsTools::Colors::White, &phong};
  cam1.setOrtho(window.width(), window.height());
  cam1.setPos(glm::vec3(window.width() * 0.5f, window.height() * 0.5f, 400.0f));
  sc.addCamera(&cam1);
  sc.setActiveCamera(0);
  sc.setDirLight(&light2);
  ObjMap mainOM;
  window.attachScene(&sc);
  window.setClearColor({0.8, 0.7, 0.6, 1.0});
  window.setUserPointer("phongshader", &phong);
  window.setUserPointer("font", &windowFont);
  window.setUserPointer("objmap", &mainOM);

  Environment environment(
      simParams.fullscreenMode ? window.width() : simParams.envDimensions.x(),
      simParams.fullscreenMode ? window.height() : simParams.envDimensions.y(),
      simParams.envGravity * simParams.envScale, 1.0 / simParams.envFrameRate);
  if (simParams.envPauseState == true) {
    environment.togglePause();
  }
  window.setUserPointer("env", &environment);

  ControlSet ctrlSet;
  ctrlSet._ctrls.emplace("radius", Control("Radius", simParams.ctrlRadius[0],
                                           simParams.ctrlRadius[1],
                                           simParams.ctrlRadius[2],
                                           simParams.ctrlRadius[3]));
  ctrlSet._ctrls.emplace("velx",
                         Control("X velocity", simParams.ctrlVelX[0],
                                 simParams.ctrlVelX[1], simParams.ctrlVelX[2],
                                 simParams.ctrlVelX[3]));
  ctrlSet._ctrls.emplace("vely",
                         Control("Y velocity", simParams.ctrlVelY[0],
                                 simParams.ctrlVelY[1], simParams.ctrlVelY[2],
                                 simParams.ctrlVelY[3]));
  ctrlSet._ctrls.emplace("elast", Control("Elasticity", 0, 1, 0.01, 1));
  window.setUserPointer("ctrlset", &ctrlSet);

  CursorEmulator userCursor(&window);
  CursorEmulator cursorEmu(&window);
  cursorEmu.active = simParams.disableUserInput;
  window.setUserPointer("cursor", &userCursor);
  window.setUserPointer("cursorEmu", &cursorEmu);

  userCursor.current.isGlfw = true;
  userCursor.active = false;
  glfwSetKeyCallback(window.glfwWindow(), keyCallback);
  // glfwSetInputMode(window.glfwWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetMouseButtonCallback(window.glfwWindow(), mouseButtonCallback);
  glfwSetCursorPosCallback(window.glfwWindow(), cursorPosCallback);

  // While the program is running
  while (!window.shouldClose()) {
    double tNow = computeTNow();
    // std::cerr << std::fixed << std::setprecision(5) << computeTNow() / 1000
    //           << "\n";

    // only update env at its dt
    if ((tNow - tLastEnv) / 1000.0 > environment.dt()) {
      updateSim(&environment);
      tLastEnv = tNow;
    }

    // only update window at framerate
    if ((tNow - tLastWindow) / 1000.0 > (1.0 / simParams.windowFrameRate)) {
      cursorEmu.update();
      handleKeyStates(&window);
      window.getEvents();
      drawSim(&window, (simParams.disableUserInput || cursorEmu.active)
                           ? cursorEmu.current
                           : userCursor.current);
      tLastWindow = tNow;
      userCursor.current.vel =
          Vec2(userCursor.current.ballX - userCursor.prev.ballX,
               userCursor.prev.ballY - userCursor.current.ballY) *
          120;
      userCursor.prev.ballX = userCursor.current.ballX;
      userCursor.prev.ballY = userCursor.current.ballY;
    }

    // if ((tNow - tLastEnergy) / 1000.0 > 0.1) {
    //   std::cerr << std::fixed << std::setprecision(2)
    //             << environment.computeEnergy() << "\n";
    //   tLastEnergy = tNow;
    // }
  }

  GraphicsTools::CloseGraphics();

  return 0;
}
