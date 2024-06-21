#include "control.h"
#include <algorithm>

Control::Control() {}

Control::Control(std::string name, double min, double max, double inc,
                 double defaultVal)
    : _name(name), _min(min), _max(max), _inc(inc), _value(defaultVal) {}

void Control::setValue(double newValue) {
  _value = std::clamp(newValue, _min, _max);
};

void Control::changeValue(double delta) {
  _value = std::clamp(_value += delta, _min, _max);
}

void Control::operator++(int) { changeValue(_inc); }

void Control::operator--(int) { changeValue(-_inc); }

ControlSet::ControlSet() {}

Control &ControlSet::operator()(std::string ctrlName) {
  return _ctrls.at(ctrlName);
}

double ControlSet::operator[](std::string ctrlName) {
  return _ctrls.at(ctrlName).getValue();
}

void ControlSet::addCtrl(std::string ctrlName, Control ctrl){
  _ctrls.emplace(ctrlName, ctrl);
}
