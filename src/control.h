#ifndef CONTROL_H
#define CONTROL_H

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

class ControlSet {
public:
  ControlSet();
  // (): get control (for change operations)
  // []: get value
  Control &operator()(std::string ctrlName);
  double operator[](std::string ctrlName);
  void addCtrl(std::string ctrlName, Control ctrl);
private:
  std::map<std::string, Control> _ctrls;
};

#endif
