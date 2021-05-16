#ifndef _BSDF_H
#define _BSDF_H
#include "constant.h"
#include "rng.h"
#include "sampling.h"
#include "vec3.h"

inline float cosTheta(const Vec3f& w) { return w[1]; }
inline float absCosTheta(const Vec3f& w) { return std::abs(w[1]); }

class BSDF {
 public:
  // BSDFの値を計算して返す
  virtual Vec3f eval(const Vec3f& wo, const Vec3f& wi) const = 0;

  // BSDF x cosに比例するように方向サンプリングを行う
  // 返り値としてBSDFの値, 方向ベクトル, pdfを返す
  virtual Vec3f sample(RNG& rng, const Vec3f& wo, Vec3f& wi,
                       float& pdf) const = 0;
};

// Lambert BRDF
class Lambert : public BSDF {
 private:
  const Vec3f rho;

 public:
  Lambert(const Vec3f& rho) : rho(rho) {}

  Vec3f eval(const Vec3f& wo, const Vec3f& wi) const override {
    return rho * PI_INV;
  }

  Vec3f sample(RNG& rng, const Vec3f& wo, Vec3f& wi,
               float& pdf) const override {
    wi = sampleCosineHemisphere(rng.getNext(), rng.getNext(), pdf);
    return rho * PI_INV;
  }
};

class Mirror : public BSDF {
 private:
  const Vec3f rho;

 public:
  Mirror(const Vec3f& rho) : rho(rho) {}

  Vec3f eval(const Vec3f& wo, const Vec3f& wi) const override {
    return Vec3f(0);
  }

  Vec3f sample(RNG& rng, const Vec3f& wo, Vec3f& wi,
               float& pdf) const override {
    wi = reflect(wo, Vec3f(0, 1, 0));
    pdf = 1;
    return rho / absCosTheta(wi);
  }
};

#endif