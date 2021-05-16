#include <cmath>

#include "renderer.h"
#include "scene.h"

int main() {
  constexpr int width = 512;     // 画像の横幅[px]
  constexpr int height = 512;    // 画像の縦幅[px]
  constexpr int samples = 1000;  // サンプル数

  // カメラの設定
  constexpr Vec3f camPos(2.78, 2.73, -9);
  constexpr Vec3f lookAt(2.78, 2.73, 2.796);
  const auto camera = std::make_shared<PinholeCamera>(
      camPos, normalize(lookAt - camPos), 0.25f * PI);

  // レンダラーの作成
  Renderer renderer(width, height, camera);

  // シーンの作成
  Sky sky(Vec3f(0.0f));
  Scene scene(sky);

  const auto white = std::make_shared<Lambert>(Vec3f(0.8));
  const auto red = std::make_shared<Lambert>(Vec3f(0.8, 0.05, 0.05));
  const auto green = std::make_shared<Lambert>(Vec3f(0.05, 0.8, 0.05));
  const auto glass = std::make_shared<Glass>(Vec3f(1.0f), 1.5f);

  const auto floor =
      std::make_shared<Plane>(Vec3f(0), Vec3f(0, 0, 5.592), Vec3f(5.56, 0, 0));
  const auto rightWall =
      std::make_shared<Plane>(Vec3f(0), Vec3f(0, 5.488, 0), Vec3f(0, 0, 5.592));
  const auto leftWall = std::make_shared<Plane>(
      Vec3f(5.56, 0, 0), Vec3f(0, 0, 5.592), Vec3f(0, 5.488, 0));
  const auto ceil = std::make_shared<Plane>(
      Vec3f(0, 5.488, 0), Vec3f(5.56, 0, 0), Vec3f(0, 0, 5.592));
  const auto backWall = std::make_shared<Plane>(
      Vec3f(0, 0, 5.592), Vec3f(0, 5.488, 0), Vec3f(5.56, 0, 0));

  const auto shortBox1 = std::make_shared<Plane>(
      Vec3f(1.3, 1.65, 0.65), Vec3f(-0.48, 0, 1.6), Vec3f(1.6, 0, 0.49));
  const auto shortBox2 = std::make_shared<Plane>(
      Vec3f(2.9, 0, 1.14), Vec3f(0, 1.65, 0), Vec3f(-0.5, 0, 1.58));
  const auto shortBox3 = std::make_shared<Plane>(
      Vec3f(1.3, 0, 0.65), Vec3f(0, 1.65, 0), Vec3f(1.6, 0, 0.49));
  const auto shortBox4 = std::make_shared<Plane>(
      Vec3f(0.82, 0, 2.25), Vec3f(0, 1.65, 0), Vec3f(0.48, 0, -1.6));
  const auto shortBox5 = std::make_shared<Plane>(
      Vec3f(2.4, 0, 2.72), Vec3f(0, 1.65, 0), Vec3f(-1.58, 0, -0.47));

  const auto tallBox1 = std::make_shared<Plane>(
      Vec3f(4.23, 3.30, 2.47), Vec3f(-1.58, 0, 0.49), Vec3f(0.49, 0, 1.59));
  const auto tallBox2 = std::make_shared<Plane>(
      Vec3f(4.23, 0, 2.47), Vec3f(0, 3.3, 0), Vec3f(0.49, 0, 1.59));
  const auto tallBox3 = std::make_shared<Plane>(
      Vec3f(4.72, 0, 4.06), Vec3f(0, 3.3, 0), Vec3f(-1.58, 0, 0.5));
  const auto tallBox4 = std::make_shared<Plane>(
      Vec3f(3.14, 0, 4.56), Vec3f(0, 3.3, 0), Vec3f(-0.49, 0, -1.6));
  const auto tallBox5 = std::make_shared<Plane>(
      Vec3f(2.65, 0, 2.96), Vec3f(0, 3.3, 0), Vec3f(1.58, 0, -0.49));

  const auto light_s = std::make_shared<Plane>(
      Vec3f(3.43, 5.486, 2.27), Vec3f(-1.3, 0, 0), Vec3f(0, 0, 1.05));

  const auto light = std::make_shared<AreaLight>(Vec3f(30));

  scene.addPrimitive(Primitive(floor, white));
  scene.addPrimitive(Primitive(rightWall, red));
  scene.addPrimitive(Primitive(leftWall, green));
  scene.addPrimitive(Primitive(ceil, white));
  scene.addPrimitive(Primitive(backWall, white));
  scene.addPrimitive(Primitive(shortBox1, glass));
  scene.addPrimitive(Primitive(shortBox2, glass));
  scene.addPrimitive(Primitive(shortBox3, glass));
  scene.addPrimitive(Primitive(shortBox4, glass));
  scene.addPrimitive(Primitive(shortBox5, glass));
  scene.addPrimitive(Primitive(tallBox1, glass));
  scene.addPrimitive(Primitive(tallBox2, glass));
  scene.addPrimitive(Primitive(tallBox3, glass));
  scene.addPrimitive(Primitive(tallBox4, glass));
  scene.addPrimitive(Primitive(tallBox5, glass));
  scene.addPrimitive(Primitive(light_s, white, light));

  // レンダリング
  renderer.render(scene, samples);

  // 画像の出力
  renderer.writePPM("output.ppm");

  return 0;
}