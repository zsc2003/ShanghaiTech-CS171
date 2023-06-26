#ifndef SCENE_H_
#define SCENE_H_

#include <vector>

#include "camera.h"
#include "image.h"
#include "geometry.h"
#include "light.h"
#include "interaction.h"
#include "config.h"

class Scene {
 public:
  Scene() = default;
  void addObject(std::shared_ptr<TriangleMesh> &geometry);
  [[nodiscard]] const std::shared_ptr<Light> &getLight() const;
  void setLight(const std::shared_ptr<Light> &new_light);
  bool isShadowed(Ray &shadow_ray);
  bool intersect(Ray &ray, Interaction &interaction);

  std::vector<std::shared_ptr<Light>> environment;

  bool is_bonus4{false};
  BVHNode* bvh;
  std::vector<Vec3f> vertices;
  std::vector<Vec3f> normals;
  void buildbvh();
  void build(BVHNode* root, BVHNode* bvh);
  bool intersectOneTriangle(Ray &ray, Interaction &interaction, const Vec3i& v_idx, const Vec3i& n_idx) const;
  void bvhHit(BVHNode *p, Interaction &interaction,Ray &ray) const;
  
 private:
  std::vector<std::shared_ptr<TriangleMesh>> objects;
  std::shared_ptr<Light> light;
};

void initSceneFromConfig(const Config &config, std::shared_ptr<Scene> &scene);

#endif //SCENE_H_
