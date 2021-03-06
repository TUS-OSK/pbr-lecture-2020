#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H
#include <memory>

#include "bsdf.h"
#include "intersect-info.h"
#include "light.h"
#include "ray.h"
#include "shape.h"

class Primitive {
 public:
  std::shared_ptr<Shape> shape;
  std::shared_ptr<BSDF> bsdf;
  std::shared_ptr<AreaLight> areaLight;

  Primitive(const std::shared_ptr<Shape>& shape,
            const std::shared_ptr<BSDF>& bsdf,
            const std::shared_ptr<AreaLight>& areaLight = nullptr)
      : shape(shape), bsdf(bsdf), areaLight(areaLight) {}

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    IntersectInfo _info;
    if (shape->intersect(ray, _info)) {
      info = _info;
      info.hitPrimitive = this;
      return true;
    } else {
      return false;
    }
  }
};

#endif