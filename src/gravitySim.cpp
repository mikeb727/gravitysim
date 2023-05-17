/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include "2DPhysEnv.h"
#include "2DVec.h"
#include "control.h"
#include "simParams.h"

#include <argparse/argparse.hpp>
#include <mb-libs/mbgfx.h>

#include <cmath>
#include <iostream>
#include <map>
#include <sstream>

typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

// global parameters; used by all objects after initialization
SimParameters simParams;

// cursor manipulation
// avoid remapping these to window coords until absolutely necessary!
struct CursorData {
  double cursorPosPrevX, cursorPosPrevY;
  double cursorPosX, cursorPosY;
} cursor;

bool freezeCursor = false;

void drawSim(GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ObjMap *ocm = static_cast<ObjMap *>(win->userPointer("objmap"));
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlset"));
  GraphicsTools::Font *font =
      static_cast<GraphicsTools::Font *>(win->userPointer("font"));

  // update renderobject positions
  for (auto &obj : env->objs()) {
    // environment to scene coords
    Vec drawPos(
        GraphicsTools::remap(obj.second.bbox()->pos().x(), 0, env->bbox()->w(),
                             -0.5 * win->width(), 0.5 * win->width()),
        GraphicsTools::remap(obj.second.bbox()->pos().y(), 0, env->bbox()->h(),
                             -0.5 * win->height(), 0.5 * win->height()));
    ocm->at(obj.first).setPos(glm::vec3(drawPos.x(), drawPos.y(), 0.0f));
  }

  // generate on-screen status text
  std::ostringstream ctrlStatus;
  ctrlStatus << "radius " << (*ctrls)["radius"] << "\nvelocity "
             << (*ctrls)["velx"] << " " << (*ctrls)["vely"] << "\nelasticity "
             << (*ctrls)["elast"];
  // draw all objs

  // custom cursor - circle showing where new ball is placed
  // red if ball cannot be placed at cursor
  GraphicsTools::ColorRgba cursorColor =
      env->bbox()->containsBBox(
          Circle(Vec(cursor.cursorPosPrevX, cursor.cursorPosPrevY),
                 (*ctrls)["radius"]))
          ? GraphicsTools::ColorRgba({0, 0, 0, 0.3})
          : GraphicsTools::ColorRgba({0.6, 0, 0, 0.3});

  win->clear();
  // objects
  win->activeScene()->render();
  // status string
  win->drawText(ctrlStatus.str(), font, GraphicsTools::Colors::Black, 50, 250,
                -1, GraphicsTools::Left);
  // cursor to window coords
  Vec cursorDrawPos(GraphicsTools::remap(cursor.cursorPosPrevX, 0, win->width(),
                                         0, win->width()),
                    GraphicsTools::remap(cursor.cursorPosPrevY, 0,
                                         win->height(), win->height(), 0));
  win->drawCircle(cursorColor, cursorDrawPos.x(), cursorDrawPos.y(),
                  (*ctrls)["radius"]);
  // velocity arrow
  Vec ctrlVel((*ctrls)["velx"], -(*ctrls)["vely"]);
  Vec arrowBasePos(
      cursorDrawPos.x() + ((*ctrls)["radius"] * cos(ctrlVel.dir())),
      cursorDrawPos.y() + ((*ctrls)["radius"] * sin(ctrlVel.dir())));
  Vec arrowTipPos(cursorDrawPos + ctrlVel);
  if (ctrlVel.mag() > (*ctrls)["radius"]) {
    Vec diff(arrowTipPos - arrowBasePos);
    win->drawArrow(cursorColor, arrowBasePos.x(), arrowBasePos.y(),
                   arrowTipPos.x(), arrowTipPos.y(),
                   fmin((*ctrls)["radius"], 0.4 * diff.mag()));
  }
  win->update();
}

void updateSim(Environment *env) { env->update(); }

