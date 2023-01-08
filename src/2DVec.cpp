#include <cmath>
#include <iomanip>
#include <iostream>

#include "2DVec.h"

Vec::Vec() : _x(0), _y(0) {}

Vec::Vec(double x, double y, VecMode mode) {
  if (mode == MagDir) {
    // using x as mag, y as dir
    _x = x * cos(y);
    _y = x * sin(y);
  } else {
    _x = x;
    _y = y;
  }
}

Vec::Vec(const Vec &v) {
  _x = v._x;
  _y = v._y;
}

Vec &Vec::operator=(const Vec &v) {
  if (this != &v) {
    _x = v._x;
    _y = v._y;
  }
  return *this;
}

Vec::~Vec() {}

double Vec::mag() const { return sqrt(pow(_x, 2) + pow(_y, 2)); }

double Vec::dir() const { return atan2(_y, _x); }

Vec Vec::unit() const { return Vec(1, atan2(_y, _x), MagDir); }

Vec Vec::plus(const Vec &v) const { return Vec(x() + v.x(), y() + v.y()); }
Vec Vec::operator+(const Vec &v) const { return plus(v); }

Vec Vec::minus(const Vec &v) const { return Vec(x() - v.x(), y() - v.y()); }
Vec Vec::operator-(const Vec &v) const { return minus(v); }

Vec Vec::scalarMultiple(double k) const { return Vec(x() * k, y() * k); }
Vec Vec::operator*(double k) const { return scalarMultiple(k); }
Vec operator*(double k, const Vec &v) { return v * k; }

Vec Vec::operator/(double k) const { return Vec(x() / k, y() / k); }

Vec Vec::dot(const Vec &v) const { return Vec(x() * v.x(), y() * v.y()); }

double Vec::cross(const Vec &v) const { return (x() * v.y() - y() * v.x()); }

bool Vec::equals(const Vec &v) const {
  return fabs(_x - v._x) < 1e-10 && fabs(_y - v._y) < 1e-10;
}
bool Vec::operator==(const Vec &v) const { return (equals(v)); }

void Vec::print(std::ostream &out, bool componentForm = true) const {
  if (componentForm) {
    out << "(" << x() << "," << y() << ")";
  } else {
    out << "(mag " << _x << ", dir " << _y << ")";
  }
}

void Vec::debugPrint(std::ostream &out) const {
  out << _x << ' ' << _y << ' ' << x() << ' ' << y();
}

Vec euler(const Vec &v, const Vec &dv, double dt) { return v + (dv * dt); }

Vec rk4(const Vec &v, const Vec &dv, const Vec &ddv, double dt) {
  Vec k1 = dv;
  Vec k2 = euler(k1, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec k3 = euler(k2, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec k4 = euler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 2 * k2 + 2 * k3 + k4) / 6));
}