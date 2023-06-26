#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "core.h"
#include "ray.h"
#include "interaction.h"
#include "bsdf.h"
#include "accel.h"

#include <vector>
#include <optional>

class TriangleMesh {
 public:
  TriangleMesh() = default;
  TriangleMesh(std::vector<Vec3f> vertices,
               std::vector<Vec3f> normals,
               std::vector<int> v_index,
               std::vector<int> n_index);
  bool intersect(Ray &ray, Interaction &interaction) const;
  void setMaterial(std::shared_ptr<BSDF> &new_bsdf);
  void buildBVH();
  void build(BVHNode* root, BVHNode* bvh);

//  private:
  bool intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i& v_idx, const Vec3i& n_idx) const;
  void bvhHit(BVHNode *p, Interaction &interaction,
              Ray &ray) const;
  std::shared_ptr<BSDF> bsdf;
  BVHNode* bvh;

  std::vector<Vec3f> vertices;
  std::vector<Vec3f> normals;
  std::vector<int> v_indices;
  std::vector<int> n_indices;
};

#endif // GEOMETRY_H_
