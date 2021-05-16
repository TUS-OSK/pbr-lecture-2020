#include <omp.h>

#include <cmath>

#include "renderer.h"
#include "scene.h"

int main() {
  constexpr int width = 512;    // 画像の横幅[px]
  constexpr int height = 512;   // 画像の縦幅[px]
  constexpr int samples = 100;  // サンプル数

  // カメラの設定
  constexpr Vec3f camPos(4, 1, 7);
  constexpr Vec3f lookAt(0);

  // レンダラーの作成
  Renderer renderer(width, height, camPos, normalize(lookAt - camPos));

  // シーンの作成
  Scene scene;

  const auto mat1 = std::make_shared<Lambert>(Vec3f(0.9));
  const auto mat2 = std::make_shared<Lambert>(Vec3f(0.9, 0.1, 0.1));
  const auto mat3 = std::make_shared<Lambert>(Vec3f(0.1, 0.9, 0.1));
  const auto mat4 = std::make_shared<Lambert>(Vec3f(0.1, 0.1, 0.9));
  const auto mat5 = std::make_shared<Mirror>(Vec3f(0.95));
  const auto mat6 = std::make_shared<Glass>(Vec3f(1.0), 1.5f);

  scene.addSphere(Sphere(Vec3f(0, -1001, 0), 1000.0, mat1));
  scene.addSphere(Sphere(Vec3f(-2, 0, 1), 1.0, mat2));
  scene.addSphere(Sphere(Vec3f(0, 0, 2), 1.0, mat6));
  scene.addSphere(Sphere(Vec3f(2, 0, -1), 1.0, mat4));
  scene.addSphere(Sphere(Vec3f(-2, 3, 1), 1.0, mat5));

  // レンダリング
  renderer.render(scene, samples);

  // 画像の出力
  renderer.writePPM("output.ppm");

  return 0;
}