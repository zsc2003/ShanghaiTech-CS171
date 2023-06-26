#ifndef CS171_HW3_INCLUDE_INTERACTION_H_
#define CS171_HW3_INCLUDE_INTERACTION_H_

#include "core.h"

struct InteractionPhongLightingModel {
  Vec3f diffusion;
  Vec3f specular;
  Vec3f ambient;
  float shininess{};
};

struct Interaction {
  enum Type { LIGHT, GEOMETRY, NONE };
  Vec3f pos{};
  float dist{RAY_DEFAULT_MAX};
  Vec3f normal{};
  Vec2f uv{};
  Vec3f ray_direction;
  InteractionPhongLightingModel model;
  Type type{Type::NONE};
};

#endif //CS171_HW3_INCLUDE_INTERACTION_H_
