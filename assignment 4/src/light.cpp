#include "light.h"
#include "utils.h"

#include <iostream>
#include <stb_image.h>

constexpr uint32_t SAMPLE_NUM = 16;

Light::Light(const Vec3f &pos, const Vec3f &color) :
    position(pos), radiance(color) {}

SquareAreaLight::SquareAreaLight(const Vec3f &pos, const Vec3f &color, const Vec2f &size) :
    Light(pos, color), size(size) {
  Vec3f v1, v2, v3, v4;
  v1 = pos + Vec3f(size.x() / 2, 0.f, -size.y() / 2);
  v2 = pos + Vec3f(-size.x() / 2, 0.f, -size.y() / 2);
  v3 = pos + Vec3f(-size.x() / 2, 0.f, size.y() / 2);
  v4 = pos + Vec3f(size.x() / 2, 0.f, size.y() / 2);
  light_mesh =TriangleMesh({v1, v2, v3, v4}, {Vec3f(0, -1, 0)}, {0, 1, 2, 0, 2, 3}, {0,0,0,0,0,0});
} 

Vec3f SquareAreaLight::emission(const Vec3f &pos, const Vec3f &dir, const Interaction &interaction) const {
  Vec3f normal(0.0f, -1.0f, 0.0f);
  float cosine = normal.dot(dir);
  
  cosine = std::max(normal.dot(dir), 0.0f);
  return cosine * this->radiance;
}

float SquareAreaLight::pdf(const Interaction &interaction, Vec3f pos) {
  // normal always facing (0,-1,0)
  Vec3f normal(0.0f, -1.0f, 0.0f);
  // float cosine = fabs(normal.dot(interaction.wi));
  float cosine = normal.dot(-interaction.wi);
  float dis = (interaction.pos - pos).norm();
  // return std::max(0.0f, cosine) / (dis * dis);
  return 1.0f / (dis * dis);
}

Vec3f SquareAreaLight::sample(Interaction &interaction, float *pdf, Sampler &sampler) const {
  Vec2f samples = sampler.get2D();
  samples = samples * 2.0f - Vec2f(1.0f, 1.0f);
  Vec3f x_tangent(1.0f, 0.0f, 0.0f);
  Vec3f y_tangent(0.0f, 0.0f, 1.0f);

  Vec3f pos = this->position
                   + samples.x() * this->size.x() / 2.0f * x_tangent
                   + samples.y() * this->size.y() / 2.0f * y_tangent;
  interaction.wi = pos - interaction.pos;
  interaction.wi.normalize();

  *pdf = 1.0f / (this->size.x() * this->size.y());
  // std::cout<<(*pdf)<<'\n';
  // std::cout<<this->size.x()<<' '<<this->size.y()<<'\n';
  return pos;
}

bool SquareAreaLight::intersect(Ray &ray, Interaction &interaction) const {
  if(this->size.x()<1e-3f)
    return false;
  if (light_mesh.intersect(ray, interaction)) {
    interaction.type = Interaction::Type::LIGHT;
    // puts("LIGHT!!!!!");
    return true;
  }
  return false;
}

Environment::Environment(const Vec3f &m_pos,
                         const Vec2f &m_size,
                         const Vec3f &m_tangent_x,
                         const Vec3f &m_tangent_y,
                         const Vec3f &m_normal,
                         const std::string &path,
                         const bool &back) :
    size(m_size), tangent_x(m_tangent_x), tangent_y(m_tangent_y), normal(m_normal), texture(path), is_back(back) {position = m_pos;}

Vec3f Environment::emission(const Vec3f &pos, const Vec3f &dir, const Interaction &interaction) const {
  Vec3f normal(0.0f, -1.0f, 0.0f);
  float cosine = normal.dot(dir);
  
  cosine = std::max(normal.dot(dir), 0.0f);
  return cosine * interaction.texture_rgb;
}

float Environment::pdf(const Interaction &interaction, Vec3f pos) {
  // normal always facing (0,-1,0)
  Vec3f normal(0.0f, -1.0f, 0.0f);
  // float cosine = fabs(normal.dot(interaction.wi));
  float cosine = normal.dot(interaction.wi);
  float dis = (interaction.pos - pos).norm();
  return abs(cosine) / (dis * dis);
}

Vec3f Environment::sample(Interaction &interaction, float *pdf, Sampler &sampler) const {
  Vec2f samples = sampler.get2D();
  samples = samples * 2.0f - Vec2f(1.0f, 1.0f);
  Vec3f x_tangent(1.0f, 0.0f, 0.0f);
  Vec3f y_tangent(0.0f, 0.0f, 1.0f);

  Vec3f pos = this->position
                   + samples.x() * this->size.x() / 2.0f * x_tangent
                   + samples.y() * this->size.y() / 2.0f * y_tangent;
  interaction.wi = position - interaction.pos;
  interaction.wi.normalize();

  *pdf = 1.0f / (this->size.x() * this->size.y());
  // std::cout<<(*pdf)<<'\n';
  // std::cout<<this->size.x()<<' '<<this->size.y()<<'\n';
  return pos;
}

bool Environment::intersect(Ray &ray, Interaction &interaction) const {
  // std::cout<<this->position<<'\n';
  if(this->is_back == true)
  {
    if(ray.direction.dot(this->normal) > 0.0f)
      return false;
  }
  Vec3f d = ray.direction;
  Vec3f o = ray.origin;
  if (fabs(d.dot(this->normal)) < 1e-7f)
    return false;
  float t = (this->position - o).dot(this->normal) / d.dot(this->normal);
  // 
  // std::cout<<this->position<<'\n';
  // std::cout<<(this->position-o)<<'\n';
  
  // std::cout<<(this->position - o).dot(this->normal)<<'\n';
  // std::cout<<d.dot(this->normal)<<'\n';
  // std::cout<<d<<'\n';
  // std::cout<<t<<'\n';
  // printf("%.4f\n",t);
  // std::cout<<"o   "<<o<<"\n\npos     "<<pos<<
  // std::cout<<ray(t)<<'\n';

  if (t < ray.t_min || t > ray.t_max)
    return false;

  if (d.norm() * t > interaction.dist)
    return false;

  Vec3f pos = ray(t);

  Vec3f vec = pos - this->position;
  // puts("true");
  // std::cout<<fabs(vec.dot(this->tangent_x))<<'\n';
  // std::cout<<this->tangent_x<<'\n';
  // std::cout<<t<<'\n'<<position<<'\n';

  if(fabs(vec.dot(this->tangent_x)) > (size.x() / 2.0f) + 1e-7)
    return false;

  // puts("judging");

  if(fabs(vec.dot(tangent_y)) > (size.y() / 2.0f) + 1e-7)
    return false;

  interaction.type = Interaction::Type::ENVIRONMENT;

  // if(this->is_back)
  //  puts("intersected!");

  interaction.pos = o + d * t;
  interaction.normal = this->normal;
  interaction.dist = d.norm() * t;
  // std::cout<<interaction.dist<<'\n';

  float u = 0.0f, v = 0.0f;
  u = vec.dot(this->tangent_x) / size.x() + 0.5f;
  if(u < 0.0f)
    u = 0.0f;
  v = vec.dot(tangent_y) / size.y() + 0.5f;
  if(v < 0.0f)
    v = 0.0f;

  interaction.texture_rgb = this->texture.getPixel(u, v) / 255.0;
  // std::cout<< interaction.texture_rgb<<'\n';
  return true;
}