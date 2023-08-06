/* A vector in the two-dimensional plane. */

#ifndef VEC2D_H
#define VEC2D_H

#include <iostream>

// Component: specify x and y components
// MagDir: specify magnitude and direction (radians)
enum VecMode { Component, MagDir };

class Vec2 {
private:
  double _x;
  double _y;

public:
  // ctor, dtor
  Vec2(); // zero vector
  Vec2(double x, double y, VecMode mode = Component);
  Vec2(const Vec2 &v);
  Vec2 &operator=(const Vec2 &v);
  ~Vec2();

  // getters
  double x() const { return _x; };
  double y() const { return _y; };
  double mag() const;
  double dir() const;
  Vec2 unit() const; // unit vector in same direction

  // math operations
  Vec2 plus(const Vec2 &v) const;
  Vec2 operator+(const Vec2 &v) const;
  Vec2 minus(const Vec2 &v) const;
  Vec2 operator-(const Vec2 &v) const;
  Vec2 scalarMultiple(double k) const;
  Vec2 operator*(double k) const;
  Vec2 operator/(double k) const;
  Vec2 operator-() const;
  double dot(const Vec2 &v) const;
  double cross(const Vec2 &v) const; // scalar in two-dimensional case
  bool equals(const Vec2 &v) const;
  bool operator==(const Vec2 &v) const;

  // numerical methods
  friend Vec2 euler(const Vec2 &v, const Vec2 &dv, double dt);
  friend Vec2 rk4(const Vec2 &v, const Vec2 &dv, const Vec2 &ddv, double dt);

  // debug
  void print(std::ostream &, bool) const;
  void debugPrint(std::ostream &) const;
};

Vec2 operator*(double k, const Vec2 &v); // commutative scalar multiplication

inline std::ostream &operator<<(std::ostream &out, const Vec2 vec) {
  vec.print(out, true);
  return out;
}

#endif
