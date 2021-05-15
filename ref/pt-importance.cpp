#include <omp.h>

#include <cmath>

#include "constant.h"
#include "image.h"
#include "pinhole-camera.h"
#include "rng.h"
#include "scene.h"

// 法線から接空間の基底を計算する
void tangentSpaceBasis(const Vec3f& n, Vec3f& t, Vec3f& b) {
  if (std::abs(n[1]) < 0.9f) {
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
  return Vec3f(std::cos(phi) * std::sin(theta), std::cos(theta),
               std::sin(phi) * std::sin(theta));
}

// 接空間での半球コサイン比例サンプリング
Vec3f sampleCosineHemisphere(float u, float v) {
  const float theta =
      0.5f * std::acos(std::clamp(1.0f - 2.0f * u, -1.0f, 1.0f));
  const float phi = 2.0f * PI * v;
  return Vec3f(std::cos(phi) * std::sin(theta), std::cos(theta),
               std::sin(phi) * std::sin(theta));
}

// Primary Rayを入力とし, 放射輝度をパストレーシングで計算して返す
Vec3f pathTracing(const Ray& ray_in, const Scene& scene, RNG& rng) {
  constexpr int maxDepth = 100;                 // 最大反射回数
  constexpr float russianRouletteProb = 0.99f;  // ロシアンルーレットの確率
  constexpr Vec3f rho = {0.9f, 0.9f, 0.9f};  // Lambert BRDFのrho

  // path tracing
  Vec3f radiance = 0;            // 放射輝度
  Vec3f throughput = {1, 1, 1};  // f*cos / pdfの積
  Ray ray = ray_in;
  for (int i = 0; i < maxDepth; ++i) {
    // ロシアンルーレット
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
    // 半球面コサイン比例サンプリング
    const Vec3f directionTangent =
        sampleCosineHemisphere(rng.getNext(), rng.getNext());
    // 接空間からワールド座標系への変換
    const Vec3f direction =
        localToWorld(directionTangent, t, info.hitNormal, b);

    // BRDFの計算
    const Vec3f brdf = rho / PI;
    // cosの計算
    const float cos = std::abs(dot(direction, info.hitNormal));
    // p.d.f.の計算
    const float pdf = 1.0f / PI * cos;

    // throughputの更新
    throughput *= brdf * cos / pdf;

    // 次のレイの生成
    ray.origin = info.hitPos;
    ray.direction = direction;
  }

  return radiance;
}

int main() {
  constexpr int width = 512;    // 画像の横幅[px]
  constexpr int height = 512;   // 画像の縦幅[px]
  constexpr int samples = 100;  // サンプル数
  Image img(width, height);

  // カメラの設定
  constexpr Vec3f camPos(4, 1, 7);
  constexpr Vec3f lookAt(0);
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

  // レンダリング
#pragma omp parallel for schedule(dynamic, 1) collapse(2)
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      // NOTE: 並列化のために画素ごとに乱数生成器を用意する
      RNG rng(i + width * j);

      Vec3f color(0);
      for (int k = 0; k < samples; ++k) {
        // (u, v)の計算
        const float u = (2.0f * (i + rng.getNext()) - width) / height;
        const float v = (2.0f * (j + rng.getNext()) - height) / height;

        // 最初のレイの生成
        const Ray ray = camera.sampleRay(u, v);

        // 放射輝度の計算
        color += pathTracing(ray, scene, rng);
      }

      // 平均
      color /= Vec3f(samples);

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
