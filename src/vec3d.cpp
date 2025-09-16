#include <cmath>
#include <iostream>

#include "vec3d.h"

const double EQ_TOLERANCE = 1e-10;

Vec3::Vec3() : _x(0), _y(0), _z(0) {}

Vec3::Vec3(double x, double y) {
  _x = x;
  _y = y;
  _z = 0;
}

Vec3::Vec3(double x, double y, double z) {
  _x = x;
  _y = y;
  _z = z;
}

Vec3::Vec3(const Vec3 &dir, double mag) {
  double lat, lon;
  lat = std::atan2(dir.unit()._y, std::hypot(dir.unit()._x, dir.unit()._z));
  lon = std::atan2(dir.unit()._z, dir.unit()._x);
  _x = mag * cos(lat) * sin(lon);
  _y = mag * sin(lat);
  _z = mag * cos(lat) * cos(lon);
}

Vec3::Vec3(const Vec3 &v) {
  _x = v._x;
  _y = v._y;
  _z = v._z;
}

Vec3 &Vec3::operator=(const Vec3 &v) {
  if (this != &v) {
    _x = v._x;
    _y = v._y;
    _z = v._z;
  }
  return *this;
}

Vec3::~Vec3() {}

double Vec3::mag() const { return sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2)); }

Vec3 Vec3::unit() const {
  return mag() < EQ_TOLERANCE ? Vec3()
                              : Vec3(_x / mag(), _y / mag(), _z / mag());
}

Vec3 Vec3::plus(const Vec3 &v) const {
  return Vec3(x() + v.x(), y() + v.y(), z() + v.z());
}
Vec3 Vec3::operator+(const Vec3 &v) const { return plus(v); }
Vec3 &Vec3::operator+=(const Vec3 &v) {
  *this = this->plus(v);
  return *this;
}

Vec3 Vec3::minus(const Vec3 &v) const {
  return Vec3(x() - v.x(), y() - v.y(), z() - v.z());
}
Vec3 Vec3::operator-(const Vec3 &v) const { return minus(v); }

Vec3 Vec3::scalarMultiple(double k) const {
  return Vec3(x() * k, y() * k, z() * k);
}
Vec3 Vec3::operator*(double k) const { return scalarMultiple(k); }
Vec3 operator*(double k, const Vec3 &v) { return v * k; }
Vec3 Vec3::operator-() const { return scalarMultiple(-1); }

Vec3 Vec3::operator/(double k) const { return scalarMultiple(1 / k); }

double Vec3::dot(const Vec3 &v) const {
  return x() * v.x() + y() * v.y() + z() * v.z();
}

Vec3 Vec3::cross(const Vec3 &v) const {
  return Vec3((y() * v.z() - z() * v.y()), (z() * v.x() - x() * v.z()),
              (x() * v.y() - y() * v.x()));
}

bool Vec3::equals(const Vec3 &v) const {
  return std::abs(_x - v._x) < EQ_TOLERANCE &&
         std::abs(_y - v._y) < EQ_TOLERANCE &&
         std::abs(_z - v._z) < EQ_TOLERANCE;
}
bool Vec3::operator==(const Vec3 &v) const { return (equals(v)); }

void Vec3::print(std::ostream &out) const {
  out << "(" << x() << "," << y() << "," << z() << ")";
}

Vec3 euler(const Vec3 &v, const Vec3 &dv, double dt) { return v + (dv * dt); }

Vec3 rk4(const Vec3 &v, const Vec3 &dv, const Vec3 &ddv, double dt) {
  Vec3 k1 = dv;
  Vec3 k2 = euler(k1, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec3 k3 = euler(k2, dv + (0.5 * dt * ddv), 0.5 * dt);
  Vec3 k4 = euler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 2 * k2 + 2 * k3 + k4) / 6));
}

Vec3 rk4_38(const Vec3 &v, const Vec3 &dv, const Vec3 &ddv, double dt) {
  Vec3 k1 = dv;
  Vec3 k2 = 0.333 * euler(k1, dv + (0.333 * dt * ddv), 0.333 * dt);
  Vec3 k3 = -0.333 * euler(k1, dv + (0.667 * dt * ddv), 0.667 * dt) +
            euler(k2, dv + (0.667 * dt * ddv), 0.667 * dt);
  Vec3 k4 = euler(k1, dv + (dt * ddv), dt) - euler(k2, dv + (dt * ddv), dt) +
            euler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 3 * k2 + 3 * k3 + k4) / 8));
}
