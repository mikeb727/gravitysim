/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include "2DPhysEnv.h"
#include "control.h"
#include "simParams.h"

#include <argparse/argparse.hpp>
#include <mb-libs/mbgfx.h>

#include <iostream>
#include <map>
#include <sstream>

typedef std::map<std::string, Control> CtrlSet;
typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

// previous cursor positions (for dragging velocity calculation)
double cursorPosPrevX, cursorPosPrevY;

void drawSim(GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  ObjMap *ocm = static_cast<ObjMap *>(win->userPointer("objmap"));

  CtrlSet *ctrls = static_cast<CtrlSet *>(win->userPointer("ctrlset"));
  GraphicsTools::Font *font =
      static_cast<GraphicsTools::Font *>(win->userPointer("font"));

  // update renderobject positions
  for (auto &obj : env->objs()) {
    float drawPosX = obj.second.bbox()->pos().x() - (win->width() * 0.5f);
    float drawPosY =
        (win->height() - obj.second.bbox()->pos().y()) - (win->height() * 0.5f);
    ocm->at(obj.first).setPos(glm::vec3(drawPosX, drawPosY, 0.0f));
  }

  std::ostringstream ctrlStatus;
  ctrlStatus << "radius " << ctrls->at("radius").getValue() << "\nvelocity "
             << ctrls->at("velx").getValue() << " "
             << ctrls->at("vely").getValue() << "\nelasticity "
             << ctrls->at("elast").getValue();
  // draw all objs
  win->clear();
  win->activeScene()->render();
  win->activeScene()->drawText2D(*font, ctrlStatus.str(),
                                 GraphicsTools::Colors::Black, 50, 250, -1, 1);
  win->update();
}

void updateSim(Environment *env) { env->update(); }

void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
  ObjMap *om = static_cast<ObjMap *>(mbWin->userPointer("objmap"));
  // If the 'C' key is pressed, remove all objects
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
}

void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
  GraphicsTools::Window *mbWin =
      static_cast<GraphicsTools::Window *>(glfwGetWindowUserPointer(win));
  Environment *env = static_cast<Environment *>(mbWin->userPointer("env"));
  CtrlSet *ctrls = static_cast<CtrlSet *>(mbWin->userPointer("ctrlset"));
  ObjMap *om = static_cast<ObjMap *>(mbWin->userPointer("objmap"));
  ShaderProgram *shader =
      static_cast<ShaderProgram *>(mbWin->userPointer("phongshader"));

  double cursorPosX, cursorPosY;
  bool objAtCursor;
  glfwGetCursorPos(win, &cursorPosX, &cursorPosY);
  Vec cursorPos(cursorPosX, cursorPosY);

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    for (auto &obj : env->objs()) {
      if (obj.second.bbox()->containsPoint(cursorPos)) {
        objAtCursor = true;
        obj.second.setSelectState(true);
        break;
      }
    }
  }
  // left mouse release: create object if none present at cursor
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    for (auto &obj : env->objs()) {
      if (obj.second.bbox()->containsPoint(cursorPos)) {
        objAtCursor = true;
        obj.second.setSelectState(false);
        break;
      }
    }
    if (!objAtCursor) {
      if (env->bbox()->containsBBox(
              Circle(cursorPos, ctrls->at("radius").getValue()))) {
        env->addObj(Object(
            new Circle(cursorPos, ctrls->at("radius").getValue()), pow(ctrls->at("radius").getValue(), 3), cursorPos,
            Vec(ctrls->at("velx").getValue(), ctrls->at("vely").getValue()),
            ctrls->at("elast").getValue()));

        GraphicsTools::Material mat = {GraphicsTools::randomColor(), NULL,
                                       0.5 * GraphicsTools::Colors::White, 4};
        om->emplace(env->lastObjId(), GraphicsTools::RenderObject());
        om->at(env->lastObjId()).setShader(shader);
        om->at(env->lastObjId()).setMaterial(mat);
        om->at(env->lastObjId())
            .genSphere(ctrls->at("radius").getValue(), 16, 16);
        om->at(env->lastObjId())
            .setPos(glm::vec3(cursorPos.x() - (mbWin->width() / 2.0f),
                              (mbWin->height() - cursorPos.y()) -
                                  (mbWin->height() * 0.5f),
                              0));
        mbWin->activeScene()->addRenderObject(&om->at(env->lastObjId()));
      }
    }
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    for (auto &obj : env->objs()) {
      if (obj.second.bbox()->containsPoint(cursorPos)) {
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
  CtrlSet *ctrls = static_cast<CtrlSet *>(mbWin->userPointer("ctrlset"));
  ObjMap *om = static_cast<ObjMap *>(mbWin->userPointer("objmap"));

  double cursorPosX, cursorPosY;
  bool objAtCursor;
  glfwGetCursorPos(win, &cursorPosX, &cursorPosY);
  Vec cursorPos(cursorPosX, cursorPosY);

  if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT)) {
    for (auto &obj : env->objs()) {
      if (obj.second.bbox()->containsPoint(cursorPos) &&
          obj.second.selected()) {
        objAtCursor = true;
        obj.second.setPos(cursorPos);
        obj.second.setVel(
            75 * Vec(cursorPosX - cursorPosPrevX, cursorPosY - cursorPosPrevY));
        break;
      }
    }
  }

  cursorPosPrevX = cursorPosX;
  cursorPosPrevY = cursorPosY;
}

