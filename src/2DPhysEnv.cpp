#include <iostream>
#include <cstdlib>

#include "2DPhysEnv.h"

using std::cout;
using std::cerr;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

const double pixelsPerMeter = 80;

Environment::Environment(): t (0) {}

Environment::Environment(double W, double H, const Vec& G): _bbox (new Rectangle(Vec(0, 0, true), W, H)), t(0), _g (G), _paused (false), nextObjId(0) {
	cerr << "Environment created (" << _bbox->getW() << "x" << _bbox->getH() << ", " << _g << ")\n";
}

Environment::~Environment() {
	_objs.clear();
	cerr << "Environment destroyed\n";
};

void Environment::addObj(const Object& obj){

    _objs[nextObjId++] = obj;

}

void Environment::removeObj(int objId){

    _objs.erase(objId);

}

void Environment::moveObjs(){

    // Check for collisions
	for (auto& obj1: _objs){
		for (auto& obj2: _objs){
			if (obj1.first != obj2.first && obj1.second.getBBox()->distanceFrom(*obj2.second.getBBox()) < 200){
				if (obj1.second.collidesWith(obj2.second)){
					obj1.second.resolveCollision(obj2.second);
				}
			}
		}
	}
	// Move objects
    for (auto& obj: _objs){
		/* Set the object's acceleration to the environment's gravity */
		if (!obj.second.getSelectState()){
		    obj.second.setAccel(_g);
			/* Move the object, reversing if any part of the object would leave the environment */
			obj.second.move(!(_bbox->containsPoint(obj.second.nextBBox()->left()) && _bbox->containsPoint(obj.second.nextBBox()->right())),
						  !(_bbox->containsPoint(obj.second.nextBBox()->top()) && _bbox->containsPoint(obj.second.nextBBox()->bottom())));
		}

    }
}

void Environment::update() {

    if (!_paused){
        moveObjs();
        t++;
    }

}

void Environment::print(std::ostream& out) const {

    out << "[" << t  << "]"<<  (_paused ? " (environment paused)" : "") << "\n";
     out << "Objects:";
    if (_objs.size() > 0){
        out << "\n";
        for (auto& obj: _objs){
            out << "    " << obj.first << " (";
            obj.second.print(out); out << ")\n";
        }
    }
    else {
        out << " none\n";
    }

}
