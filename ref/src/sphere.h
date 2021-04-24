#ifndef _SPHERE_H
#define _SPHERE_H
#include "intersect-info.h"
#include "ray.h"
#include "vec3.h"

enum class MaterialType { Diffuse, Mirror, Glass };

class Sphere {
 public:
  Vec3f center;                // 中心位置
  float radius;                // 半径
  Vec3f kd;                    // 色
  MaterialType material_type;  // 材質

  Sphere(const Vec3f& center, float radius, const Vec3f& kd,
         const MaterialType& material_type)
      : center(center), radius(radius), kd(kd), material_type(material_type) {}

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    const float b = dot(ray.direction, ray.origin - center);
    const float c = length2(ray.origin - center) - radius * radius;
    const float D = b * b - c;
    // 判別式
    if (D < 0) return false;

    // 解の公式で解の候補を計算
    const float t0 = -b - std::sqrt(D);
    const float t1 = -b + std::sqrt(D);

    // レイの始点から近い方の解を計算
    float t = t0;
    if (t < ray.tmin || t > ray.tmax) {
      t = t1;

      // レイが許容交差距離内に無かった場合
      if (t < ray.tmin || t > ray.tmax) {
        return false;
      }
    }

    // 交差情報を計算
    info.t = t;
    info.hitPos = ray(t);
    info.hitNormal = normalize(info.hitPos - center);
    info.hitSphere = this;

    return true;
  }
};

#endif