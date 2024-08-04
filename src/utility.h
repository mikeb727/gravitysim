#ifndef UTILS_H
#define UTILS_H

#include "control.h"
#include "cursor.h"
#include "env3d.h"
#include "vec3d.h"

namespace utils {

typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

Vec3 remapGlfwCursor(Vec3 vec, GraphicsTools::Window *win);

double computeTNow();

// scalar rk4 implementation
double scalarEuler(const double &v, const double &dv, double dt);
double scalarRk4(const double &v, const double &dv, const double &ddv,
                 double dt);

// common object creation
void createObj(GraphicsTools::Window *win);

} // namespace utils

#endif