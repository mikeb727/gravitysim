#include "object.h"
#include <cmath>

using namespace std;

/* The scale of the simulation. */
const double pixelsPerMeter = 80;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

Object::Object() : _bbox (new Circle(Vec(0, 0, true), 10)), _m(1), _elast(0), objType("Object") {}

Object::Object(BBox* bounds, double mass, const Vec& position, const Vec& velocity, const double& elasticity, Window* win)
: _win(win), _bbox(bounds), _m(mass), _vel(velocity), _elast(elasticity), objType("Object"){
	SDL_Color objColor = {rand() % 255, rand() % 255, rand() % 255, 0};
	while (objColor.r + objColor.g + objColor.b > 400){
		objColor.r = rand() % 255;
		objColor.g = rand() % 255;
		objColor.b = rand() % 255;
	}
	_color = objColor;
	cerr << "Object created at " << _bbox->getPos() << " with velocity " << _vel << " and elasticity " << elasticity << endl;
}

Object::~Object() {
    cerr << "Object at " << _bbox->getPos() << " destroyed" << endl;
}

bool Object::collidesWith(const Object& otherObj) const {

    if (this == &otherObj) return false;
    return nextBBox()->intersects(*otherObj.nextBBox());

}

void Object::resolveCollision(Object& otherObj){
	double sumVel = _vel.mag() + otherObj._vel.mag();
	double sumMass = _m + otherObj._m;
	Vec collVec = otherObj._bbox->getPos() - _bbox->getPos();
	double resultAngle = _vel.dir() + ((collVec.dir() - _vel.dir()) * 2) + M_PI;
	/*
	if (_pos.x() <= otherObj._pos.x()){ // to the left
		newVel = Vec((m * sumVel / sumMass), 2 * newVel.dir() - M_PI, false);
	}
	else { // to the right
		newVel = Vec((m * sumVel / sumMass), 2 * newVel.dir(), false);vel
	}
	*/
	_vel = _elast * Vec((otherObj._m * sumVel / sumMass), resultAngle, false);
	otherObj._vel = _elast * Vec((_m * sumVel / sumMass), resultAngle + M_PI, false);
}

Vec Object::nextPos() const {

	return rk4(_bbox->getPos(), _vel, _accel, (1.0 / framesPerSecond));

}

BBox* Object::nextBBox() const {
	return _bbox->shift(nextPos() - _bbox->getPos());
}

void Object::move(bool reverseH = false, bool reverseV = false){

	_vel = euler(_vel, _accel, (1.0 / framesPerSecond));
  // _vel = rk4(_vel, _accel, Vec(0, 0), (1.0/framesPerSecond));

  reverseDirection(reverseH, reverseV);
	_bbox->setPos(nextPos());

}

void Object::reverseDirection(bool h, bool v){
  
  _vel = Vec((h ? -1 : 1) * _vel.x(), (v ? -1 : 1) * _vel.y()) * _elast;

}

void Object::setPos(const Vec& newPos){

    _bbox->setPos(newPos);

}

void Object::setVel(const Vec& vec){

	_vel = vec;

}

void Object::setAccel(const Vec& vec){

	_accel = vec;

}

void Object::print(std::ostream& out) const {

	out << objType << ", radius "  << ", " << _bbox->getPos() << ", " << _vel << ", " << _accel << (selectState ? "selected" : "");

}

void Object::draw() const{
	_bbox->draw(_win, _color);
}
