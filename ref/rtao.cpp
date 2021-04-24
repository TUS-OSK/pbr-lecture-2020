#include <omp.h>

#include <cmath>

#include "image.h"
#include "pinhole-camera.h"
#include "rng.h"
#include "scene.h"

constexpr float PI = 3.14159265359f;

// ローカル座標系からワールド座標系への変換
Vec3f localToWorld(const Vec3f& v, const Vec3f& lx, const Vec3f& ly,
                   const Vec3f& lz) {
  return Vec3f(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
               v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
               v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
}

// 法線から接空間の基底を計算する
void tangentSpaceBasis(const Vec3f& n, Vec3f& t, Vec3f& b) {
  if (n[1] < 0.9f) {
    t = normalize(cross(n, Vec3f(0, 1, 0)));
  } else {
    t = normalize(cross(n, Vec3f(0, 0, -1)));
  }
  b = normalize(cross(t, n));
}

// 接空間での半球面一様サンプリング
Vec3f sampleHemisphere(float u, float v) {
  const float theta = std::acos(std::max(1.0f - u, 0.0f));
  const float phi = 2.0f * PI * v;
  return Vec3f(std::cos(phi) * std::sin(theta), 1.0f - u,
               std::sin(phi) * std::sin(theta));
}

// AO
float RTAO(const IntersectInfo& info, const Scene& scene, RNG& rng) {
  // AOサンプル数
  constexpr int AO_SAMPLES = 100;
  // 遮蔽判定距離
  constexpr float DISTANCE = 10.0f;

  // 遮蔽割合の計算
  int count = 0;
  for (int i = 0; i < AO_SAMPLES; ++i) {
    // 接空間の基底の計算
    Vec3f t, b;
    tangentSpaceBasis(info.hitNormal, t, b);

    // 半球面一様サンプリング
    const Vec3f direction_tangent =
        sampleHemisphere(rng.getNext(), rng.getNext());

    // 接空間からワールド座標系への変換
    const Vec3f direction =
        localToWorld(direction_tangent, t, info.hitNormal, b);

    // 遮蔽判定
    Ray shadow_ray(info.hitPos, direction);
    IntersectInfo shadow_info;
    if (scene.intersect(shadow_ray, shadow_info) && shadow_info.t < DISTANCE) {
      count++;
    }
  }

  return static_cast<float>(count) / AO_SAMPLES;
}

int main() {
  constexpr int width = 512;
  constexpr int height = 512;
  constexpr int SSAA_samples = 16;
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

#pragma omp parallel for schedule(dynamic, 1)
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

        // 1.0 - AOを色にする
        IntersectInfo info;
        if (scene.intersect(ray, info)) {
          color += Vec3f(1.0f - RTAO(info, scene, rng));
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