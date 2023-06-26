#ifndef ACCEL_H_
#define ACCEL_H_

#include "core.h"
#include "ray.h"


struct AABB {
  // the minimum and maximum coordinate for the AABB
  Vec3f low_bnd;
  Vec3f upper_bnd;
  /// test intersection with given ray.
  /// ray distance of entrance and exit point are recorded in t_in and t_out
  AABB() : low_bnd(0, 0, 0), upper_bnd(0, 0, 0) {}
  AABB(Vec3f low, Vec3f upper) : low_bnd(low), upper_bnd(upper) {}
  /// construct an AABB from three vertices of a triangle.
  AABB(const Vec3f &v1, const Vec3f &v2, const Vec3f &v3);
  /// Construct AABB by merging two AABBs
  AABB(const AABB &a, const AABB &b);
  bool intersect(const Ray &ray, float *t_in, float *t_out);
  /// test if a point is in the aabb 
  bool intersectPoint(Vec3f point);
  /// Get the AABB center
  [[nodiscard]] Vec3f getCenter() const { return (low_bnd + upper_bnd) / 2; }
  /// Get the length of a specified side on the AABB
  [[nodiscard]] float getDist(int dim) const { return upper_bnd[dim] - low_bnd[dim]; }
  /// Check whether the AABB is overlapping with another AABB
  [[nodiscard]] bool isOverlap(const AABB &other) const;
};

// You may need to add your code for BVH construction here.
struct BVHNode {
  // BVHNode *child[8];
  BVHNode *left, *right;

  // bounding box of current node.
  AABB aabb;
  // index of voxel in current BVH leaf node.
  std::vector<Vec3i> voxels;
  int x1, x2;
  int y1, y2;
  int z1, z2;
  int l, r;

  
  BVHNode() : left(nullptr), right(nullptr), aabb() {}
  void delete_node();
  ~BVHNode() { delete_node(); }
};

#endif //ACCEL_H_