#ifndef _BSDF_H
#define _BSDF_H
#include "constant.h"
#include "rng.h"
#include "sampling.h"
#include "vec3.h"

class BSDF {
 public:
  // BSDF x cosに比例するように方向サンプリングを行う
  // 返り値としてBSDFの値, pdfを返す
  virtual Vec3f sample(RNG& rng, Vec3f& dir, float& pdf) const = 0;
};

// Lambert BRDF
class Lambert : public BSDF {
 private:
  Vec3f rho;

 public:
  Lambert(const Vec3f& rho) : rho(rho) {}

  Vec3f sample(RNG& rng, Vec3f& dir, float& pdf) const {
    dir = sampleCosineHemisphere(rng.getNext(), rng.getNext(), pdf);
    return rho * PI_INV;
  }
};

#endif