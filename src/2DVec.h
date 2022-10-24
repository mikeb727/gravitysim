/* A vector in the two-dimensional plane. */

#ifndef VEC2D_H
#define VEC2D_H

#include <iostream>

class Vec {
private:
    double magnitude;
    double direction;
public:
    /* Creates a vector with magnitude and direction zero. */
    Vec();
    /* Creates a vector with the specified magnitude and direction,
        or specified components, if the final parameter is true. */
    Vec(double, double, bool = true);
    /* copy constructor */
    Vec(const Vec&);
    Vec& operator=(const Vec&);
    /* Destroys the vector. */
    ~Vec();
    /* Returns the horizontal component of the vector. */
    double x() const;
    /* Returns the vertical component of the vector. */
    double y() const;
    /* Returns the magnitude of the vector. */
    double mag() const {return magnitude;};
    /* Returns the angle, in radians, between the positive
        horizontal "axis" and the vector. */
    double dir() const {return direction;};
    /* Returns the vector's unit vector. */
    Vec unit() const;
    /* Returns the sum of the vector and the other one
        specified. */
    Vec plus(const Vec&) const;
    Vec operator+(const Vec&) const;
    /* Returns the difference between the vector and the
        other one specified. */
    Vec minus(const Vec&) const;
    Vec operator-(const Vec&) const;
    /* Returns the product of the vector and a real
        number. */
    Vec scalarMultiple(double) const;
    Vec operator*(double) const;
    Vec operator/(double) const;
    /* Returns the dot product of the vector and the
        other one specified. */
    Vec dot(const Vec&) const;
    /* Returns the magnitude cross product of the vector and
        the other one specified (not a vector, since all
        cross products of vectors in the 2D plane will point
        in the same direction). */
    double cross(const Vec&) const;
    /* Prints the vector as (magnitude, direction), or as <x, y>,
        if the final parameter is true, to the
        output location specified. */
    void print(std::ostream&, bool) const;
    /* Prints the magnitude, direction, and components
        of the vector, all separated by spaces. */
    void debugPrint(std::ostream&) const;
    /* Returns whether the vectors are equal. */
    bool equals(const Vec&) const;
    bool operator==(const Vec&) const;
    /* Returns the result of Euler integration with the given vector, derivative, and time step. */
    friend Vec euler(const Vec&, const Vec&, double);
    friend Vec rk4(const Vec&, const Vec&, const Vec&, double);
};

Vec operator* (double, const Vec&);

inline std::ostream& operator<< (std::ostream& out, const Vec vec){
    vec.print(out, true); return out;
}

#endif

