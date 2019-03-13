#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c, alpha);
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c, alpha);
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  BBox centroid_box, bbox;

  for (Primitive *p : prims) {
      BBox bb = p->get_bbox();
      bbox.expand(bb);
      Vector3D c = bb.centroid();
      centroid_box.expand(c);
  }

  BVHNode *node = new BVHNode(bbox);

  if(prims.size () <= max_leaf_size){
      node->prims = new vector<Primitive *>(prims);
      return node;
  }
  int axis = 0;
  if (centroid_box.extent.x >= centroid_box.extent.y && centroid_box.extent.x >= centroid_box.extent.z) {
      axis = 0;
  }
  else if (centroid_box.extent.y >= centroid_box.extent.x && centroid_box.extent.y >= centroid_box.extent.z) {
      axis = 1;
  }else{
      axis = 2;
  }


  vector<Primitive *> pl = vector<Primitive *>();
  vector<Primitive *> pr = vector<Primitive *>();

  bool p1 = false;
  bool p2 = false;
  for (Primitive *p : prims) {
      if (p->get_bbox().centroid()[axis] <= centroid_box.centroid()[axis]) {
          pl.push_back(p);
          p1 = true;
      }else {
          pr.push_back(p);
          p2 = true;
      }
  }

  if(p1 && p2){
      node->l = construct_bvh(pl, max_leaf_size);
      node->r = construct_bvh(pr, max_leaf_size);
      return node;
  }
  else if(p1 && !p2){
      vector<Primitive *> l = vector<Primitive *>();
      vector<Primitive *> r = vector<Primitive *>();
      for (int i = 0; i <  prims.size() / 2; i++) {
          l.push_back(pl[i]);
      }
      for (int i =  prims.size() / 2; i < prims.size(); i++) {
          r.push_back(pl[i]);
      }
      node->l = construct_bvh(l, max_leaf_size);
      node->r = construct_bvh(r, max_leaf_size);
      return node;

  }
  else{
      vector<Primitive *> l = vector<Primitive *>();
      vector<Primitive *> r = vector<Primitive *>();
      for (int i = 0; i <  prims.size() / 2; i++) {
          l.push_back(pr[i]);
      }

      for (int i =  prims.size() / 2; i < prims.size(); i++) {
          r.push_back(pr[i]);
      }
      node->l = construct_bvh(l, max_leaf_size);
      node->r = construct_bvh(r, max_leaf_size);
      return node;
  }

}


bool BVHAccel::intersect(const Ray& ray, BVHNode *node) const {

  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.


    double t0, t1;
    if (!node->bb.intersect(ray, t0, t1)) {
        return false;
    }
    else {
        if (t1 < ray.min_t || t0 > ray.max_t) {
            return false;
        }
        else {
            if (node->isLeaf()) {
                for (Primitive *p : *(node->prims)) {
                    total_isects++;
                    if (p->intersect(ray)) {
                        return true;
                    }
                }
                return false;
            }
            else {
                return intersect(ray, node->l) || intersect(ray, node->r);
            }
        }
    }
}


bool BVHAccel::intersect(const Ray& ray, Intersection* i, BVHNode *node) const {

  // TODO (Part 2.3):
  // Fill in the intersect function.
    double t0, t1;
    if (!node->bb.intersect(ray, t0, t1)) {
        return false;
    }
    else {
        if (t1 < ray.min_t || t0 > ray.max_t) {
            return false;
        } else {
            if (node->isLeaf()) {
                bool hit = false;
                for (Primitive *p : *(node->prims)) {
                    total_isects++;
                    if (p->intersect(ray, i)) {
                        hit = true;
                    }
                }
                return hit;
            }
            else {
                bool left = false;
                bool right = false;
                left = left ||intersect(ray, i, node->l);
                right = right || intersect(ray, i, node->r);
                return left || right;
            }
        }
    }
}
}  // namespace StaticScene
}  // namespace CGL
