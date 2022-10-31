/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include <iostream>
#include <map>

#include "window.h"
#include "control.h"
#include "2DPhysEnv.h"

using std::cerr;

typedef std::map<std::string, Control> CtrlSet;

/* The dimensions of the simulation window. */
const int SCREENWIDTH = 1024;
const int SCREENHEIGHT = 768;

/* The scale of the simulation ([pixelsPerMeter] pixels = 1 meter). */
const double pixelsPerMeter = 80;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

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
        ctrls["radius"].changeValue(1.0);
		cerr << "Radius is " << ctrls["radius"].getValue() << "\n";
    }
    // If the Q key is pressed, decrease elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q){
        ctrls["radius"].changeValue(-1.0);
		cerr << "Radius is " << ctrls["radius"].getValue() << "\n";
    }
    // If the S key is pressed, increase elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s){
        ctrls["elast"].changeValue(0.01);
		cerr << "Elasticity is " << ctrls["elast"].getValue() << "\n";
    }
    // If the A key is pressed, decrease elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a){
        ctrls["elast"].changeValue(-0.01);
		cerr << "Elasticity is " << ctrls["elast"].getValue() << "\n";
    }
	//  velocity
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT){
		ctrls["velx"].changeValue(-5);
		cerr << "Velocity is " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
	}
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT){
		ctrls["velx"].changeValue(5);
		cerr << "Velocity is " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
	}
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP){
		ctrls["vely"].changeValue(-5);
		cerr << "Velocity is " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
	}
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN){
		ctrls["vely"].changeValue(5);
		cerr << "Velocity is " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
	}

    // If the 'C' key is pressed, remove all objects
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_c){
        env->clearObjs();
    }
    // If the space bar is pressed, pause the environment
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){
		env->togglePause();
    }

    if (event.type == SDL_MOUSEBUTTONDOWN){
		Vec mousePosition(event.button.x, event.button.y);
		bool objAtLocation = false, ctrlAtLocation = false;
		if (event.button.button == SDL_BUTTON_LEFT){
			// TODO reenable mouse interactions with controls
			// for (const Control& ctrl: ctrls){
			// 	if (ctrl.containsPoint(mousePosition)){
			// 		ctrlAtLocation = true;
			// 		ctrl.setFromPosition(mousePosition);
			// 		break;
			// 	}
			// }
			if (!ctrlAtLocation){
				for (Object& obj: env->getObjList()){
					if (obj.getBBox()->containsPoint(mousePosition)){
						objAtLocation = true;
						obj.setSelectState(true);
						break;
					}
				}
			}
		}
		if (event.button.button == SDL_BUTTON_RIGHT){
			for (int i = 0; i < env->getObjList().size(); i++){
				if (env->getObjList()[i].getBBox()->containsPoint(mousePosition)){
					objAtLocation = true;
                    env->removeObj(i);
					break;
				}
			}		
		}
	}

    if (event.type == SDL_MOUSEBUTTONUP){
    	Vec mousePosition(event.button.x, event.button.y);
    	bool objAtLocation = false, ctrlAtLocation = false;
        if (event.button.button == SDL_BUTTON_LEFT){
			// for (const Control& ctrl: ctrls){
			// 	if (ctrl.containsPoint(mousePosition)){
			// 		ctrlAtLocation = true;
			// 		break;
			// 	}
			// }
			for (Object& obj: env->getObjList()){
				if (obj.getBBox()->containsPoint(mousePosition)){
					objAtLocation = true;
					obj.setSelectState(false);
					break;
				}
			}
			if (!objAtLocation && !ctrlAtLocation){
				if (env->getBBox()->containsBBox(Circle(mousePosition, ctrls["radius"].getValue()))){
					env->addObj(Object(new Circle(mousePosition, ctrls["radius"].getValue()), 1, mousePosition, Vec(ctrls["velx"].getValue(), ctrls["vely"].getValue(), true), ctrls["elast"].getValue()));
				}
			}
        }
        if (event.button.button == SDL_BUTTON_RIGHT){
        	// does nothing
        }
    }

    if (event.type == SDL_MOUSEMOTION){
    	Vec mousePosition(event.button.x, event.button.y);
		bool objAtLocation = false, ctrlAtLocation = false;
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)){
			// for (int i = 0; i < _ctrls.size(); i++){
			// 	if (_ctrls[i].containsPoint(mousePosition)){
			// 		ctrlAtLocation = true;
			// 		_ctrls[i].setFromPosition(mousePosition);			
			// 		break;
			// 	}
			// }
			if (!ctrlAtLocation){
				for (Object& obj: env->getObjList()){
					if (obj.getBBox()->containsPoint(mousePosition) && obj.getSelectState()){
						objAtLocation = true;
		            	obj.setPos(Vec(mousePosition));
		                obj.setVel(Vec((event.motion.xrel) * (pixelsPerMeter * pixelsPerMeter / framesPerSecond),
		                				   (event.motion.yrel) * (pixelsPerMeter * pixelsPerMeter / framesPerSecond), true));
						break;
					}
				}	
			}
        }
    }

	return false;

}

int main(int argc, char* argv[]){
	
	Environment* mainEnv;
	Window* mainWindow;
	CtrlSet mainCtrls;

	mainCtrls["radius"] = Control("Radius", 15, 100, 30);
	mainCtrls["elast"] = Control("Elasticity", 0, 1, 1);
	mainCtrls["velx"] = Control("X velocity", -500, 500, 0);
	mainCtrls["vely"] = Control("Y velocity", -500, 500, 0);

	if (argc == 3){
		mainWindow = new Window("Gravity Sim", atoi(argv[1]), atoi(argv[2]));
		mainEnv = new Environment(atoi(argv[2]), atoi(argv[1]), Vec(0, 9.8 * pixelsPerMeter, true));
	}
	else if (argc == 5){
		mainWindow = new Window("Gravity Sim", atoi(argv[1]), atoi(argv[2]));
		mainEnv = new Environment(atoi(argv[2]), atoi(argv[1]), Vec(atoi(argv[4]), atoi(argv[3]) * pixelsPerMeter, true));
	}
	else {
		mainWindow = new Window("Gravity Sim", SCREENWIDTH, SCREENHEIGHT);
		mainEnv = new Environment(SCREENHEIGHT, SCREENWIDTH, Vec(0, 9.8 * pixelsPerMeter, true));
	}

	SDL_Event ev;

    // While the program is running
    bool quit = false;
    while (!quit){
    	while (SDL_PollEvent(&ev)){
			quit = handleInput(ev, mainWindow, mainEnv, mainCtrls);
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
