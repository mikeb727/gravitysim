#include "bbox.h"

BBox::BBox() : _h(0), _w(0) {}
BBox::BBox(const Vec2 &pos, double width, double height, double angle)
    : _pos(pos), _w(width), _h(height), _angle(angle) {}
BBox::~BBox() {}