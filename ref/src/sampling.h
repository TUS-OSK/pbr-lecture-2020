#ifndef _SAMPLING_H
#define _SAMPLING_H
#include <algorithm>
#include <cmath>

#include "constant.h"
#include "vec3.h"

// 接空間での半球面一様サンプリング
inline Vec3f sampleHemisphere(float u, float v, float& pdf) {
  const float theta = std::acos(std::max(1.0f - u, 0.0f));
  const float phi = 2.0f * PI * v;
  pdf = PI_MUL_2_INV;
  return Vec3f(std::cos(phi) * std::sin(theta), std::cos(theta),
               std::sin(phi) * std::sin(theta));
}

// 接空間での半球コサイン比例サンプリング
inline Vec3f sampleCosineHemisphere(float u, float v, float& pdf) {
  const float theta =
      0.5f * std::acos(std::clamp(1.0f - 2.0f * u, -1.0f, 1.0f));
  const float phi = 2.0f * PI * v;

  const float cosTheta = std::cos(theta);
  pdf = PI_INV * cosTheta;

  return Vec3f(std::cos(phi) * std::sin(theta), cosTheta,
               std::sin(phi) * std::sin(theta));
}

#endif