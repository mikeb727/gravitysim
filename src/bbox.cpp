#include "bbox.h"

BBox::BBox() : _h(0), _w(0) {}
BBox::BBox(const Vec &pos, double height, double width)
    : _pos(pos), _h(height), _w(width) {}
BBox::~BBox() {}