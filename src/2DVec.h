/* A vector in the two-dimensional plane. */

#ifndef VEC2D_H
#define VEC2D_H

#include <iostream>

// Component: specify x and y components
// MagDir: specify magnitude and direction (radians)
enum VecMode { Component, MagDir };

class Vec {
private:
  double _x;
  double _y;

public:
  // ctor, dtor
  Vec(); // zero vector
  Vec(double x, double y, VecMode mode = Component);
  Vec(const Vec &v);
  Vec &operator=(const Vec &v);
  ~Vec();

  // getters
  double x() const { return _x; };
  double y() const { return _y; };
  double mag() const;
  double dir() const;
  Vec unit() const; // unit vector in same direction

  // math operations
  Vec plus(const Vec &v) const;
  Vec operator+(const Vec &v) const;
  Vec minus(const Vec &v) const;
  Vec operator-(const Vec &v) const;
  Vec scalarMultiple(double k) const;
  Vec operator*(double k) const;
  Vec operator/(double k) const;
  Vec operator-() const;
  double dot(const Vec &v) const;
  double cross(const Vec &v) const; // scalar in two-dimensional case
  bool equals(const Vec &v) const;
  bool operator==(const Vec &v) const;

  // numerical methods
  friend Vec euler(const Vec &v, const Vec &dv, double dt);
  friend Vec rk4(const Vec &v, const Vec &dv, const Vec &ddv, double dt);

  // debug
  void print(std::ostream &, bool) const;
  void debugPrint(std::ostream &) const;
};

Vec operator*(double k, const Vec &v); // commutative scalar multiplication

inline std::ostream &operator<<(std::ostream &out, const Vec vec) {
  vec.print(out, true);
  return out;
}

#endif
