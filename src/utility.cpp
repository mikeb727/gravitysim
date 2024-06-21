#include "utility.h"

#include <chrono>

double computeTNow() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

double euler(const double &v, const double &dv, double dt) { return v + (dv * dt); }

double rk4(const double &v, const double &dv, const double &ddv, double dt) {
  double k1 = dv;
  double k2 = euler(k1, dv + (0.5 * dt * ddv), 0.5 * dt);
  double k3 = euler(k2, dv + (0.5 * dt * ddv), 0.5 * dt);
  double k4 = euler(k3, dv + (dt * ddv), dt);

  return v + (dt * ((k1 + 2 * k2 + 2 * k3 + k4) / 6));
}