void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlset"));
  ObjMap *om = static_cast<ObjMap *>(mbWin->userPointer("objmap"));
  // If thed'C' key is pressed, remove all objects
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    for (auto &obj : env->objs()) {
      mbWin->activeScene()->removeRenderObject(obj.first);
    }
    env->clearObjs();
  }
  // If the space bar is pressed, pause the environment
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    env->togglePause();
  }
  // If the escape key is pressed, signal to quit simulation
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    mbWin->setShouldClose(1);
  }
  // If the F1 key is released, return the cursor to its previous position
  // (unfreeze)
  if (key == GLFW_KEY_F1 && action == GLFW_RELEASE && freezeCursor) {
    freezeCursor = false;
    glfwSetCursorPos(mbWin->glfwWindow(), cursor.cursorPosPrevX,
                     cursor.cursorPosPrevY);
  }
  if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
    (*ctrls)("velx").setValue(0);
    (*ctrls)("vely").setValue(0);
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

  bool objAtCursor;
  // cursor to environment coords
  Vec envObjPos(GraphicsTools::remap(cursor.cursorPosPrevX, 0, env->bbox()->w(),
                                     0, mbWin->width()),
                GraphicsTools::remap(cursor.cursorPosPrevY, 0, env->bbox()->h(),
                                     mbWin->height(), 0));

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
        obj.second.setSelectState(false);
        break;
      }
    }
    if (!objAtCursor) {
      if (env->bbox()->containsBBox(Circle(envObjPos, (*ctrls)["radius"]))) {
        env->addObj(Object(new Circle(envObjPos, (*ctrls)["radius"]),
                           pow((*ctrls)["radius"], 3), envObjPos,
                           Vec((*ctrls)["velx"], -(*ctrls)["vely"]),
                           (*ctrls)["elast"]));
        Vec drawPos(GraphicsTools::remap(envObjPos.x(), 0, env->bbox()->w(),
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
        om->at(env->lastObjId()).genSphere((*ctrls)["radius"], 16, 16);
        om->at(env->lastObjId()).setPos(glm::vec3(drawPos.x(), drawPos.y(), 0));
        mbWin->activeScene()->addRenderObject(&om->at(env->lastObjId()));
      }
    }
  }
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

void cursorPosCallback(GLFWwindow *win, double x, double y) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(mbWin->userPointer("ctrlset"));
  ObjMap *om = static_cast<ObjMap *>(mbWin->userPointer("objmap"));

  freezeCursor = glfwGetKey(win, GLFW_KEY_F1) == GLFW_PRESS;

  bool objAtCursor;

  glfwGetCursorPos(mbWin->glfwWindow(), &cursor.cursorPosX, &cursor.cursorPosY);
  Vec cursorPos(cursor.cursorPosX, cursor.cursorPosY);

  if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT)) {
    // cursor to environment coords
    Vec envObjPos(GraphicsTools::remap(cursorPos.x(), 0, env->bbox()->w(), 0,
                                       mbWin->width()),
                  GraphicsTools::remap(cursorPos.y(), 0, env->bbox()->h(),
                                       mbWin->height(), 0));
    for (auto &obj : env->objs()) {
      if (obj.second.bbox()->containsPoint(envObjPos) &&
          obj.second.selected()) {
        objAtCursor = true;
        obj.second.setPos(envObjPos);
        obj.second.setVel(Vec(0, 0));
        break;
      }
    }
  }

  if (!glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) &&
      !glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) && freezeCursor) {
    (*ctrls)("velx").setValue(2.0 *
                              (cursor.cursorPosX - cursor.cursorPosPrevX));
    (*ctrls)("vely").setValue(2.0 *
                              (cursor.cursorPosY - cursor.cursorPosPrevY));
    std::cerr << cursor.cursorPosPrevY << "\n";
  }

  cursor.cursorPosPrevX =
      freezeCursor ? cursor.cursorPosPrevX : cursor.cursorPosX;
  cursor.cursorPosPrevY =
      freezeCursor ? cursor.cursorPosPrevY : cursor.cursorPosY;
}

