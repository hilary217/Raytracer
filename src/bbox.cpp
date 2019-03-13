#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

    // TODO (Part 2.2):
    // Implement ray - bounding box intersection test
    // If the ray intersected the bouding box within the range given by
    // t0, t1, update t0 and t1 with the new intersection times.


    double txmin, txmax, tymin, tymax, tzmin, tzmax;

    txmin = (min.x - r.o.x) / r.d.x;
    txmax = (max.x - r.o.x) / r.d.x;
    tymin = (min.y - r.o.y) / r.d.y;
    tymax = (max.y - r.o.y) / r.d.y;
    tzmin = (min.z - r.o.z) / r.d.z;
    tzmax = (max.z - r.o.z) / r.d.z;
    if (txmin > txmax)
        std::swap(txmin, txmax);
    if (tymin > tymax)
        std::swap(tymin, tymax);
    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);
    if(tymin > txmax || txmin > tymax){
        return false;
    }
    if(tymin > tzmax || tzmin > tymax){
        return false;
    }
    if(txmin > tzmax || tzmin > txmax){
        return false;
    }
    double tminmiddle = std::max(txmin, tymin);
    double tmaxmiddle = std::min(txmax, tymax);
    txmin = std::max(tminmiddle, tzmin);
    txmax = std::min(tmaxmiddle, tzmax);
    if (txmin <= txmax && txmax >= 0) {
        t0 = txmin;
        t1 = txmax;
        return true;
    } else {
        return false;
    }

}

void BBox::draw(Color c, float alpha) const {

  glColor4f(c.r, c.g, c.b, alpha);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
