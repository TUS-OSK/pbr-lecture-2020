#ifndef _CAMERA_H
#define _CAMERA_H
#include "ray.h"
#include "vec3.h"

class Camera {
 protected:
  Vec3f camPos;      // イメージセンサーの中心位置
  Vec3f camForward;  // カメラの前方向
  Vec3f camRight;    // カメラの右方向
  Vec3f camUp;       // カメラの上方向

 public:
  Camera(const Vec3f& camPos, const Vec3f& camForward)
      : camPos(camPos), camForward(camForward) {
    camRight = normalize(cross(camForward, Vec3f(0, 1, 0)));
    camUp = normalize(cross(camRight, camForward));

    std::cout << "[Camera] camPos: " << camPos << std::endl;
    std::cout << "[Camera] camForward: " << camForward << std::endl;
    std::cout << "[Camera] camRight: " << camRight << std::endl;
    std::cout << "[Camera] camUp: " << camUp << std::endl;
  }

  virtual Ray sampleRay(float u, float v) const = 0;
};

class PinholeCamera : public Camera {
 private:
  float f;  // ピンホールまでの距離

 public:
  PinholeCamera(const Vec3f& camPos, const Vec3f& camForward, float fov)
      : Camera(camPos, camForward) {
    f = 1.0f / std::tan(0.5f * fov);
  }

  Ray sampleRay(float u, float v) const override {
    const Vec3f I = camPos + u * camRight + v * camUp;  // 画素上の点の位置
    const Vec3f P = camPos + f * camForward;  // ピンホールの位置
    return Ray(I, normalize(P - I));
  }
};

#endif