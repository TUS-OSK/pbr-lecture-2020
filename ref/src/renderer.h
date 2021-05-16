#ifndef _RENDERER_H
#define _RENDERER_H
#include <omp.h>

#include "image.h"
#include "integrator.h"
#include "pinhole-camera.h"
#include "scene.h"

class Renderer {
 private:
  Image image;
  std::shared_ptr<PinholeCamera> camera;
  std::shared_ptr<Integrator> integrator;

 public:
  Renderer(unsigned int width, unsigned int height, const Vec3f& camPos,
           const Vec3f& camForward)
      : image{width, height},
        camera(std::make_shared<PinholeCamera>(camPos, camForward)),
        integrator(std::make_shared<PathTracing>()) {}

  // レンダリングする
  void render(const Scene& scene, int samples) {
    const int width = image.getWidth();
    const int height = image.getHeight();

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
          const Ray ray = camera->sampleRay(u, v);

          // 放射輝度の計算
          color += integrator->radiance(ray, scene, rng);
        }

        // 平均
        color /= Vec3f(samples);

        // 画素への書き込み
        image.setPixel(i, j, color);
      }
    }
  }

  // PPM画像を出力する
  void writePPM(const std::string& filename) {
    image.gammaCorrection();
    image.writePPM(filename);
  }
};

#endif