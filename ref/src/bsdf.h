#ifndef _BSDF_H
#define _BSDF_H
#include "constant.h"
#include "rng.h"
#include "sampling.h"
#include "vec3.h"

inline float cosTheta(const Vec3f& w) { return w[1]; }
inline float absCosTheta(const Vec3f& w) { return std::abs(w[1]); }

// フレネル反射率を計算する
inline float fresnel(const Vec3f& w, const Vec3f& n, float ior1, float ior2) {
  const float f0 = std::pow((ior1 - ior2) / (ior1 + ior2), 2.0f);
  return f0 + (1.0f - f0) * std::pow(1.0f - std::abs(dot(w, n)), 5.0f);
}

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

class Glass : public BSDF {
 private:
  const Vec3f rho;
  const float ior;

 public:
  Glass(const Vec3f& rho, float ior) : rho(rho), ior(ior) {}

  Vec3f eval(const Vec3f& wo, const Vec3f& wi) const override {
    return Vec3f(0);
  }

  Vec3f sample(RNG& rng, const Vec3f& wo, Vec3f& wi,
               float& pdf) const override {
    // 物体外部 or 内部に応じて適切なパラメーターを設定
    float ior1, ior2;
    Vec3f n;
    if (wo[1] > 0) {
      ior1 = 1.0f;
      ior2 = ior;
      n = Vec3f(0, 1, 0);
    } else {
      ior1 = ior;
      ior2 = 1.0f;
      n = Vec3f(0, -1, 0);
    }

    // フレネル反射率
    const float fr = fresnel(wo, n, ior1, ior2);

    // 反射の場合
    if (rng.getNext() < fr) {
      wi = reflect(wo, n);
      pdf = 1;
      return rho / absCosTheta(wi);
    }
    // 屈折の場合
    else {
      Vec3f tr;
      if (refract(wo, n, ior1, ior2, tr)) {
        wi = tr;
        pdf = 1;
        return rho / absCosTheta(wi);
      }
      // 全反射の場合
      else {
        wi = reflect(wo, n);
        pdf = 1;
        return rho / absCosTheta(wi);
      }
    }
  }
};

#endif