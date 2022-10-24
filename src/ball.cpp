#include "ball.h"
#include <cmath>

using namespace std;

/* The scale of the simulation. */
const double pixelsPerMeter = 80;

/* Pi. */
const double pi = 3.1415926535;

/* The speed of the simulation, in frames per second. */
const int framesPerSecond = 60;

Ball::Ball(): Circle(Vec(0, 0, true), 10), m (1), elasticity (0), alpha (255), objType ("Ball") {}

Ball::Ball(Window* w, const double& R, const Vec& P, const double& E, const Vec& V)
: win (w), Circle(P, R), m (R * R * R), vel (V), elasticity (E), alpha (255), objType ("Ball"){
	SDL_Color objColor = {rand() % 255, rand() % 255, rand() % 255, 0};
	while (objColor.r + objColor.g + objColor.b > 400){
		objColor.r = rand() % 255;
		objColor.g = rand() % 255;
		objColor.b = rand() % 255;
	}
	color = objColor;
	cerr << "Ball created at " << _pos << " with velocity " << vel << " and elasticity " << elasticity << endl;
}

Ball::~Ball() {
    cerr << "Ball destroyed" << endl;
    
}

bool Ball::collidesWith(const Ball& otherObj) const {

    if (this == &otherObj) return false;
    return nextBBox().intersects(otherObj.nextBBox());

}

void Ball::resolveCollision(Ball& otherObj){
	double sumVel = vel.mag() + otherObj.vel.mag();
	double sumMass = m + otherObj.m;
	Vec collVec = otherObj._pos - _pos;
	double resultAngle = vel.dir() + ((collVec.dir() - vel.dir()) * 2) + pi;
	/*
	if (_pos.x() <= otherObj._pos.x()){ // to the left
		newVel = Vec((m * sumVel / sumMass), 2 * newVel.dir() - pi, false);
	}
	else { // to the right
		newVel = Vec((m * sumVel / sumMass), 2 * newVel.dir(), false);vel
	}
	*/
	vel = elasticity * Vec((otherObj.m * sumVel / sumMass), resultAngle, false);
	otherObj.vel = elasticity * Vec((m * sumVel / sumMass), resultAngle + pi, false);
}

Vec Ball::nextPos() const {

	return rk4(_pos, vel, accel, (1.0 / framesPerSecond));

}

Circle Ball::nextBBox() const {
	return Circle(nextPos(), _r);

}

void Ball::move(bool reverseH = false, bool reverseV = false){

	if (reverseH && reverseV){
		reverseDirection(true, true);
	}
	else if (reverseH){
		reverseDirection(true, false);
	}
	else if (reverseV){
		reverseDirection(false, true);
	}
	else {
		_pos = nextPos();
	}

	// cerr << vel << endl;
}

void Ball::reverseDirection(bool h, bool v){

	if (h){
		vel = Vec(-1 * vel.x(), vel.y(), true) * elasticity;
	}

	if (v){
		vel = Vec(vel.x(), -1 * vel.y(), true) * elasticity;
	}

}
/*
void Ball::resolveCollisionWith (Ball& otherObj){

    double collisionAngle = atan2(pos.y() - otherObj.pos.y(), pos.x() - otherObj.pos.x());

	Vec objVel = vel;
	Vec otherVel = otherVel;

	setVel(Vec(otherVel.mag(), collisionAngle, false));
	otherObj.setVel(Vec(objVel.mag(), collisionAngle + pi, false));

	setVel(Vec((otherVel.mag() * cos(otherVel.dir() - collisionAngle) * cos(collisionAngle)) + (objVel.mag() * sin(objVel.dir() - collisionAngle) * cos(collisionAngle + (pi / 2))),
			   (otherVel.mag() * cos(otherVel.dir() - collisionAngle) * sin(collisionAngle)) + (objVel.mag() * sin(objVel.dir() - collisionAngle) * sin(collisionAngle + (pi / 2))), true));
	otherObj.setVel(Vec((objVel.mag() * cos(objVel.dir() - collisionAngle) * cos(collisionAngle)) + (otherVel.mag() * sin(otherVel.dir() - collisionAngle) * cos(collisionAngle + (pi / 2))),
			   			(objVel.mag() * cos(objVel.dir() - collisionAngle) * sin(collisionAngle)) + (otherVel.mag() * sin(otherVel.dir() - collisionAngle) * sin(collisionAngle + (pi / 2))), true));


	// objs[i].setVel(Vec(objs[j].vel.mag(), collisionAngle, true));
	// objs[j].setVel(Vec(objs[i].vel.mag(), collisionAngle + pi, true));

}
*/

void Ball::setPos(const Vec& newPos){

    _pos = Vec(newPos.x() - _r / 4, newPos.y() - _r / 4);

}

void Ball::setVel(const Vec& vec){

	vel = vec;

}

void Ball::setAccel(const Vec& vec){

	accel = vec;

}

void Ball::print(std::ostream& out) const {

	out << objType << ", radius " << _r << ", " << _pos << ", " << vel << ", " << accel << (selectState ? "selected" : "");

}

void Ball::draw() const{
	win->drawCircleGradient(color, ImageTools::blend(color, 0.6, Colors::white, 0.4), _pos.x(), _pos.y(), _r);
}
