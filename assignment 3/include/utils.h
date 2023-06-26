#ifndef CS171_HW3_INCLUDE_UTILS_H_
#define CS171_HW3_INCLUDE_UTILS_H_

#include "core.h"

namespace utils {

static inline float clamp01(float v) {
  if (v > 1) v = 1;
  else if (v < 0) v = 0;
  return v;
}

static inline uint8_t gammaCorrection(float radiance) {
  return static_cast<uint8_t>(ceil(255 * pow(clamp01(radiance), 1 / 2.2)));
}

}

#endif //CS171_HW3_INCLUDE_UTILS_H_
