#ifndef CONTROL_H
#define CONTROL_H

#include "window.h"
#include "rectangle.h"

#include <string>
#include <sstream>

class Control : public Rectangle {
public:
	Control();
	Control(Window* win, std::string name, double min, double max, double defValue, int height, int width, const Vec& position);
	void setValue(double newValue) {_value = newValue;};
	void setFromPosition(const Vec& position);
	void increaseValue(double change);
	void decreaseValue(double change);
	double getValue() const {return _value;};
	void draw() const;
private:
	std::string _name;
	double _min, _value, _max;
	Rectangle _interactiveArea;
	Window* _win;
};


#endif