bool handleInput(GraphicsTools::Window *win) {
  Environment *env = static_cast<Environment *>(win->userPointer("env"));
  CtrlSet *ctrls = static_cast<CtrlSet *>(win->userPointer("ctrlset"));
  GraphicsTools::Scene *sc = win->activeScene();
  // If the W key is pressed, increase elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_W)) {
    ctrls->at("radius").changeValue(1.0);
    std::cerr << "ctrl radius " << ctrls->at("radius").getValue() << "\n";
  }
  // If the Q key is pressed, decrease elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_Q)) {
    ctrls->at("radius").changeValue(-1.0);
    std::cerr << "ctrl radius " << ctrls->at("radius").getValue() << "\n";
  }
  // If the S key is pressed, increase elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_S)) {
    ctrls->at("elast").changeValue(0.01);
    std::cerr << "ctrl elast " << ctrls->at("elast").getValue() << "\n";
  }
  // If the A key is pressed, decrease elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_A)) {
    ctrls->at("elast").changeValue(-0.01);
    std::cerr << "ctrl elast " << ctrls->at("elast").getValue() << "\n";
  }
  //  velocity
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_LEFT)) {
    ctrls->at("velx").changeValue(-5);
    std::cerr << "ctrl vel " << ctrls->at("velx").getValue() << " "
              << ctrls->at("vely").getValue() << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_RIGHT)) {
    ctrls->at("velx").changeValue(5);
    std::cerr << "ctrl vel " << ctrls->at("velx").getValue() << " "
              << ctrls->at("vely").getValue() << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_UP)) {
    ctrls->at("vely").changeValue(-5);
    std::cerr << "ctrl vel " << ctrls->at("velx").getValue() << " "
              << ctrls->at("vely").getValue() << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_DOWN)) {
    ctrls->at("vely").changeValue(5);
    std::cerr << "ctrl vel " << ctrls->at("velx").getValue() << " "
              << ctrls->at("vely").getValue() << "\n";
  }

  return false;
}

int main(int argc, char *argv[]) {

  argparse::ArgumentParser argParser("gravity_sim");
  argParser.add_argument("-c", "--config").default_value("").nargs(1);
  argParser.parse_args(argc, argv);

  SimParameters paramSet = parseXml(argParser.get<std::string>("--config"));

  Environment mainEnv(paramSet.envDimensions.x(), paramSet.envDimensions.y(),
                      paramSet.envGravity * paramSet.envScale,
                      0.5 / paramSet.frameRate);

  CtrlSet mainCtrls;
  mainCtrls["radius"] = Control("Radius", 15, 100, 40);
  mainCtrls["elast"] = Control("Elasticity", 0, 1, 1);
  mainCtrls["velx"] = Control("X velocity", -1000, 1000, 0);
  mainCtrls["vely"] = Control("Y velocity", -1000, 1000, 0);

  GraphicsTools::InitGraphics();
  GraphicsTools::Window mainWindow("Gravity Sim", paramSet.envDimensions.x(),
                                   paramSet.envDimensions.y());
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
    handleInput(&mainWindow);
    updateSim(&mainEnv);
    drawSim(&mainWindow);
  }

  GraphicsTools::CloseGraphics();

  return 0;
}
