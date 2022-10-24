/* An object within a two-dimensional physics
    environment. */

#ifndef BALL_H
#define BALL_H

#include "2DVec.h"
#include "circle.h"

#include <iostream>
#include <cmath>

#include "window.h"

class Ball : public Circle {
public:

    /* Creates an object at (0, 0) with no velocity or elasticity,
        with no image. */
	Ball();

	/* Creates an object with the radius, mass, position, elasticity,
        and velocity specified, to be displayed in the renderer
        specified. */
	Ball(Window*, const double&, const Vec&, const double&, const Vec&);

    /* Destroys the object, printing a message indicating its destruction. */
	~Ball();

	/* Returns the position of the object in the next frame. */
	Vec nextPos() const;

	/* Returns the bounding box of the object in the next frame. */
	Circle nextBBox() const;

	/* Returns the velocity of the object. */
	const Vec&&getVel() const {return vel;};

	/* Returns the acceleration of the object. */
	const Vec&&getAccel() const {return accel;};

    /* Returns the object's mass. */
    double getM() const {return m;};
    
    /* Returns the object's color. */
    SDL_Color getColor() const {return color;};

	/* Returns the object's opacity. */
	int getAlpha() const {return alpha;};

	/* Sets the acceleration of the object. This function is to be used
		by the environment. */
	void setAccel(const Vec&);

    /* Whether the object is currently selected (left-clicked and dragged)
        by a user. If an object is selected, it will not move according to
        the physical laws of the environment. This function returns true
        if the object is selected and false otherwise. */
    bool getSelectState() const {return selectState;};
    
    void setSelectState(bool ss) {selectState = ss;};

    /* Whether the object will collide with another object. This function
        returns true if at least one of the object's points is within or
        on the border of the other object, and false otherwise. */
    bool collidesWith(const Ball&) const;

	/* Moves the object according to physics. */
	void move(bool, bool);

	/* Reverses the object's direction. */
	void reverseDirection(bool, bool);

	/* Resolves a collision between the object and another object. */
	void resolveCollision(Ball&);

	/* Sets the object's position to the one given. */
	void setPos(const Vec&);

	/* Sets the object's velocity according to a vector. */
	void setVel(const Vec&);

    /* Sets the select state to false, effectively signaling that the object
        is free to move according to the environment's physical laws again. */
	void unselect() {selectState = false;};

	/* Prints all available information about the object to the output
        location specified. */
    void print(std::ostream&) const;
    
    void draw() const;
    
private:

  double m; // object mass
	Vec vel; // object velocity
	Vec accel; // object acceleration
	double elasticity; // ratio of pre- to post- collision velocity
	
	bool selectState; // whether the object is being clicked
	
	SDL_Color color; // object color
	int alpha; // object image opacity
	
	SDL_Texture* img; // object image
	Window* win; // target drawing window

	std::string objType;

};

#endif
