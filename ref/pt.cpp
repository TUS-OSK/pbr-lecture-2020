#include <omp.h>

#include <cmath>

#include "constant.h"
#include "image.h"
#include "integrator.h"
#include "pinhole-camera.h"
#include "rng.h"
#include "sampling.h"
#include "scene.h"

int main() {
  constexpr int width = 512;    // 画像の横幅[px]
  constexpr int height = 512;   // 画像の縦幅[px]
  constexpr int samples = 100;  // サンプル数
  Image img(width, height);

  // Integratorの設定
  const auto integrator = std::make_shared<PathTracing>();

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
        color += integrator->radiance(ray, scene, rng);
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