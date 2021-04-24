#ifndef _IMAGE_H
#define _IMAGE_H
#include <algorithm>
#include <fstream>
#include <string>

#include "vec3.h"

class Image {
 private:
  unsigned int width;   // 横の画素数
  unsigned int height;  // 縦の画素数
  float* pixels;        // 画素のRGB配列

 public:
  Image(unsigned int width, unsigned int height)
      : width(width), height(height) {
    pixels = new float[3 * width * height];

    // 0で初期化
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        const int idx = 3 * i + 3 * width * j;
        pixels[idx] = 0;
        pixels[idx + 1] = 0;
        pixels[idx + 2] = 0;
      }
    }
  }
  ~Image() { delete[] pixels; }

  unsigned int getWidth() const { return width; }
  unsigned int getHeight() const { return height; }

  void setPixel(unsigned int i, unsigned int j, const Vec3f& RGB) {
    const int idx = 3 * i + 3 * width * j;
    pixels[idx] = RGB[0];      // R
    pixels[idx + 1] = RGB[1];  // G
    pixels[idx + 2] = RGB[2];  // B
  }

  void writePPM(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
      std::cerr << "failed to open " << filename << std::endl;
      return;
    }

    file << "P3" << std::endl;
    file << width << " " << height << std::endl;
    file << "255" << std::endl;
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        const int idx = 3 * i + 3 * width * j;
        float R = pixels[idx];
        float G = pixels[idx + 1];
        float B = pixels[idx + 2];

        // 各成分を[0, 255]に含まれるように変換し出力
        file << static_cast<unsigned int>(std::clamp(255.0f * R, 0.0f, 255.0f))
             << " ";
        file << static_cast<unsigned int>(std::clamp(255.0f * G, 0.0f, 255.0f))
             << " ";
        file << static_cast<unsigned int>(std::clamp(255.0f * B, 0.0f, 255.0f))
             << std::endl;
      }
    }
    file.close();
  }

  // ガンマ補正
  void gammaCorrection() {
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        const int idx = 3 * i + 3 * width * j;
        pixels[idx] = std::pow(pixels[idx], 1 / 2.2f);          // R
        pixels[idx + 1] = std::pow(pixels[idx + 1], 1 / 2.2f);  // G
        pixels[idx + 2] = std::pow(pixels[idx + 2], 1 / 2.2f);  // B
      }
    }
  }
};

#endif