#include "quaternion.h"

#include <cmath>
#include <cstddef>

const double EQ_TOLERANCE = 1e-10;

Quaternion::Quaternion() : _w(1), _x(0), _y(0), _z(0) {}

Quaternion::Quaternion(const Vec3 &axis, double angle) {
  Vec3 norm = axis.unit();
  _w = cos(0.5 * angle);
  _x = norm.x() * sin(0.5 * angle);
  _y = norm.y() * sin(0.5 * angle);
  _z = norm.z() * sin(0.5 * angle);
}

Quaternion::Quaternion(double w, double x, double y, double z)
    : _w(w), _x(x), _y(y), _z(z) {}

Quaternion::Quaternion(const Quaternion &q) {
  _w = q._w;
  _x = q._x;
  _y = q._y;
  _z = q._z;
}

Quaternion &Quaternion::operator=(const Quaternion &q) {
  if (this != &q) {
    _w = q._w;
    _x = q._x;
    _y = q._y;
    _z = q._z;
  }
  return *this;
}

Quaternion::~Quaternion() {}

Quaternion Quaternion::inverse() const { return Quaternion(_w, -_x, -_y, -_z); }

Quaternion Quaternion::multiply(const Quaternion &q) const {
  // Vec3 vp(_x, _y, _z), vq(q._x, q._y, q._z);
  // double resultW = (_w * q._w) - vp.dot(vq);
  // Vec3 resultXYZ = (_w * vq) + (q._w * vp) + vp.cross(vq);
  // return Quaternion(resultW, resultXYZ.x(), resultXYZ.y(), resultXYZ.z());
  return Quaternion((_w * q._w) - (_x * q._x) - (_y * q._y) - (_z * q._z),
                    (_w * q._x) + (_x * q._w) - (_y * q._z) + (_z * q._y),
                    (_w * q._y) + (_x * q._z) + (_y * q._w) - (_z * q._x),
                    (_w * q._z) - (_x * q._y) + (_y * q._x) + (_z * q._w));
}

Quaternion Quaternion::operator*(const Quaternion &q) const {
  return multiply(q);
}

Quaternion Quaternion::conjugate(const Quaternion &q) const {
  return q * *this * q.inverse();
}

void Quaternion::toAxisAngle(Vec3 &axis, double &angle) const {
  double quotient = sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2));
  axis = quotient < EQ_TOLERANCE ? Vec3(1, 0, 0): Vec3(_x, _y, _z) / quotient;
  angle = 2.0 * atan2(quotient, _w);
}

Quaternion euler(const Quaternion &q, const Quaternion &dq, double dt) {
  Vec3 dqAxis;
  double dqAngle;
  dq.toAxisAngle(dqAxis, dqAngle);
  Quaternion dqScaled(dqAxis, dqAngle);
  dqAngle *= dt;
  Vec3 v; double d;
  dqScaled.toAxisAngle(v, d);
  // std::cerr << q << "    " << q.conjugate(dqScaled) << "\n";

  return q.conjugate(dqScaled);
}

void Quaternion::print(std::ostream &out) const {
  out << "(" << w() << "," << x() << "," << y() << "," << z() << ")";
}