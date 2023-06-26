#ifndef INTERACTION_H_
#define INTERACTION_H_

#include "core.h"

struct Interaction {
  enum Type { LIGHT, GEOMETRY, NONE, ENVIRONMENT };
  Vec3f pos{0, 0, 0};
  float dist{RAY_DEFAULT_MAX};
  Vec3f normal{0, 0, 0};
  std::shared_ptr<BSDF> material{nullptr};
  Vec3f wi{0,0,0};
  Vec3f wo{0,0,0};
  Type type{Type::NONE};
  int is_inside{0};
  Vec3f texture_rgb;
};

#endif //INTERACTION_H_
