#ifndef UTILS_H
#define UTILS_H

#include "2DPhysEnv.h"
#include "2DVec.h"
#include "control.h"
#include "cursor.h"
#include <mb-libs/mbgfx.h>

namespace utils {

typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

Vec2 remapGlfwCursor(Vec2 vec, GraphicsTools::Window *win);

double computeTNow();

// scalar rk4 implementation
double scalarEuler(const double &v, const double &dv, double dt);
double scalarRk4(const double &v, const double &dv, const double &ddv, double dt);

// common object creation
void createObj(GraphicsTools::Window *win);

} // namespace utils

#endif