/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <iostream>
#include <map>

#include "2DPhysEnv.h"
#include "control.h"
#include <argparse/argparse.hpp>
#include <mb-libs/colors.h>
#include <mb-libs/mbgfx.h>

using std::cerr;

typedef std::map<std::string, Control> CtrlSet;
typedef std::map<int, GraphicsTools::ColorRgba> ObjColorMap;
typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

/* The dimensions of the simulation window. */
const int ENV_WIDTH = 1024;
const int ENV_HEIGHT = 768;

/* The scale of the simulation ([pixelsPerMeter] pixels = 1 meter). */
const double pixelsPerMeter = 120;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

void drawSim(GraphicsTools::Window *win, Environment *env, ObjMap &ocm) {
  // update renderobject positions
  for (auto &obj : env->objs()) {
    float drawPosX = obj.second.bbox()->pos().x() - (win->width() * 0.5f);
    float drawPosY =
        (win->height() - obj.second.bbox()->pos().y()) - (win->height() *
        0.5f);
    ocm.at(obj.first).setPos(glm::vec3(drawPosX, drawPosY, 0.0f));
  }
  // draw all objs
  win->clear();
  win->activeScene()->render();
  win->update();
}

void updateSim(Environment *env) { env->update(); }

void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
  GraphicsTools::Window *mbWin =
      (GraphicsTools::Window *)glfwGetWindowUserPointer(win);
  Environment *env = (Environment *)mbWin->userPointer("env");
  // If the 'C' key is pressed, remove all objects
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    env->clearObjs();
  }
  // If the space bar is pressed, pause the environment
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    env->togglePause();
  }
}

bool handleInput(GraphicsTools::Window *win, Environment *env, CtrlSet &ctrls,
                 ObjColorMap &ocm) {
  GraphicsTools::Scene *sc = win->activeScene();
  SDL_Event event;
  // If the escape key is pressed, signal to quit simulation
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_ESCAPE)) {
    win->setShouldClose(1);
    cerr << "Escape key pressed\n";
  }
  // If the W key is pressed, increase elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_W)) {
    ctrls["radius"].changeValue(1.0);
    cerr << "ctrl radius " << ctrls["radius"].getValue() << "\n";
  }
  // If the Q key is pressed, decrease elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_Q)) {
    ctrls["radius"].changeValue(-1.0);
    cerr << "ctrl radius " << ctrls["radius"].getValue() << "\n";
  }
  // If the S key is pressed, increase elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_S)) {
    ctrls["elast"].changeValue(0.01);
    cerr << "ctrl elast " << ctrls["elast"].getValue() << "\n";
  }
  // If the A key is pressed, decrease elasticity of Objects created
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_A)) {
    ctrls["elast"].changeValue(-0.01);
    cerr << "ctrl elast " << ctrls["elast"].getValue() << "\n";
  }
  //  velocity
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_LEFT)) {
    ctrls["velx"].changeValue(-5);
    cerr << "ctrl vel " << ctrls["velx"].getValue() << " "
         << ctrls["vely"].getValue() << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_RIGHT)) {
    ctrls["velx"].changeValue(5);
    cerr << "ctrl vel " << ctrls["velx"].getValue() << " "
         << ctrls["vely"].getValue() << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_UP)) {
    ctrls["vely"].changeValue(-5);
    cerr << "ctrl vel " << ctrls["velx"].getValue() << " "
         << ctrls["vely"].getValue() << "\n";
  }
  if (glfwGetKey(win->glfwWindow(), GLFW_KEY_DOWN)) {
    ctrls["vely"].changeValue(5);
    cerr << "ctrl vel " << ctrls["velx"].getValue() << " "
         << ctrls["vely"].getValue() << "\n";
  }

  if (event.type == SDL_MOUSEBUTTONDOWN) {
    Vec mousePosition(event.button.x, event.button.y);
    bool objAtLocation = false, ctrlAtLocation = false;
    if (event.button.button == SDL_BUTTON_LEFT) {
      // TODO reenable mouse interactions with controls
      // for (const Control& ctrl: ctrls){
      // 	if (ctrl.containsPoint(mousePosition)){
      // 		ctrlAtLocation = true;
      // 		ctrl.setFromPosition(mousePosition);
      // 		break;
      // 	}
      // }
      if (!ctrlAtLocation) {
        for (auto &obj : env->objs()) {
          if (obj.second.bbox()->containsPoint(mousePosition)) {
            objAtLocation = true;
            obj.second.setSelectState(true);
            break;
          }
        }
      }
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      for (auto &obj : env->objs()) {
        if (obj.second.bbox()->containsPoint(mousePosition)) {
          objAtLocation = true;
          env->removeObj(obj.first);
          break;
        }
      }
    }
  }

  if (event.type == SDL_MOUSEBUTTONUP) {
    Vec mousePosition(event.button.x, event.button.y);
    bool objAtLocation = false, ctrlAtLocation = false;
    if (event.button.button == SDL_BUTTON_LEFT) {
      // for (const Control& ctrl: ctrls){
      // 	if (ctrl.containsPoint(mousePosition)){
      // 		ctrlAtLocation = true;
      // 		break;
      // 	}
      // }
      for (auto &obj : env->objs()) {
        if (obj.second.bbox()->containsPoint(mousePosition)) {
          objAtLocation = true;
          obj.second.setSelectState(false);
          break;
        }
      }
      if (!objAtLocation && !ctrlAtLocation) {
        if (env->bbox()->containsBBox(
                Circle(mousePosition, ctrls["radius"].getValue()))) {
          env->addObj(
              Object(new Circle(mousePosition, ctrls["radius"].getValue()), 1,
                     mousePosition,
                     Vec(ctrls["velx"].getValue(), ctrls["vely"].getValue()),
                     ctrls["elast"].getValue()));
          ocm[env->lastObjId()] = GraphicsTools::randomColor();
        }
      }
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      // does nothing
    }
  }

  if (event.type == SDL_MOUSEMOTION) {
    Vec mousePosition(event.button.x, event.button.y);
    bool objAtLocation = false, ctrlAtLocation = false;
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
      // for (int i = 0; i < _ctrls.size(); i++){
      // 	if (_ctrls[i].containsPoint(mousePosition)){
      // 		ctrlAtLocation = true;
      // 		_ctrls[i].setFromPosition(mousePosition);
      // 		break;
      // 	}
      // }
      if (!ctrlAtLocation) {
        for (auto &obj : env->objs()) {
          if (obj.second.bbox()->containsPoint(mousePosition) &&
              obj.second.selected()) {
            objAtLocation = true;
            obj.second.setPos(mousePosition);
            obj.second.setVel(
                Vec((event.motion.xrel) *
                        (pixelsPerMeter * pixelsPerMeter / framesPerSecond),
                    (event.motion.yrel) *
                        (pixelsPerMeter * pixelsPerMeter / framesPerSecond)));
            break;
          }
        }
      }
    }
  }

  return false;
}

