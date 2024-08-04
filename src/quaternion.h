#ifndef QUATERNION_H
#define QUATERNION_H

#include "vec3d.h"

class Quaternion {
public:
  // ctor, dtor
  Quaternion(); // identity quaternion
  Quaternion(const Vec3 &axis, double angle);
  Quaternion(double w, double x, double y, double z);
  Quaternion(const Quaternion &q);
  Quaternion &operator=(const Quaternion &q);
  ~Quaternion();

  // getters
  double w() const {return _w;};
  double x() const {return _x;};
  double y() const {return _y;};
  double z() const {return _z;};

  // math operations
  Quaternion inverse() const;
  Quaternion multiply(const Quaternion &q) const;
  Quaternion operator*(const Quaternion &q) const;
  Quaternion conjugate(const Quaternion &q) const;
  void toAxisAngle(Vec3 &axis, double &angle) const;
  
  friend Quaternion euler(const Quaternion &q, const Quaternion &dq, double dt);

    void print(std::ostream &out) const;

private:
  double _w;
  double _x;
  double _y;
  double _z;
};

inline std::ostream &operator<<(std::ostream &out, const Quaternion q) {
  q.print(out);
  return out;
}

#endif