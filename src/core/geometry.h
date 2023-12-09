#ifndef CORE_GEOMETRY_H
#define CORE_GEOMETRY_H
// This file contains the most basic of geometry required for pbrt to work.
// MOst classes and functions are graciously covered by glm, however some do
// still need to be implemented.
//
// Created on 09/12/2023 S Alam

// TODO: Implement useful assertions
#include <glm/glm.hpp>

#include "phr.h"

// This function creates a local coordinate system given a vector in 3D space.
// We can construct the two other vectors by applying the cross-product,since
// they are orthogonal to both. Note that the vectors generated are unique up to
// rotation.
//

template <typename T>
class Vector2 : public glm::tvec2<T> {
 public:
  Vector2() : glm::tvec2<T>() {}
  Vector2(T x, T y) : glm::tvec2<T>(x, y) {}
  Vector2(const glm::tvec2<T> &v) : glm::tvec2<T>(v) {}
};

template <typename T>
class Vector3 : public glm::tvec3<T> {
 public:
  Vector3() : glm::tvec3<T>() {}
  Vector3(T x, T y, T z) : glm::tvec3<T>(x, y, z) {}
  Vector3(const glm::tvec3<T> &v) : glm::tvec3<T>(v) {}
};

typedef glm::vec3 Vector3f;
typedef glm::vec2 Vector2f;
typedef glm::vec3 Vector3i;
typedef glm::vec2 Vector2i;

inline void CoordinateSystem(const glm::vec3 &v1, glm::vec3 *v2,
                             glm::vec3 *v3) {
  if (std::abs(v1.x) > std::abs(v1.y)) {
    *v2 = glm::vec3(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
  } else {
    *v2 = glm::vec3(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
  }
  *v3 = glm::cross(v1, *v2);
}

template <typename T>
class Point3 : public glm::tvec3<T> {
 public:
  Point3() : glm::tvec3<T>() {}
  Point3(T x, T y, T z) : glm::tvec3<T>(x, y, z) {}
  Point3(const glm::tvec3<T> &v) : glm::tvec3<T>(v) {}
};

template <typename T>
class Point2 : public glm::tvec2<T> {
 public:
  Point2() : glm::tvec2<T>() {}
  Point2(T x, T y) : glm::tvec2<T>(x, y) {}
  Point2(const glm::tvec2<T> &v) : glm::tvec2<T>(v) {}
  explicit Point2(const Point3<T> &p) : glm::tvec2<T>(p.x, p.y) {}
};

typedef Point2<Float> Point2f;
typedef Point2<int> Point2i;

typedef Point3<Float> Point3f;
typedef Point3<int> Point3i;

template <typename T>
Point2<T> Lerp(Float t, const Point2<T> &p0, const Point2<T> &p1) {
  return (1 - t) * p0 + t * p1;
}

template <typename T>
class Normal3 : glm::tvec3<T> {
 public:
  Normal3() : glm::tvec3<T>() {}
  Normal3(T x, T y, T z) : glm::tvec3<T>(x, y, z) {}
  Normal3(const glm::tvec3<T> &v) : glm::tvec3<T>(v) {}
  explicit Normal3(const Vector3<T> &v) : glm::tvec3<T>(v) {}
};

typedef Normal3<Float> Normal3f;

template <typename T>
inline Normal3<T> FaceForward(const Normal3<T> &n, const Vector3<T> &v) {
  return (glm::dot(n, v) < 0.f) ? -n : n;
}

// Forward declarations of medium

class Medium;

class Ray {
 public:
  Ray() : tMax(Infinity), time(0.f), medium(nullptr) {}
  Ray(const Point3f &o, const Vector3f &d, Float tMax = Infinity,
      Float time = 0.f, const Medium *medium = nullptr)
      : o(o), d(d), tMax(tMax), time(time), medium(medium) {}

  Point3f operator()(Float t) const { return o + d * t; }

 public:
  Point3f o;
  Vector3f d;
  mutable Float tMax;
  Float time;
  const Medium *medium;
};

class RayDifferential : public Ray {
 public:
  RayDifferential() { hasDifferentials = false; }
  RayDifferential(const Point3f &o, const Vector3f &d, Float tMax = Infinity,
                  Float time = 0.f, const Medium *medium = nullptr)
      : Ray(o, d, tMax, time, medium) {
    hasDifferentials = false;
  }
  RayDifferential(const Ray &ray) : Ray(ray) { hasDifferentials = false; }
  void ScaleDifferentials(Float s) {
    rxOrigin = o + (rxOrigin - o) * s;
    ryOrigin = o + (ryOrigin - o) * s;
    rxDirection = d + (rxDirection - d) * s;
    ryDirection = d + (ryDirection - d) * s;
  }

 public:
  bool hasDifferentials;
  Point3f rxOrigin, ryOrigin;
  Vector3f rxDirection, ryDirection;
};

#endif  // CORE_GEOMETRY_H
