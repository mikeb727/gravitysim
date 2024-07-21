#include <cmath>
#include <iostream>

#include "2DVec.h"

const double EQ_TOLERANCE = 1e-10;

Vec2::Vec2() : _x(0), _y(0) {}

Vec2::Vec2(double x, double y, VecMode mode) {
  if (mode == MagDir) {
    // using x as mag, y as dir
    _x = x * cos(y);
    _y = x * sin(y);
  } else {
    _x = x;
    _y = y;
  }
}

Vec2::Vec2(const Vec2 &v) {
  _x = v._x;
  _y = v._y;
}

Vec2 &Vec2::operator=(const Vec2 &v) {
  if (this != &v) {
    _x = v._x;
    _y = v._y;
  }
  return *this;
}

Vec2::~Vec2() {}

double Vec2::mag() const { return sqrt(pow(_x, 2) + pow(_y, 2)); }

double Vec2::dir() const { return atan2(_y, _x); }

Vec2 Vec2::unit() const { return Vec2(1, dir(), MagDir); }

Vec2 Vec2::plus(const Vec2 &v) const { return Vec2(x() + v.x(), y() + v.y()); }
Vec2 Vec2::operator+(const Vec2 &v) const { return plus(v); }
Vec2 &Vec2::operator+=(const Vec2 &v) {
  *this = this->plus(v);
  return *this;
}

Vec2 Vec2::minus(const Vec2 &v) const { return Vec2(x() - v.x(), y() - v.y()); }
Vec2 Vec2::operator-(const Vec2 &v) const { return minus(v); }

Vec2 Vec2::scalarMultiple(double k) const { return Vec2(x() * k, y() * k); }
Vec2 Vec2::operator*(double k) const { return scalarMultiple(k); }
Vec2 operator*(double k, const Vec2 &v) { return v * k; }
Vec2 Vec2::operator-() const { return scalarMultiple(-1); }

Vec2 Vec2::operator/(double k) const { return Vec2(x() / k, y() / k); }

double Vec2::dot(const Vec2 &v) const { return x() * v.x() + y() * v.y(); }

double Vec2::cross(const Vec2 &v) const { return (x() * v.y() - y() * v.x()); }

bool Vec2::equals(const Vec2 &v) const {
  return std::abs(_x - v._x) < EQ_TOLERANCE &&
         std::abs(_y - v._y) < EQ_TOLERANCE;
}
bool Vec2::operator==(const Vec2 &v) const { return (equals(v)); }

void Vec2::print(std::ostream &out, bool componentForm = true) const {
  if (componentForm) {
    out << "(" << x() << "," << y() << ")";
  } else {
    out << "(mag " << _x << ", dir " << _y << ")";
  }
}

Vec2 euler(const Vec2 &v, const Vec2 &dv, double dt) { return v + (dv * dt); }

Vec2 rk4(const Vec2 &v, const Vec2 &dv, const Vec2 &ddv, double dt) {
  Vec2 k1 = dv;
  Vec2 k2 = euler(k1, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec2 k3 = euler(k2, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec2 k4 = euler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 2 * k2 + 2 * k3 + k4) / 6));
}

Vec2 rk4_38(const Vec2 &v, const Vec2 &dv, const Vec2 &ddv, double dt) {
  Vec2 k1 = dv;
  Vec2 k2 = 0.333 * euler(k1, dv + (0.333 * dt * ddv), 0.333 * dt);
  Vec2 k3 = -0.333 * euler(k1, dv + (0.667 * dt * ddv), 0.667 * dt) +
            euler(k2, dv + (0.667 * dt * ddv), 0.667 * dt);
  Vec2 k4 = euler(k1, dv + (dt * ddv), dt) - euler(k2, dv + (dt * ddv), dt) +
            euler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 3 * k2 + 3 * k3 + k4) / 8));
}
