#include <cmath>
#include <iomanip>
#include <iostream>

#include "2DVec.h"

Vec::Vec() : magnitude(0), direction(0) {}

Vec::Vec(double x, double y, VecMode mode) {
  if (mode == Component) {
    magnitude = sqrt(pow(x, 2) + pow(y, 2));
    direction = atan2(y, x);
  } else {
    magnitude = x;
    direction = y;
  }
}

Vec::Vec(const Vec &v) {
  magnitude = v.magnitude;
  direction = v.direction;
}

Vec &Vec::operator=(const Vec &v) {
  if (this != &v) {
    magnitude = v.magnitude;
    direction = v.direction;
  }
  return *this;
}

Vec::~Vec() {}

double Vec::x() const { return magnitude * cos(direction); }

double Vec::y() const { return magnitude * sin(direction); }

Vec Vec::unit() const { return Vec(x() / magnitude, y() / magnitude, Component); }

Vec Vec::plus(const Vec &v) const {
  return Vec(x() + v.x(), y() + v.y(), Component);
}
Vec Vec::operator+(const Vec &v) const { return plus(v); }

Vec Vec::minus(const Vec &v) const {
  return Vec(x() - v.x(), y() - v.y(), Component);
}
Vec Vec::operator-(const Vec &v) const { return minus(v); }

Vec Vec::scalarMultiple(double k) const { return Vec(x() * k, y() * k, Component); }
Vec Vec::operator*(double k) const { return scalarMultiple(k); }
Vec operator*(double k, const Vec &v) { return v * k; }

Vec Vec::operator/(double k) const { return Vec(x() / k, y() / k, Component); }

Vec Vec::dot(const Vec &v) const { return Vec(x() * v.x(), y() * v.y(), Component); }

double Vec::cross(const Vec &v) const {
  return (x() * v.y() - y() * v.x());
}

bool Vec::equals(const Vec &v) const {
  return (magnitude == v.magnitude && direction == v.direction);
}
bool Vec::operator==(const Vec &v) const { return (equals(v)); }

void Vec::print(std::ostream &out, bool componentForm = true) const {
  if (componentForm) {
    out << "(" << x() << "," << y() << ")";
  } else {
    out << "(mag " << magnitude << ", dir " << direction << ")";
  }
}

void Vec::debugPrint(std::ostream &out) const {
  out << magnitude << ' ' << direction << ' ' << x() << ' ' << y();
}

Vec euler(const Vec &v, const Vec &dv, double dt) { return v + (dv * dt); }

Vec rk4(const Vec &v, const Vec &dv, const Vec &ddv, double dt) {
  Vec k1 = dv;
  Vec k2 = euler(k1, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec k3 = euler(k2, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec k4 = euler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 2 * k2 + 2 * k3 + k4) / 6));
}