#ifndef _SCENE_H
#define _SCENE_H
#include <vector>

#include "intersect-info.h"
#include "primitive.h"
#include "ray.h"

class Scene {
 private:
  std::vector<Primitive> primitives;

 public:
  Scene() {}

  void addPrimitive(const Primitive& primitive) {
    primitives.push_back(primitive);
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    bool hit = false;

    IntersectInfo info_each;
    // NOTE: 最小値を求めるために, 予め最大値をセットしておく
    info.t = ray.tmax;
    for (const auto& primitive : primitives) {
      // 交差距離が以前に交差したものより短かったら交差情報を更新
      if (primitive.intersect(ray, info_each) && info_each.t < info.t) {
        hit = true;
        info = info_each;
      }
    }

    return hit;
  }
};

#endif