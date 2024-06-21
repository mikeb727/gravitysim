#ifndef UTILS_H
#define UTILS_H

double computeTNow();

// scalar rk4 implementation
double euler(const double &v, const double &dv, double dt);
double rk4(const double &v, const double &dv, const double &ddv, double dt);

#endif