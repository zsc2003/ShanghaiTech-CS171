#ifndef CS171_HW3_INCLUDE_CORE_H_
#define CS171_HW3_INCLUDE_CORE_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <memory>

#define UNIMPLEMENTED

using Vec3f = Eigen::Vector3f;
using Vec3i = Eigen::Vector3i;
using Vec2f = Eigen::Vector2f;
using Vec2i = Eigen::Vector2i;
using Vec4f = Eigen::Vector4f;


constexpr float RAY_DEFAULT_MIN = 1e-5;
constexpr float RAY_DEFAULT_MAX = 1e7;
constexpr float PI = 3.141592653579f;
constexpr float EPS = 1e-3;

#endif //CS171_HW3_INCLUDE_CORE_H_
