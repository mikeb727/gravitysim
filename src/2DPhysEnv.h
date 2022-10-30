/* A two-dimensional physics envrionment. */

#ifndef ENV2D_H
#define ENV2D_H

#include <cmath>
#include <vector>

#include "2DVec.h"
#include "rectangle.h"
#include "object.h"

typedef std::vector<Object> EnvObjSet;

class Environment {

public:

    /* Creates an environment with height and width both set
        to zero. */
    Environment();
    /* Creates an environment with dimensions and gravity
        specified. */
    Environment(double width, double height, const Vec& gravity);

    /* Destroys an environment. */
    ~Environment();

    /* Returns the environment's gravity. */
    const Vec& getG() const {return _g;};

    /* Returns the list of objects. */
    EnvObjSet& getObjList() {return _objs;};

    /* Returns the time the environment has
        run in the simulation. */
    int getT() const {return t;};

    const BBox* getBBox() const {return _bbox;};
    
    /* Adds an object to the environment. */
    void addObj(const Object& newObj);

    /* Removes an object from the environment. */
    void removeObj(int objIndex);

    /* Clears all objects from the environment. */
    void clearObjs() {_objs.clear();};

    /* Toggles the run state */
    void togglePause() {_paused = _paused ? false : true;};

    /* Moves all the objects in the environment. */
    void moveObjs();

    /* Updates the environment, by moving all the
        objects and redrawing them at their new
        locations, and incrementing the run
        time. */
    void update();

    /* Prints all available information about the
        environment to the output given. */
    void print(std::ostream& dest) const;
    
private:
    
	BBox* _bbox; // environment boundary
    int t; // simulation time
    Vec _g; // environment gravity
    bool _paused; // environment run state
    EnvObjSet _objs; // set of objects

};


#endif