bool handleKeyStates(GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ControlSet *ctrls = static_cast<ControlSet *>(win->userPointer("ctrlset"));
  GraphicsTools::Scene *sc = win->activeScene();
  // If the W key is pressed, increase radius of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_W)) {
    (*ctrls)("radius")++;
    std::cerr << "ctrl radius " << (*ctrls)["radius"] << "\n";
  }
  // If the Q key is pressed, decrease radius of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_Q)) {
    (*ctrls)("radius")--;
    std::cerr << "ctrl radius " << (*ctrls)["radius"] << "\n";
  }
  // If the S key is pressed, increase elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_S)) {
    (*ctrls)("elast")++;
    std::cerr << "ctrl elast " << (*ctrls)["elast"] << "\n";
  }
  // If the A key is pressed, decrease elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_A)) {
    (*ctrls)("elast")--;
    std::cerr << "ctrl elast " << (*ctrls)["elast"] << "\n";
  }
  //  velocity
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_LEFT)) {
    (*ctrls)("velx")--;
    std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
              << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_RIGHT)) {
    (*ctrls)("velx")++;
    std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
              << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_UP)) {
    (*ctrls)("vely")--;
    std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
              << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_DOWN)) {
    (*ctrls)("vely")++;
    std::cerr << "ctrl vel " << (*ctrls)["velx"] << " " << (*ctrls)["vely"]
              << "\n";
  }

  return false;
}

int main(int argc, char *argv[]) {

  argparse::ArgumentParser argParser("gravity_sim");
  argParser.add_argument("-c", "--config").default_value("").nargs(1);
  argParser.parse_args(argc, argv);

  simParams = parseXmlConfig(argParser.get<std::string>("--config"));

  Environment mainEnv(simParams.envDimensions.x(), simParams.envDimensions.y(),
                      simParams.envGravity * simParams.envScale,
                      0.5 / simParams.frameRate);

  if (simParams.envPauseState == true) {
    mainEnv.togglePause();
  }

  ControlSet mainCtrls;
  mainCtrls._ctrls.emplace("radius", Control("Radius", simParams.ctrlRadius[0],
                                             simParams.ctrlRadius[1],
                                             simParams.ctrlRadius[2],
                                             simParams.ctrlRadius[3]));
  mainCtrls._ctrls.emplace("velx",
                           Control("X velocity", simParams.ctrlVelX[0],
                                   simParams.ctrlVelX[1], simParams.ctrlVelX[2],
                                   simParams.ctrlVelX[3]));
  mainCtrls._ctrls.emplace("vely",
                           Control("Y velocity", simParams.ctrlVelY[0],
                                   simParams.ctrlVelY[1], simParams.ctrlVelY[2],
                                   simParams.ctrlVelY[3]));
  mainCtrls._ctrls.emplace("elast", Control("Elasticity", 0, 1, 0.01, 1));

  GraphicsTools::InitGraphics();
  GraphicsTools::Window mainWindow("Gravity Sim", simParams.envDimensions.x(),
                                   simParams.envDimensions.y());
  mainWindow.setClearColor({0.8, 0.7, 0.6, 1.0});
  ObjMap mainOM;

  mainWindow.setUserPointer("env", &mainEnv);
  mainWindow.setUserPointer("objmap", &mainOM);
  mainWindow.setUserPointer("ctrlset", &mainCtrls);
  ShaderProgram phongShader("assets/phong_shadows_vs.glsl",
                            "assets/phong_shadows_fs.glsl");
  mainWindow.setUserPointer("phongshader", &phongShader);
  GraphicsTools::Font font1("assets/font.ttf", 20);
  mainWindow.setUserPointer("font", &font1);

  GraphicsTools::Scene sc;
  mainWindow.attachScene(&sc);

  // glfwSetInputMode(mainWindow.glfwWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  GraphicsTools::Camera cam1;
  cam1.setOrtho(mainWindow.width(), mainWindow.height());
  cam1.setPos(glm::vec3(0.0f, 0.0f, 200.0f));
  sc.addCamera(&cam1);
  sc.setActiveCamera(0);

  GraphicsTools::DirectionalLight light2 = {
      glm::normalize(glm::vec3(0, -0.2, -1)),
      0.5 * GraphicsTools::Colors::White, GraphicsTools::Colors::White,
      0.1 * GraphicsTools::Colors::White, &phongShader};
  sc.setDirLight(&light2);

  glfwSetKeyCallback(mainWindow.glfwWindow(), keyCallback);
  glfwSetMouseButtonCallback(mainWindow.glfwWindow(), mouseButtonCallback);
  glfwSetCursorPosCallback(mainWindow.glfwWindow(), cursorPosCallback);

  // While the program is running
  bool quit = false;
  while (!mainWindow.shouldClose()) {
    handleKeyStates(&mainWindow);
    updateSim(&mainEnv);
    drawSim(&mainWindow);
  }

  GraphicsTools::CloseGraphics();

  return 0;
}
