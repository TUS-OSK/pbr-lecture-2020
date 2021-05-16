#ifndef _LIGHT_H
#define _LIGHT_H
#include "vec3.h"

class Light {
 public:
  virtual Vec3f Le() const = 0;
};

class AreaLight : public Light {
 private:
  const Vec3f le;

 public:
  AreaLight(const Vec3f& le) : le(le) {}

  Vec3f Le() const override { return le; }
};

class Sky : public Light {
 private:
  const Vec3f le;

 public:
  Sky(const Vec3f& le) : le(le) {}

  Vec3f Le() const override { return le; }
};

#endif