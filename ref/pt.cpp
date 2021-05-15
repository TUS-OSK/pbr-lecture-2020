#include <omp.h>

#include <cmath>

#include "constant.h"
#include "image.h"
#include "pinhole-camera.h"
#include "rng.h"
#include "sampling.h"
#include "scene.h"

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

    // BSDF Sampling
    float pdf;
    Vec3f directionTangent;
    const Vec3f bsdf = info.hitSphere->bsdf->sample(rng, directionTangent, pdf);
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

  const auto mat1 = std::make_shared<Lambert>(Vec3f(0.9));
  const auto mat2 = std::make_shared<Lambert>(Vec3f(0.9, 0.1, 0.1));
  const auto mat3 = std::make_shared<Lambert>(Vec3f(0.1, 0.9, 0.1));
  const auto mat4 = std::make_shared<Lambert>(Vec3f(0.1, 0.1, 0.9));

  scene.addSphere(Sphere(Vec3f(0, -1001, 0), 1000.0, mat1));
  scene.addSphere(Sphere(Vec3f(-2, 0, 1), 1.0, mat2));
  scene.addSphere(Sphere(Vec3f(0), 1.0, mat3));
  scene.addSphere(Sphere(Vec3f(2, 0, -1), 1.0, mat4));
  scene.addSphere(Sphere(Vec3f(-2, 3, 1), 1.0, mat1));
  scene.addSphere(Sphere(Vec3f(3, 1, 2), 1.0, mat1));

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