/* This program allows a user to interact with
    objects within a two-dimensional physical
    environment. Left clicking within the
    environment creates an object, while right-
    clicking on an object removes it. The user
    can grab and throw objects by holding down
    the left mouse button and moving the mouse. */

#include <iostream>
#include <vector>

#include "2DPhysEnv.h"

using namespace std;

/* The dimensions of the simulation window. */
const int SCREENWIDTH = 1024;
const int SCREENHEIGHT = 768;

/* The scale of the simulation ([pixelsPerMeter] pixels = 1 meter). */
const double pixelsPerMeter = 80;

int main(int argc, char* argv[]){
	
	Window* mainWindow;
	Environment* mainEnv;

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
			quit = mainEnv->handleInput(ev);
		}
        // Clear the screen
        // Update window
		if (mainEnv->getT() % 60 == 0){
			mainEnv->print(cout);
		}
        mainWindow->clear();
        mainEnv->update();
		mainEnv->draw();
        mainWindow->update();

    }
    
    delete mainWindow; delete mainEnv;
    
    //cin.get();

	return 0;
}
