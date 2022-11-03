/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include <iostream>
#include <map>

#include "graphicsTools.h"
#include "control.h"
#include "2DPhysEnv.h"

using std::cerr;

typedef std::map<std::string, Control> CtrlSet;
typedef std::map<int, GraphicsTools::ColorRgba> ObjColorMap;

/* The dimensions of the simulation window. */
const int SCREENWIDTH = 1024;
const int SCREENHEIGHT = 768;

/* The scale of the simulation ([pixelsPerMeter] pixels = 1 meter). */
const double pixelsPerMeter = 80;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;



void drawSim(GraphicsTools::Window* win, Environment* env, ObjColorMap& ocm){
	win->clear();
	// draw all objs
  	win->drawRectangle(GraphicsTools::ColorRgba{200, 180, 150, 255}, 0, 0, env->bbox()->w(), env->bbox()->h());
	for (auto& obj: env->objs()){
		Vec pos(obj.second.bbox()->pos());
		win->drawCircleGradient(ocm[obj.first], GraphicsTools::blend(ocm[obj.first], 0.5, GraphicsTools::Colors::White, 0.5), pos.x(), pos.y(), obj.second.bbox()->w()/2);
	}
	win->update();
}

void updateSim(Environment* env){
	env->update();
}

bool handleInput(SDL_Event event, GraphicsTools::Window* win, Environment* env, CtrlSet& ctrls, ObjColorMap& ocm){
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
		cerr << "ctrl radius " << ctrls["radius"].getValue() << "\n";
    }
    // If the Q key is pressed, decrease elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q){
        ctrls["radius"].changeValue(-1.0);
		cerr << "ctrl radius " << ctrls["radius"].getValue() << "\n";
    }
    // If the S key is pressed, increase elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s){
        ctrls["elast"].changeValue(0.01);
		cerr << "ctrl elast " << ctrls["elast"].getValue() << "\n";
    }
    // If the A key is pressed, decrease elasticity of Objects created
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a){
        ctrls["elast"].changeValue(-0.01);
		cerr << "ctrl elast " << ctrls["elast"].getValue() << "\n";
    }
	//  velocity
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT){
		ctrls["velx"].changeValue(-5);
		cerr << "ctrl vel " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
	}
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT){
		ctrls["velx"].changeValue(5);
		cerr << "ctrl vel " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
	}
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP){
		ctrls["vely"].changeValue(-5);
		cerr << "ctrl vel " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
	}
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN){
		ctrls["vely"].changeValue(5);
		cerr << "ctrl vel " << ctrls["velx"].getValue() << " " << ctrls["vely"].getValue() << "\n";
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
				for (auto& obj: env->objs()){
					if (obj.second.bbox()->containsPoint(mousePosition)){
						objAtLocation = true;
						obj.second.setSelectState(true);
						break;
					}
				}
			}
		}
		if (event.button.button == SDL_BUTTON_RIGHT){
			for (int i = 0; i < env->objs().size(); i++){
				if (env->objs()[i].bbox()->containsPoint(mousePosition)){
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
			for (auto& obj: env->objs()){
				if (obj.second.bbox()->containsPoint(mousePosition)){
					objAtLocation = true;
					obj.second.setSelectState(false);
					break;
				}
			}
			if (!objAtLocation && !ctrlAtLocation){
				if (env->bbox()->containsBBox(Circle(mousePosition, ctrls["radius"].getValue()))){
					env->addObj(Object(new Circle(mousePosition, ctrls["radius"].getValue()), 1, mousePosition, Vec(ctrls["velx"].getValue(), ctrls["vely"].getValue()), ctrls["elast"].getValue()));
					ocm[env->lastObjId()] = GraphicsTools::randomColor();
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
				for (auto& obj: env->objs()){
					if (obj.second.bbox()->containsPoint(mousePosition) && obj.second.selected()){
						objAtLocation = true;
		            	obj.second.setPos(mousePosition);
		                obj.second.setVel(Vec((event.motion.xrel) * (pixelsPerMeter * pixelsPerMeter / framesPerSecond),
		                				   (event.motion.yrel) * (pixelsPerMeter * pixelsPerMeter / framesPerSecond)));
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
	GraphicsTools::Window* mainWindow;
	CtrlSet mainCtrls;
	ObjColorMap mainOcm;

	mainCtrls["radius"] = Control("Radius", 15, 100, 30);
	mainCtrls["elast"] = Control("Elasticity", 0, 1, 1);
	mainCtrls["velx"] = Control("X velocity", -500, 500, 0);
	mainCtrls["vely"] = Control("Y velocity", -500, 500, 0);

	GraphicsTools::InitGraphics();
	GraphicsTools::InitText();

	if (argc == 3){
		mainWindow = new GraphicsTools::Window("Gravity Sim", atoi(argv[1]), atoi(argv[2]));
		mainEnv = new Environment(atoi(argv[2]), atoi(argv[1]), Vec(0, 9.8 * pixelsPerMeter));
	}
	else if (argc == 5){
		mainWindow = new GraphicsTools::Window("Gravity Sim", atoi(argv[1]), atoi(argv[2]));
		mainEnv = new Environment(atoi(argv[2]), atoi(argv[1]), Vec(atoi(argv[4]), atoi(argv[3]) * pixelsPerMeter));
	}
	else {
		mainWindow = new GraphicsTools::Window("Gravity Sim", SCREENWIDTH, SCREENHEIGHT);
		mainEnv = new Environment(SCREENHEIGHT, SCREENWIDTH, Vec(0, 9.8 * pixelsPerMeter));
	}

	SDL_Event ev;

    // While the program is running
    bool quit = false;
    while (!quit){
    	while (SDL_PollEvent(&ev)){
			quit = handleInput(ev, mainWindow, mainEnv, mainCtrls, mainOcm);
		}
        // Clear the screen
        // Update window
		if (mainEnv->time() % 60 == 0){
			mainEnv->print(std::cout);
		}

		updateSim(mainEnv);
		drawSim(mainWindow, mainEnv, mainOcm);
    }
    
    delete mainWindow; delete mainEnv;

	GraphicsTools::CloseText();
	GraphicsTools::CloseGraphics();

	return 0;
}
