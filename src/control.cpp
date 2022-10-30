#include "control.h"

using namespace std;

Control::Control(){}

Control::Control(Window* win, std::string name, double min, double max, double defValue, int height, int width, const Vec& position)
: _win (win), _name (name), _min (min), _max (max), _value (defValue), Rectangle(position, height + ImageTools::fontSize, width),
  _interactiveArea (Rectangle(_pos + Vec(0, ImageTools::fontSize + 4), _h - ImageTools::fontSize, _w)){}

void Control::draw() const {
	stringstream text; text << _name << ": " << _value;
	_win->drawText(text.str(), Colors::black, _pos.x(), _pos.y());
	_win->drawRectangle(Colors::red, _interactiveArea.getPos().x(), _interactiveArea.getPos().y(),
						(int)(_interactiveArea.getW() * (_value / _max)), _interactiveArea.getH());
}

void Control::setFromPosition(const Vec& position){
	if (containsPoint(position)){
		if (_interactiveArea.containsPoint(position)){
			setValue(_min + (((position.x() - _interactiveArea.left().x()) * (_max - _min)) / _interactiveArea.getW()));
		}
	}
}

void Control::changeValue(double delta){
	_value = max(_min, min(_value += delta, _max));
}
