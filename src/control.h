#ifndef CONTROL_H
#define CONTROL_H

#include <algorithm>
#include <map>
#include <string>

class Control {
public:
  Control();
  Control(std::string name, double min, double max, double inc,
          double defaultVal);
  void setValue(double newValue);
  void changeValue(double delta);
  double getValue() const { return _value; };
  void operator--(int);
  void operator++(int);

private:
  std::string _name;
  // _inc is default increment from xml
  double _min, _value, _inc, _max;
};

struct ControlSet {
  ControlSet();
  // (): get control (for change operations)
  // []: get value
  Control &operator()(std::string ctrlName);
  double operator[](std::string ctrlName);
  std::map<std::string, Control> _ctrls;
  void addCtrl(std::string, Control);
};

#endif
