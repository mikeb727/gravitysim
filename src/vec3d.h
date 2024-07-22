/* A vector in three-dimensional space. */

#ifndef VEC3D_H
#define VEC3D_H

#include <iostream>

class Vec3 {
private:
  double _x;
  double _y;
  double _z;

public:
  // ctor, dtor
  Vec3(); // zero vector
  Vec3(double x, double y); // zero z-component
  Vec3(double x, double y, double z);
  Vec3(Vec3 &dir, double mag);
  Vec3(const Vec3 &v);
  Vec3 &operator=(const Vec3 &v);
  ~Vec3();

  // getters
  double x() const { return _x; };
  double y() const { return _y; };
  double z() const { return _z; };
  double mag() const;
  // double dir() const;
  // "direction" is not as important in 3D; lat/lon would be closest, but is of
  // less use
  Vec3 unit() const; // unit vector in same direction

  // math operations
  Vec3 plus(const Vec3 &v) const;
  Vec3 operator+(const Vec3 &v) const;
  Vec3 &operator+=(const Vec3 &v);
  Vec3 minus(const Vec3 &v) const;
  Vec3 operator-(const Vec3 &v) const;
  Vec3 scalarMultiple(double k) const;
  Vec3 operator*(double k) const;
  Vec3 operator/(double k) const;
  Vec3 operator-() const;
  double dot(const Vec3 &v) const;
  Vec3 cross(const Vec3 &v) const;
  bool equals(const Vec3 &v) const;
  bool operator==(const Vec3 &v) const;

  // numerical methods
  friend Vec3 euler(const Vec3 &v, const Vec3 &dv, double dt);
  friend Vec3 rk4(const Vec3 &v, const Vec3 &dv, const Vec3 &ddv, double dt);
  friend Vec3 rk4_38(const Vec3 &v, const Vec3 &dv, const Vec3 &ddv,
                     double dt); // lower-error 3/8 rule?

  // debug
  void print(std::ostream &out) const;
};

Vec3 operator*(double k, const Vec3 &v); // commutative scalar multiplication

inline std::ostream &operator<<(std::ostream &out, const Vec3 vec) {
  vec.print(out);
  return out;
}

#endif
