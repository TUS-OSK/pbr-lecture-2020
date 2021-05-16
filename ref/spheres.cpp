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
  const auto camera = std::make_shared<PinholeCamera>(
      camPos, normalize(lookAt - camPos), 0.5f * PI);

  // レンダラーの作成
  Renderer renderer(width, height, camera);

  // シーンの作成
  Sky sky(Vec3f(1.0f));
  Scene scene(sky);

  const auto floor = std::make_shared<Plane>(Vec3f(-5, -1, -5), Vec3f(10, 0, 0),
                                             Vec3f(0, 0, 10));
  const auto sphere1 = std::make_shared<Sphere>(Vec3f(-2, 0, 1), 1);
  const auto sphere2 = std::make_shared<Sphere>(Vec3f(0, 0, 2), 1);
  const auto sphere3 = std::make_shared<Sphere>(Vec3f(2, 0, -1), 1);
  const auto sphere4 = std::make_shared<Sphere>(Vec3f(-2, 3, 1), 1);

  const auto mat1 = std::make_shared<Lambert>(Vec3f(0.9));
  const auto mat2 = std::make_shared<Lambert>(Vec3f(0.9, 0.1, 0.1));
  const auto mat3 = std::make_shared<Lambert>(Vec3f(0.1, 0.9, 0.1));
  const auto mat4 = std::make_shared<Lambert>(Vec3f(0.1, 0.1, 0.9));
  const auto mat5 = std::make_shared<Mirror>(Vec3f(0.95));
  const auto mat6 = std::make_shared<Glass>(Vec3f(1.0), 1.5f);

  scene.addPrimitive(Primitive(floor, mat1));
  scene.addPrimitive(Primitive(sphere1, mat2));
  scene.addPrimitive(Primitive(sphere2, mat6));
  scene.addPrimitive(Primitive(sphere3, mat4));
  scene.addPrimitive(Primitive(sphere4, mat5));

  // レンダリング
  renderer.render(scene, samples);

  // 画像の出力
  renderer.writePPM("output.ppm");

  return 0;
}