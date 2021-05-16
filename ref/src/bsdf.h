#ifndef _BSDF_H
#define _BSDF_H
#include "constant.h"
#include "rng.h"
#include "sampling.h"
#include "vec3.h"

class BSDF {
 public:
  // BSDFの値を計算して返す
  virtual Vec3f eval(const Vec3f& wo, const Vec3f& wi) const = 0;

  // BSDF x cosに比例するように方向サンプリングを行う
  // 返り値としてBSDFの値, 方向ベクトル, pdfを返す
  virtual Vec3f sample(RNG& rng, Vec3f& dir, float& pdf) const = 0;
};

// Lambert BRDF
class Lambert : public BSDF {
 private:
  Vec3f rho;

 public:
  Lambert(const Vec3f& rho) : rho(rho) {}

  Vec3f eval(const Vec3f& wo, const Vec3f& wi) const override {
    return rho * PI_INV;
  }

  Vec3f sample(RNG& rng, Vec3f& dir, float& pdf) const override {
    dir = sampleCosineHemisphere(rng.getNext(), rng.getNext(), pdf);
    return rho * PI_INV;
  }
};

#endif