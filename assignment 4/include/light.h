#ifndef LIGHT_H_
#define LIGHT_H_

#include <vector>

#include "core.h"
#include "ray.h"
#include "geometry.h"
#include "texture.h"

class Light {
 public:
  Light() = default;
  explicit Light(const Vec3f &pos, const Vec3f &color);
  virtual ~Light() = default;
  [[nodiscard]] virtual Vec3f emission(const Vec3f &pos, const Vec3f &dir, const Interaction &interaction) const = 0;
  virtual float pdf(const Interaction &interaction, Vec3f pos) = 0;
  [[nodiscard]] virtual Vec3f sample(Interaction &interaction, float *pdf, Sampler &sampler) const = 0;
  virtual bool intersect(Ray &ray, Interaction &interaction) const = 0;

 //protected:
  /// position of light in world space
  Vec3f position;
  /// RGB color of the light
  Vec3f radiance;
};


// Square area light, consist of two right triangles, normal always facing (0,-1,0)
class SquareAreaLight : public Light {
 public:
  explicit SquareAreaLight(const Vec3f &pos,
                           const Vec3f &color,
                           const Vec2f &size);
  [[nodiscard]] Vec3f emission(const Vec3f &pos, const Vec3f &dir, const Interaction &interaction) const override;
  float pdf(const Interaction &interaction, Vec3f pos) override;
  [[nodiscard]] Vec3f sample(Interaction &interaction, float *pdf, Sampler &sampler) const override;
  bool intersect(Ray &ray, Interaction &interaction) const override;
//  protected:
  // build light mesh from position and size. position locates at the center of rectangle.
  TriangleMesh light_mesh;
  Vec2f size;
};

class Environment : public Light {
 public:
  explicit Environment(const Vec3f &m_pos,
                       const Vec2f &m_size,
                       const Vec3f &m_tangent_x,
                       const Vec3f &m_tangent_y,
                       const Vec3f &m_normal,
                       const std::string &path,
                       const bool &back);
  [[nodiscard]] Vec3f emission(const Vec3f &pos, const Vec3f &dir,const Interaction &interaction) const override;
  float pdf(const Interaction &interaction, Vec3f pos) override;
  [[nodiscard]] Vec3f sample(Interaction &interaction, float *pdf, Sampler &sampler) const override;
  bool intersect(Ray &ray, Interaction &interaction) const override;
 protected:
  // build light mesh from position and size. position locates at the center of rectangle.
  TriangleMesh light_mesh;
  Texture texture;
  Vec2f size;
  // Vec3f pos;
  Vec3f tangent_x, tangent_y, normal;
  bool is_back;
};

#endif //LIGHT_H_
