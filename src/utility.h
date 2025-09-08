#ifndef simUtils_H
#define simUtils_H

#include "control.h"
#include "cursor.h"
#include "vec3d.h"
#include "env3d.h"

namespace simUtils {

// cursor tools for changing created ball size, speed, and spin angle/speed
enum class Tool { None = 0, SizeTool = 1, SpeedTool = 2, SpinTool = 3 };

// cursor data and visibility for now
struct UserCursor {
  CursorData data;
  CursorData prev;
  simUtils::Tool activeTool;
  // position of last on-screen right-click
  Vec3 lastRightClick;
  // difference between on-screen right-click pos and current pos (while dragging)
  Vec3 rightClickDragDelta;
  // approximate on-screen cursor velocity
  Vec3 cursorVel;
  int baseRenderObjId;
  int selectedObjId = -1;
  Vec3 objSelectionOffset;
};

typedef std::map<int, GraphicsTools::RenderObject> ObjMap;

Vec3 remapGlfwCursor(Vec3 vec, GraphicsTools::Window *win);

void drawUserCursor(GraphicsTools::Window *win, simUtils::UserCursor *uc);
void drawCursor(GraphicsTools::Window *win, CursorData cur, int curGfxId);
void populateSpinRingPoints(float speed, float radius, Vec3 origin, Vec3 axis, float *destArray,
                             int arraySize);

void drawSim(GraphicsTools::Window *win);
void setupEnvWalls(GraphicsTools::Window *win);
void setupUserCursors(GraphicsTools::Window *win, UserCursor *uc);

double computeTNow();

// scalar rk4 implementation
double scalarEuler(const double &v, const double &dv, double dt);
double scalarRk4(const double &v, const double &dv, const double &ddv,
                 double dt);

// common object creation
void createObj(GraphicsTools::Window *win);
// linked object deletion
void clearEnvObjs(GraphicsTools::Window *win);
void setupControls(ControlSet &ctrlSet);

int objIdAtEnvPos(Vec3 pos, Environment *env);
bool cubeOverlapAtEnvPos(Vec3 pos, Environment *env, float radius);

} // namespace simUtils

#endif