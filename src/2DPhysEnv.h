/* A two-dimensional physics envrionment. */

#ifndef ENV2D_H
#define ENV2D_H

#include <cmath>
#include <vector>

#include "2DVec.h"
#include "rectangle.h"
#include "object.h"
#include "window.h"
#include "control.h"

class Environment {

public:

    /* Creates an environment with height and width both set
        to zero. */
    Environment();
    /* Creates an environment with dimensions and gravity
        specified. */
    Environment(Window* win, double, double, const Vec&);

    /* Destroys an environment. */
    ~Environment();

    /* Returns the environment's gravity. */
    const Vec& getG() const {return _g;};

    /* Returns the time the environment has
        run in the simulation. */
    int getT() const {return t;};
    
	SDL_Color getBackground() const {return _back;};
    
    void setBackground(SDL_Color c) {_back = c;};

    /* Receives input from an SDL event and
        modifies the environment and its
        objects accordingly. */
    bool handleInput(SDL_Event);
    
    /* Adds an object to the environment. */
    void addObj(const Object&);

    /* Removes an object from the environment. */
    void removeObj(int);

    /* Moves all the objects in the environment. */
    void moveObjs();

    /* Updates the environment, by moving all the
        objects and redrawing them at their new
        locations, and incrementing the run
        time. */
    void update();

    /* Prints all available information about the
        environment to the output given. */
    void print(std::ostream&) const;
    
    void draw() const;
    
private:
    
	BBox* _bbox; // environment boundary
    int t; // simulation time
    Vec _g; // environment gravity
    bool _paused; // environment run state
    std::vector<Object> _objs; // set of objects
    std::vector<Control> _ctrls; // set of controls
    SDL_Color _back; // environment background color
    Window* _win; // target drawing window

};


#endif
