#include "bbox.h"

BBox::BBox() : _h (0), _w (0) {}
BBox::BBox(const Vec& p, double h, double w) : _pos (p), _h (h), _w (w) {}
BBox::~BBox() {}