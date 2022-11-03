#include "control.h"
#include <algorithm>

Control::Control() {}

Control::Control(std::string name, double min, double max, double defaultVal)
    : _name(name), _min(min), _max(max), _value(defaultVal) {}

void Control::changeValue(double delta) {
  _value = std::clamp(_value += delta, _min, _max);
}
