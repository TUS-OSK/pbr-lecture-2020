#ifndef _SPHERE_H
#define _SPHERE_H

#include "intersect-info.h"
#include "ray.h"
#include "vec3.h"

class Shape {
 public:
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

class Sphere : public Shape {
 public:
  Vec3f center;  // 中心位置
  float radius;  // 半径

  Sphere(const Vec3f& center, float radius) : center(center), radius(radius) {}

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

    return true;
  }
};

class Plane : public Shape {
 public:
  const Vec3f leftCornerPoint;
  const Vec3f right;
  const Vec3f up;

  Plane(const Vec3f& leftCornerPoint, const Vec3f& right, const Vec3f& up)
      : leftCornerPoint(leftCornerPoint), right(right), up(up) {}

  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    const Vec3f normal = normalize(cross(right, up));
    const Vec3f center = leftCornerPoint + 0.5f * right + 0.5f * up;
    const Vec3f rightDir = normalize(right);
    const float rightLength = length(right);
    const Vec3f upDir = normalize(up);
    const float upLength = length(up);

    const float t =
        -dot(ray.origin - center, normal) / dot(ray.direction, normal);
    if (t < ray.tmin || t > ray.tmax) return false;

    const Vec3 hitPos = ray(t);
    const float dx = dot(hitPos - leftCornerPoint, rightDir);
    const float dy = dot(hitPos - leftCornerPoint, upDir);
    if (dx < 0.0f || dx > rightLength || dy < 0.0f || dy > upLength)
      return false;

    info.t = t;
    info.hitPos = hitPos;
    info.hitNormal = dot(-ray.direction, normal) > 0.0 ? normal : -normal;
    return true;
  }
};

#endif