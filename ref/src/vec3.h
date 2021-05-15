#ifndef _VEC3_H
#define _VEC3_H
#include <cmath>
#include <iostream>

template <typename T>
struct Vec3 {
  T v[3];

  constexpr Vec3() : v{0, 0, 0} {}
  constexpr Vec3(T x) : v{x, x, x} {}
  constexpr Vec3(T x, T y, T z) : v{x, y, z} {}

  T operator[](unsigned int i) const { return v[i]; }
  T& operator[](unsigned int i) { return v[i]; }

  Vec3 operator-() const { return Vec3(-v[0], -v[1], -v[2]); }

  Vec3& operator+=(const Vec3& v) {
    this->v[0] += v[0];
    this->v[1] += v[1];
    this->v[2] += v[2];
    return *this;
  }

  Vec3& operator-=(const Vec3& v) {
    this->v[0] -= v[0];
    this->v[1] -= v[1];
    this->v[2] -= v[2];
    return *this;
  }

  Vec3& operator*=(const Vec3& v) {
    this->v[0] *= v[0];
    this->v[1] *= v[1];
    this->v[2] *= v[2];
    return *this;
  }

  Vec3& operator/=(const Vec3& v) {
    this->v[0] /= v[0];
    this->v[1] /= v[1];
    this->v[2] /= v[2];
    return *this;
  }
};

// ベクトル同士の加算
template <typename T>
inline Vec3<T> operator+(const Vec3<T>& v1, const Vec3<T>& v2) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] + v2[i];
  }
  return ret;
}

// ベクトル同士の減算
template <typename T>
inline Vec3<T> operator-(const Vec3<T>& v1, const Vec3<T>& v2) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] - v2[i];
  }
  return ret;
}

// スカラー倍
template <typename T>
inline Vec3<T> operator*(const Vec3<T>& v1, const T& k) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] * k;
  }
  return ret;
}
template <typename T>
inline Vec3<T> operator*(const T& k, const Vec3<T>& v2) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = k * v2[i];
  }
  return ret;
}
template <typename T>
inline Vec3<T> operator/(const Vec3<T>& v1, const T& k) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] / k;
  }
  return ret;
}
template <typename T>
inline Vec3<T> operator/(const T& k, const Vec3<T>& v2) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = k / v2[i];
  }
  return ret;
}

// アダマール積
template <typename T>
inline Vec3<T> operator*(const Vec3<T>& v1, const Vec3<T>& v2) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] * v2[i];
  }
  return ret;
}
template <typename T>
inline Vec3<T> operator/(const Vec3<T>& v1, const Vec3<T>& v2) {
  Vec3<T> ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] / v2[i];
  }
  return ret;
}

// ベクトルの長さ
template <typename T>
inline float length(const Vec3<T>& v) {
  float sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += v[i] * v[i];
  }
  return std::sqrt(sum);
}

template <typename T>
inline float length2(const Vec3<T>& v) {
  float sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += v[i] * v[i];
  }
  return sum;
}

// 正規化
template <typename T>
inline Vec3<T> normalize(const Vec3<T>& v) {
  return v / length(v);
}

// 内積
template <typename T>
inline T dot(const Vec3<T>& v1, const Vec3<T>& v2) {
  T ret = 0;
  for (int i = 0; i < 3; ++i) {
    ret += v1[i] * v2[i];
  }
  return ret;
}

// 外積
template <typename T>
inline Vec3<T> cross(const Vec3<T>& v1, const Vec3<T>& v2) {
  Vec3<T> ret;
  ret[0] = v1[1] * v2[2] - v1[2] * v2[1];
  ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
  ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
  return ret;
}

// print
template <typename T>
inline std::ostream& operator<<(std::ostream& stream, const Vec3<T>& v) {
  stream << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
  return stream;
}

// ワールド座標系からローカル座標系への変換
template <typename T>
Vec3<T> worldToLocal(const Vec3<T>& v, const Vec3<T>& lx, const Vec3<T>& ly,
                     const Vec3<T>& lz) {
  return Vec3<T>(v[0] * lx[0] + v[1] * lx[1] + v[2] * lx[2],
                 v[0] * ly[0] + v[1] * ly[1] + v[2] * ly[2],
                 v[0] * lz[0] + v[1] * lz[1] + v[2] * lz[2]);
}

// ローカル座標系からワールド座標系への変換
template <typename T>
Vec3<T> localToWorld(const Vec3<T>& v, const Vec3<T>& lx, const Vec3<T>& ly,
                     const Vec3<T>& lz) {
  return Vec3<T>(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
                 v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
                 v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
}

using Vec3u = Vec3<unsigned int>;
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;

// 反射ベクトルの計算
inline Vec3f reflect(const Vec3f& v, const Vec3f& n) {
  return -v + 2.0f * dot(v, n) * n;
}

// 屈折ベクトルの計算
inline bool refract(const Vec3f& v, const Vec3f& n, float ior1, float ior2,
                    Vec3f& r) {
  const Vec3f t_h = -ior1 / ior2 * (v - dot(v, n) * n);

  // 全反射
  if (length2(t_h) > 1.0) return false;

  const Vec3f t_p = -std::sqrt(std::max(1.0f - length2(t_h), 0.0f)) * n;
  r = t_h + t_p;

  return true;
}

// 法線から接空間の基底を計算する
void tangentSpaceBasis(const Vec3f& n, Vec3f& t, Vec3f& b) {
  if (n[1] < 0.9f) {
    t = normalize(cross(n, Vec3f(0, 1, 0)));
  } else {
    t = normalize(cross(n, Vec3f(0, 0, -1)));
  }
  b = normalize(cross(t, n));
}

#endif