#include <iostream>
#include <iomanip>
#include <cmath>

#include "2DVec.h"

using namespace std;

const double pi = 3.14159265;

Vec::Vec(): magnitude (0), direction (0) {}

Vec::Vec(double M, double D, bool componentForm){
    if (componentForm){
        magnitude = sqrt(pow(M, 2) + pow(D, 2));
        direction = atan2(D, M);
    }
    else {
        magnitude = M;
        direction = D;
    }
}

Vec::Vec(const Vec& vec){
    magnitude = vec.magnitude;
    direction = vec.direction;
}

Vec& Vec::operator=(const Vec& vec) {
    if (this != &vec){
        magnitude = vec.magnitude;
        direction = vec.direction;
    }
    return *this;
}

Vec::~Vec() {}

double Vec::x() const {
    return magnitude * cos(direction);
}

double Vec::y() const {
    return magnitude * sin(direction);
}

Vec Vec::unit() const{
    return Vec(x() / magnitude, y() / magnitude);
}


Vec Vec::plus(const Vec& vec) const {
    return Vec(x() + vec.x(), y() + vec.y());
}
Vec Vec::operator+(const Vec& vec) const {return plus(vec);}

Vec Vec::minus(const Vec& vec) const {
    return Vec(x() - vec.x(), y() - vec.y());
}
Vec Vec::operator-(const Vec& vec) const {return minus(vec);}

Vec Vec::scalarMultiple(double m) const {
    return Vec(x() * m, y() * m);
}
Vec Vec::operator*(double m) const {return scalarMultiple(m);}
Vec operator* (double m , const Vec& vec) {return vec * m;}

Vec Vec::operator/(double m) const {
    return Vec(x() / m, y() / m);
}

Vec Vec::dot(const Vec& vec) const {
    return Vec(x() * vec.x(), y() * vec.y());
}

double Vec::cross(const Vec& vec) const {
    return (x() * vec.y() - y() * vec.x());
}

bool Vec::equals(const Vec& vec) const {
	return (magnitude == vec.magnitude && direction == vec.direction);
}
bool Vec::operator==(const Vec& vec) const {
	return (equals(vec));
}

void Vec::print(std::ostream& out, bool componentForm = true) const {
    if (componentForm){
        out << "<" << x() << ", " << y() << ">";
    }
    else {
        out << "(" << magnitude << ", " << direction << ")" << endl;
    }
}

void Vec::debugPrint(std::ostream& out) const {
    out << magnitude << ' ' << direction << ' ' << x() << ' ' << y();
}

Vec euler(const Vec& v, const Vec& dv, double t){
	return v + (dv * t);
}

Vec rk4(const Vec& v, const Vec& dv, const Vec& ddv, double t){
	Vec k1 = dv;
	Vec k2 = euler(k1, dv + (0.5 * t * ddv), 0.5 * t);
	Vec k3 = euler(k2, dv + (0.5 * t * ddv), 0.5 * t);
	Vec k4 = euler(k3, dv + (t * ddv), t);
	
	return v + (t * ((k1 + 2 * k2 + 2 * k3 + k4) / 6));
}

/*
int main(){

    Vec vec1;
    Vec vec2(2, pi, false);
    Vec vec3(3, 4, true);
    Vec vec4(7, 24);

    cerr << "vec1 is " << vec1 << ", or "; vec1.print(cerr, false);
    cerr << "vec2 is " << vec2 << ", or "; vec2.print(cerr, false);
    cerr << "vec3 is " << vec3 << ", or "; vec3.print(cerr, false);
    cerr << "vec4 is " << vec4 << ", or "; vec4.print(cerr, false);

    Vec vec5 = vec3;

    cerr << "\nvec5 is " << vec5 << ", or "; vec5.print(cerr, false);

    Vec vec6 = vec3 + vec4;

    cerr << "vec6 is " << vec6 << ", or "; vec6.print(cerr, false);

    Vec vec7 = vec6 - (vec3 * 2);

    cerr << "vec7 is " << vec7 << ", or "; vec7.print(cerr, false);

    vec7 = vec6 - (2 * vec3);

    cerr << "vec7 is " << vec7 << ", or "; vec7.print(cerr, false);

    vec1 = Vec(0, 2);

    cerr << "vec1 is now " << vec1 << ", or "; vec1.print(cerr, false);
    cerr << "vec1 dot vec2 is " << vec1.dot(vec2) << endl;
    cerr << "vec2 dot vec1 is " << vec2.dot(vec1) << endl;

    cerr << "vec3 cross vec4 is " << vec3.cross(vec4) << endl;
    cerr << "vec4 cross vec3 is " << vec4.cross(vec3) << endl;

}
*/
