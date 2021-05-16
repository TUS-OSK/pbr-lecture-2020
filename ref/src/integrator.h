#ifndef _INTEGRATOR_H
#define _INTEGRATOR_H
#include "ray.h"
#include "rng.h"
#include "scene.h"

class Integrator {
 public:
  // 放射輝度を計算する
  virtual Vec3f radiance(const Ray& ray, const Scene& scene,
                         RNG& rng) const = 0;
};

class PathTracing : public Integrator {
 private:
  int maxDepth = 100;  // 最大反射回数

 public:
  PathTracing(int maxDepth = 100) : maxDepth(maxDepth) {}

  Vec3f radiance(const Ray& ray_in, const Scene& scene,
                 RNG& rng) const override {
    Vec3f radiance = {0};          // 放射輝度
    Vec3f throughput = {1, 1, 1};  // f*cos / pdfの積
    Ray ray = ray_in;
    for (int i = 0; i < maxDepth; ++i) {
      // ロシアンルーレット
      const float russianRouletteProb = std::min(
          std::max(std::max(throughput[0], throughput[1]), throughput[2]),
          1.0f);
      if (rng.getNext() > russianRouletteProb) {
        break;
      }
      throughput /= russianRouletteProb;

      // レイを飛ばして交差点を計算
      IntersectInfo info;
      if (!scene.intersect(ray, info)) {
        // 空に飛んでいった場合
        radiance += throughput * Vec3f(1);
        break;
      }

      // 接空間の基底の計算
      Vec3f t, b;
      tangentSpaceBasis(info.hitNormal, t, b);

      // レイの方向を接空間に変換
      const Vec3f woTangent =
          worldToLocal(-ray.direction, t, info.hitNormal, b);

      // BSDF Sampling
      float pdf;
      Vec3f wiTangent;
      const Vec3f bsdf =
          info.hitPrimitive->bsdf->sample(rng, woTangent, wiTangent, pdf);
      // 接空間からワールド座標系への変換
      const Vec3f wi = localToWorld(wiTangent, t, info.hitNormal, b);

      // cosの計算
      // NOTE:
      // 物体内部の場合, 法線がひっくり返っている可能性があるのでabsをつけている
      const float cos = std::abs(dot(wi, info.hitNormal));

      // throughputの更新
      throughput *= bsdf * cos / pdf;

      // 次のレイの生成
      ray.origin = info.hitPos;
      ray.direction = wi;
    }

    return radiance;
  }
};

#endif