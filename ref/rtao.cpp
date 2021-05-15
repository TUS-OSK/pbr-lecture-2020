#include <omp.h>

#include <cmath>

#include "constant.h"
#include "image.h"
#include "pinhole-camera.h"
#include "rng.h"
#include "sampling.h"
#include "scene.h"

// AO
float RTAO(const IntersectInfo& info, const Scene& scene, RNG& rng) {
  // 遮蔽判定距離
  constexpr float DISTANCE = 10.0f;
  // rho
  constexpr float rho = 1.0f;

  // RTAO
  float radiance = 0;
  // 接空間の基底の計算
  Vec3f t, b;
  tangentSpaceBasis(info.hitNormal, t, b);

  // 半球面一様サンプリング
  float _pdf;
  const Vec3f direction_tangent =
      sampleHemisphere(rng.getNext(), rng.getNext(), _pdf);

  // 接空間からワールド座標系への変換
  const Vec3f direction = localToWorld(direction_tangent, t, info.hitNormal, b);

  // 遮蔽判定
  Ray shadow_ray(info.hitPos, direction);
  IntersectInfo shadow_info;
  if (!scene.intersect(shadow_ray, shadow_info)) {
    const float cos = dot(direction, info.hitNormal);
    radiance += 2.0 * rho * cos;
  }

  return radiance;
}

int main() {
  constexpr int width = 512;
  constexpr int height = 512;
  constexpr int SSAA_samples = 1000;
  Image img(width, height);

  const Vec3f camPos(4, 1, 7);
  const Vec3f lookAt(0);
  PinholeCamera camera(camPos, normalize(lookAt - camPos));

  // シーンの作成
  Scene scene;
  scene.addSphere(
      Sphere(Vec3f(0, -1001, 0), 1000.0, Vec3f(0.9), MaterialType::Diffuse));
  scene.addSphere(Sphere(Vec3f(-2, 0, 1), 1.0, Vec3f(0.8, 0.2, 0.2),
                         MaterialType::Diffuse));
  scene.addSphere(
      Sphere(Vec3f(0), 1.0, Vec3f(0.2, 0.8, 0.2), MaterialType::Diffuse));
  scene.addSphere(Sphere(Vec3f(2, 0, -1), 1.0, Vec3f(0.2, 0.2, 0.8),
                         MaterialType::Diffuse));
  scene.addSphere(Sphere(Vec3f(-2, 3, 1), 1.0, Vec3f(1), MaterialType::Mirror));
  scene.addSphere(Sphere(Vec3f(3, 1, 2), 1.0, Vec3f(1), MaterialType::Glass));

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      // NOTE: 並列化のために画素ごとに乱数生成器を用意する
      RNG rng(i + width * j);

      Vec3f color(0);
      for (int k = 0; k < SSAA_samples; ++k) {
        // (u, v)の計算
        const float u = (2.0f * (i + rng.getNext()) - width) / height;
        const float v = (2.0f * (j + rng.getNext()) - height) / height;

        // レイの生成
        const Ray ray = camera.sampleRay(u, v);

        // 放射輝度の計算
        IntersectInfo info;
        if (scene.intersect(ray, info)) {
          const float radiance = RTAO(info, scene, rng);
          color += Vec3f(radiance);
        } else {
          color += Vec3f(1.0f);
        }
      }

      // 平均
      color /= Vec3f(SSAA_samples);

      // 画素への書き込み
      img.setPixel(i, j, color);
    }
  }

  // ガンマ補正
  img.gammaCorrection();

  // PPM画像の生成
  img.writePPM("output.ppm");

  return 0;
}