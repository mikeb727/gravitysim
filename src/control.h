#ifndef CONTROL_H
#define CONTROL_H

#include <algorithm>
#include <string>

class Control {
public:
  Control();
  Control(std::string name, double min, double max, double defaultVal);
  void setValue(double newValue) { _value = std::clamp(_value, _min, _max); };
  void changeValue(double delta);
  double getValue() const { return _value; };

private:
  std::string _name;
  double _min, _value, _max;
};

#endif
