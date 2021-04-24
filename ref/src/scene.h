#ifndef _SCENE_H
#define _SCENE_H
#include <vector>

#include "intersect-info.h"
#include "ray.h"
#include "sphere.h"

class Scene {
 private:
  std::vector<Sphere> spheres;

 public:
  Scene() {}

  void addSphere(const Sphere& sphere) { spheres.push_back(sphere); }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    bool hit = false;

    IntersectInfo info_each;
    // NOTE: 最小値を求めるために, 予め最大値をセットしておく
    info.t = ray.tmax;
    for (const auto& sphere : spheres) {
      // 交差距離が以前に交差したものより短かったら交差情報を更新
      if (sphere.intersect(ray, info_each) && info_each.t < info.t) {
        hit = true;
        info = info_each;
      }
    }

    return hit;
  }
};

#endif