int main(int argc, char *argv[]) {

  argparse::ArgumentParser argParser("gravity_sim");
  argParser.add_argument("-d", "--dimensions")
      .default_value(std::vector<int>({ENV_WIDTH, ENV_HEIGHT}))
      .nargs(2)
      .scan<'d', int>();
  argParser.add_argument("-g", "--gravity")
      .default_value(std::vector<double>({0, 9.8}))
      .nargs(2)
      .scan<'g', double>();
  argParser.parse_args(argc, argv);
  std::vector<int> envDimensions =
      argParser.get<std::vector<int>>("--dimensions");
  std::vector<double> envGravity =
      argParser.get<std::vector<double>>("--gravity");

  std::cerr << envDimensions[0] << " " << envDimensions[1] << "\n";

  Environment mainEnv(envDimensions[0], envDimensions[1],
                      Vec(envGravity[0], envGravity[1]) * pixelsPerMeter,
                      0.5 / framesPerSecond);

  CtrlSet mainCtrls;
  mainCtrls["radius"] = Control("Radius", 15, 100, 30);
  mainCtrls["elast"] = Control("Elasticity", 0, 1, 1);
  mainCtrls["velx"] = Control("X velocity", -500, 500, 0);
  mainCtrls["vely"] = Control("Y velocity", -500, 500, 0);

  ObjColorMap mainOcm;

  GraphicsTools::InitGraphics();
  GraphicsTools::Window mainWindow("Gravity Sim", envDimensions[0], envDimensions[1]);
  ShaderProgram phongShader("assets/phong_shadows_vs.glsl",
                            "assets/phong_shadows_fs.glsl");
  mainWindow.setClearColor({0.8, 0.7, 0.6, 1.0});
  ObjMap mainOM;

  GraphicsTools::Scene sc;
  mainWindow.attachScene(&sc);

  GraphicsTools::Camera cam1;
  cam1.setOrtho(mainWindow.width(), mainWindow.height());
  cam1.setPos(glm::vec3(0.0f, 0.0f, 50.0f));
  sc.addCamera(&cam1);
  sc.setActiveCamera(0);

  GraphicsTools::DirectionalLight light2 = {glm::normalize(glm::vec3(0, 0, -1)),
                                            {0.1, 0.1, 0.1, 1.0},
                                            {1.0, 1.0, 1.0, 1.0},
                                            {0.1, 0.1, 0.1, 1.0},
                                            &phongShader};
  sc.setDirLight(&light2);

  glfwSetKeyCallback(mainWindow.glfwWindow(), keyCallback);

  for (int i = 0; i < 10; ++i) {
    float posX = 200 + (70 * i);
    float posY = 500 - (40 * i);
    float rad = 30 + i;
    GraphicsTools::Material mat = {GraphicsTools::randomColor(), NULL, GraphicsTools::Colors::White, 64};
    mainEnv.addObj(Object(new Circle(Vec(posX, posY), rad), 1, Vec(posX, posY),
                          Vec(20, 0), 1));
    mainOM.emplace(mainEnv.lastObjId(), GraphicsTools::RenderObject());
    mainOM.at(mainEnv.lastObjId()).setShader(&phongShader);
    mainOM.at(mainEnv.lastObjId()).setMaterial(mat);
    mainOM.at(mainEnv.lastObjId()).genSphere(rad, 8, 8);
    mainOM.at(mainEnv.lastObjId())
        .setPos(glm::vec3(
            posX - (mainWindow.width() / 2.0f),
            (mainWindow.height() - posY) - (mainWindow.height() * 0.5f), 0));
    mainWindow.activeScene()->addRenderObject(&mainOM.at(mainEnv.lastObjId()));
  }

  // While the program is running
  bool quit = false;
  while (!mainWindow.shouldClose()) {
    // handleInput(&mainWindow, &mainEnv, mainCtrls, mainOcm);
    if (mainEnv.time() % 60 == 0) {
      mainEnv.print(std::cerr);
    }
    updateSim(&mainEnv);
    drawSim(&mainWindow, &mainEnv, mainOM);
  }

  GraphicsTools::CloseGraphics();

  return 0;
}
