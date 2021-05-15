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
      const float russianRouletteProb =
          std::max(std::max(throughput[0], throughput[1]), throughput[2]);
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

      // BSDF Sampling
      float pdf;
      Vec3f directionTangent;
      const Vec3f bsdf =
          info.hitSphere->bsdf->sample(rng, directionTangent, pdf);
      // 接空間からワールド座標系への変換
      const Vec3f direction =
          localToWorld(directionTangent, t, info.hitNormal, b);

      // cosの計算
      const float cos = std::abs(dot(direction, info.hitNormal));

      // throughputの更新
      throughput *= bsdf * cos / pdf;

      // 次のレイの生成
      ray.origin = info.hitPos;
      ray.direction = direction;
    }

    return radiance;
  }
};

#endif