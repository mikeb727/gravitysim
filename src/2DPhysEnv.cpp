#include <iostream>
#include <cstdlib>

#include "2DPhysEnv.h"

using std::cout;
using std::cerr;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

const double pixelsPerMeter = 80;

Environment::Environment(): t (0){}

Environment::Environment(double W, double H, const Vec& G): _bbox (new Rectangle(Vec(0, 0, true), W, H)), t(0), _g (G), _paused (false) {
	cerr << "Environment created (" << _bbox->getW() << "x" << _bbox->getH() << ", " << _g << ")\n";
}

Environment::~Environment() {
	_objs.clear();
	cerr << "Environment destroyed\n";
};

void Environment::addObj(const Object& obj){

    _objs.push_back(obj);

}

void Environment::removeObj(int objIndex){

    _objs.erase(_objs.begin() + objIndex);

}

void Environment::moveObjs(){

    // Check for collisions
	for (int i = 0; i < _objs.size(); i++){
		for (int j = 0; j < _objs.size(); j++){
			if (i != j && _objs[i].getBBox()->distanceFrom(*_objs[j].getBBox()) < 200){
				//cerr << i << ' ' << j << endl;
				if (_objs[i].collidesWith(_objs[j])){
					_objs[i].resolveCollision(_objs[j]);
				}
			}
		}
	}
	// Move objects
    for (int i = 0; i < _objs.size(); i++){
		/* Set the object's acceleration to the environment's gravity */
		if (!_objs[i].getSelectState()){
		    _objs[i].setAccel(_g);
			/* Move the object, reversing if any part of the object would leave the environment */
			_objs[i].move(!(_bbox->containsPoint(_objs[i].nextBBox()->left()) && _bbox->containsPoint(_objs[i].nextBBox()->right())),
						  !(_bbox->containsPoint(_objs[i].nextBBox()->top()) && _bbox->containsPoint(_objs[i].nextBBox()->bottom())));
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
        for (int i = 0; i < _objs.size(); i++){
            out << "    " << i << " (";
            _objs[i].print(out); out << ")\n";
        }
    }
    else {
        out << " none\n";
    }

}
