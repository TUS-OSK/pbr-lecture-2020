#ifndef _RAY_H
#define _RAY_H
#include "vec3.h"

struct Ray {
  Vec3f origin;                         // 始点
  Vec3f direction;                      // 方向
  static constexpr float tmin = 1e-3f;  // 許容最小交差距離
  static constexpr float tmax = 10000;  // 許容最大交差距離

  Ray(const Vec3f& origin, const Vec3f& direction)
      : origin(origin), direction(direction) {}

  // 始点から距離tの点を返す
  Vec3f operator()(float t) const { return origin + t * direction; }
};

#endif