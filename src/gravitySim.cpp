/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include <iostream>
#include <vector>

#include "window.h"
#include "2DPhysEnv.h"

using std::cerr;

typedef std::vector<Control> CtrlSet;

/* The dimensions of the simulation window. */
const int SCREENWIDTH = 1024;
const int SCREENHEIGHT = 768;

/* The scale of the simulation ([pixelsPerMeter] pixels = 1 meter). */
const double pixelsPerMeter = 80;

void drawSim(Window* win, Environment* env){
	win->clear();
	// draw all objs
	for (const Object& obj: env->getObjList()){
		Vec pos(obj.getBBox()->getPos());
		win->drawCircleGradient(Colors::red, Colors::white, pos.x(), pos.y(), obj.getBBox()->getW()/2);
	}
	win->update();
}

void updateSim(Environment* env){
	env->update();
}

bool handleInput(SDL_Event event, Window* win, Environment* env, CtrlSet& ctrls){
    // If the close button on the titlebar is clicked, signal to quit simulation
    if (event.type == SDL_QUIT){
        cerr << "Close button clicked\n";
        return true;
    }
    // If the escape key is pressed, signal to quit simulation
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE){
        cerr << "Escape key pressed\n";
        return true;
    }
    // If the W key is pressed, increase elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w){
        ctrls[0].increaseValue(1.0);
		cerr << "Radius is " << ctrls[0].getValue() << "\n";
    }
    // If the Q key is pressed, decrease elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q){
        ctrls[0].decreaseValue(1.0);
		cerr << "Radius is " << ctrls[0].getValue() << "\n";
    }
    // If the S key is pressed, increase elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s){
        ctrls[1].increaseValue(0.01);
		cerr << "Elasticity is " << ctrls[1].getValue() << "\n";
    }
    // If the A key is pressed, decrease elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a){
        ctrls[1].decreaseValue(0.01);
		cerr << "Elasticity is " << ctrls[1].getValue() << "\n";
    }
	// TODO velocity

    // If the 'C' key is pressed, remove all objects
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_c){
        env->clearObjs();
    }
    // If the space bar is pressed, pause the environment
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){
		env->togglePause();
    }

	return false;

}

int main(int argc, char* argv[]){
	
	Environment* mainEnv;
	Window* mainWindow;
	CtrlSet mainCtrls;

	mainCtrls.push_back(Control(mainWindow, "Radius", 3, 100, 30, 18, 150, Vec(24, 24)));
	mainCtrls.push_back(Control(mainWindow, "Elasticity", 0, 1, 1, 18, 150, Vec(228, 24)));
	mainCtrls.push_back(Control(mainWindow, "X velocity", -100, 100, 0, 18, 150, Vec(0, 0)));
	mainCtrls.push_back(Control(mainWindow, "Y velocity", -100, 100, 0, 18, 150, Vec(0, 0)));

	if (argc == 3){
		mainWindow = new Window("Gravity Sim", atoi(argv[1]), atoi(argv[2]));
		mainEnv = new Environment(mainWindow, atoi(argv[2]), atoi(argv[1]), Vec(0, 9.8 * pixelsPerMeter, true));
	}
	else if (argc == 5){
		mainWindow = new Window("Gravity Sim", atoi(argv[1]), atoi(argv[2]));
		mainEnv = new Environment(mainWindow, atoi(argv[2]), atoi(argv[1]), Vec(atoi(argv[4]), atoi(argv[3]) * pixelsPerMeter, true));
	}
	else {
		mainWindow = new Window("Gravity Sim", SCREENWIDTH, SCREENHEIGHT);
		mainEnv = new Environment(mainWindow, SCREENHEIGHT, SCREENWIDTH, Vec(0, 9.8 * pixelsPerMeter, true));
	}

	SDL_Event ev;

    // While the program is running
    bool quit = false;
    while (!quit){
    	while (SDL_PollEvent(&ev)){
			quit = handleInput(ev, mainWindow, mainEnv, mainCtrls);
			mainEnv->handleInput(ev);
		}
        // Clear the screen
        // Update window
		if (mainEnv->getT() % 60 == 0){
			mainEnv->print(std::cout);
		}

		updateSim(mainEnv);
		drawSim(mainWindow, mainEnv);
    }
    
    delete mainWindow; delete mainEnv;

	return 0;
}
