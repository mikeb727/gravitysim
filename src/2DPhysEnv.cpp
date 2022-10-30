#include <iostream>
#include <cstdlib>

#include "2DPhysEnv.h"

using std::cout;
using std::cerr;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

const double pixelsPerMeter = 80;

Environment::Environment(): t (0){
	SDL_Color back = {200, 200, 200, 0};
}

Environment::Environment(Window* w, double W, double H, const Vec& G): _bbox (new Rectangle(Vec(0, 0, true), W, H)), _win (w), t(0), _g (G), _paused (false) {
	_ctrls.push_back(Control(_win, "Radius", 3, 100, 30, 18, 150, Vec(24, 24)));
	_ctrls.push_back(Control(_win, "Elasticity", 0, 1, 1, 18, 150, Vec(228, 24)));
	cerr << "Environment created (" << _bbox->getW() << "x" << _bbox->getH() << ", " << _g << ")\n";
}

Environment::~Environment() {
	_objs.clear();
	cerr << "Environment destroyed\n";
};

bool Environment::handleInput(SDL_Event event){

    if (event.type == SDL_MOUSEBUTTONDOWN){
		Vec mousePosition(event.button.x, event.button.y);
		bool objAtLocation = false, ctrlAtLocation = false;
		if (event.button.button == SDL_BUTTON_LEFT){
			for (int i = 0; i < _ctrls.size(); i++){
				if (_ctrls[i].containsPoint(mousePosition)){
					ctrlAtLocation = true;
					_ctrls[i].setFromPosition(mousePosition);
					break;
				}
			}
			if (!ctrlAtLocation){
				for (int i = 0; i < _objs.size(); i++){
					if (_objs[i].getBBox()->containsPoint(mousePosition)){
						objAtLocation = true;
						_objs[i].setSelectState(true);
						break;
					}
				}
			}
		}
		if (event.button.button == SDL_BUTTON_RIGHT){
			for (int i = 0; i < _objs.size(); i++){
				if (_objs[i].getBBox()->containsPoint(mousePosition)){
					objAtLocation = true;
                    removeObj(i);
					break;
				}
			}		
		}
	}   
    
    if (event.type == SDL_MOUSEBUTTONUP){
    	Vec mousePosition(event.button.x, event.button.y);
    	bool objAtLocation = false, ctrlAtLocation = false;
        if (event.button.button == SDL_BUTTON_LEFT){
			for (int i = 0; i < _ctrls.size(); i++){
				if (_ctrls[i].containsPoint(mousePosition)){
					ctrlAtLocation = true;
					break;
				}
			}
			for (int i = 0; i < _objs.size(); i++){
				if (_objs[i].getBBox()->containsPoint(mousePosition)){
					objAtLocation = true;
					_objs[i].setSelectState(false);
					break;
				}
			}
			if (!objAtLocation && !ctrlAtLocation){
				if (_bbox->containsBBox(Circle(mousePosition, _ctrls[0].getValue()))){
					addObj(Object(new Circle(mousePosition, _ctrls[0].getValue()), 1, mousePosition, Vec(0, 0, true), _ctrls[1].getValue(), _win));
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
			for (int i = 0; i < _ctrls.size(); i++){
				if (_ctrls[i].containsPoint(mousePosition)){
					ctrlAtLocation = true;
					_ctrls[i].setFromPosition(mousePosition);			
					break;
				}
			}
			if (!ctrlAtLocation){
				for (int i = 0; i < _objs.size(); i++){
					if (_objs[i].getBBox()->containsPoint(mousePosition) && _objs[i].getSelectState()){
						objAtLocation = true;
		            	_objs[i].setPos(Vec(mousePosition));
		                _objs[i].setVel(Vec((event.motion.xrel) * (pixelsPerMeter * pixelsPerMeter / framesPerSecond),
		                				   (event.motion.yrel) * (pixelsPerMeter * pixelsPerMeter / framesPerSecond), true));
						break;
					}
				}	
			}
        }
    }

    
    return false;
}

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
			//objs[i].move(!((0 <= objs[i].nextPos().x() - objs[i].getR()) && (objs[i].nextPos().x() + objs[i].getR() <= _w)),
			//             !((0 <= objs[i].nextPos().y() - (objs[i].getR())) && (objs[i].nextPos().y() + (objs[i].getR() ) <= _h)));
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
