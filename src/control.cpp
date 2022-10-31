#include "control.h"

Control::Control(){}

Control::Control(std::string name, double min, double max, double defaultVal)
: _name (name), _min (min), _max (max), _value (defaultVal) {}

void Control::changeValue(double delta){
	_value = std::max(_min, std::min(_value += delta, _max));
}
