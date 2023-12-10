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

template <typename T>
class Bounds2 {
 public:
  Bounds2() {
    T minNum = std::numeric_limits<T>::lowest();
    T maxNum = std::numeric_limits<T>::max();
    pMin = Point2<T>(maxNum, maxNum);
    pMax = Point2<T>(minNum, minNum);
  }
  Bounds2(Point2<T> &p) : pMin(p), pMax(p) {}
  Bounds2(const Point2<T> &p1, const Point2<T> &p2) {
    pMin = Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
    pMax = Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
  }
  const Point2<T> &operator[](int i) const;
  Point2<T> &operator[](int i);
  Point2<T> Corner(int corner) const {
    return Point2<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y);
  }

  Vector2<T> Diagonal() const { return pMax - pMin; }

  T SurfaceArea() const {
    Vector2<T> d = Diagonal();
    return d.x * d.y;
  }

  int MaximumExtent() const {
    Vector2<T> d = Diagonal();
    if (d.x > d.y) {
      return 0;
    } else {
      return 1;
    }
  }

  Point2<T> Lerp(const Point2f &t) const {
    return Point2<T>(::Lerp(t.x, pMin.x, pMax.x), ::Lerp(t.y, pMin.y, pMax.y));
  }

  Vector2<T> Offset(const Point2<T> &p) const {
    Vector2<T> o = p - pMin;
    if (pMax.x > pMin.x) {
      o.x /= pMax.x - pMin.x;
    }
    if (pMax.y > pMin.y) {
      o.y /= pMax.y - pMin.y;
    }
    return o;
  }

  void boundingCircle(Point2<T> *center, Float *radius) const {
    *center = (pMin + pMax) / 2;
    *radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
  }

 public:
  Point2<T> pMin, pMax;
};

// copy over Bounds2 into Bounds3
template <typename T>
class Bounds3 {
 public:
  Bounds3() {
    T minNum = std::numeric_limits<T>::lowest();
    T maxNum = std::numeric_limits<T>::max();
    pMin = Point3<T>(maxNum, maxNum, maxNum);
    pMax = Point3<T>(minNum, minNum, minNum);
  }
  Bounds3(Point3<T> &p) : pMin(p), pMax(p) {}
  Bounds3(const Point3<T> &p1, const Point3<T> &p2) {
    pMin = Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                     std::min(p1.z, p2.z));
    pMax = Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                     std::max(p1.z, p2.z));
  }
  const Point3<T> &operator[](int i) const;
  Point3<T> &operator[](int i);
  Point3<T> Corner(int corner) const {
    return Point3<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y,
                     (*this)[(corner & 4) ? 1 : 0].z);
  }
  Vector3<T> Diagonal() const { return pMax - pMin; }

  T SurfaceArea() const {
    Vector3<T> d = Diagonal();
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
  }

  T Volume() const {
    Vector3<T> d = Diagonal();
    return d.x * d.y * d.z;
  }

  int MaximumExtent() const {
    Vector3<T> d = Diagonal();
    if (d.x > d.y && d.x > d.z) {
      return 0;
    } else if (d.y > d.z) {
      return 1;
    } else {
      return 2;
    }
  }

  Point3<T> Lerp(const Point3f &t) const {
    return Point3<T>(::Lerp(t.x, pMin.x, pMax.x), ::Lerp(t.y, pMin.y, pMax.y),
                     ::Lerp(t.z, pMin.z, pMax.z));
  }

  Vector3<T> Offset(const Point3<T> &p) const {
    Vector3<T> o = p - pMin;
    if (pMax.x > pMin.x) {
      o.x /= pMax.x - pMin.x;
    }
    if (pMax.y > pMin.y) {
      o.y /= pMax.y - pMin.y;
    }
    if (pMax.z > pMin.z) {
      o.z /= pMax.z - pMin.z;
    }
    return o;
  }

  void BoundingSphere(Point3<T> *center, Float *radius) const {
    *center = (pMin + pMax) / 2;
    *radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
  }

 public:
  Point3<T> pMin, pMax;
};

template <typename T>
Bounds2<T> Union(const Bounds2<T> &b, const Point2<T> &p) {
  return Bounds2<T>(
      Point2<T>(glm::min(b.pMin.x, p.x), glm::min(b.pMin.y, p.y)),
      Point2<T>(glm::max(b.pMax.x, p.x), glm::max(b.pMax.y, p.y)));
}

template <typename T>
Bounds3<T> Union(const Bounds3<T> &b, const Point3<T> &p) {
  return Bounds3<T>(Point3<T>(glm::min(b.pMin.x, p.x), glm::min(b.pMin.y, p.y),
                              glm::min(b.pMin.z, p.z)),
                    Point3<T>(glm::max(b.pMax.x, p.x), glm::max(b.pMax.y, p.y),
                              glm::max(b.pMax.z, p.z)));
}

template <typename T>
Bounds2<T> Union(const Bounds2<T> &b1, const Bounds2<T> &b2) {
  return Bounds2<T>(
      Point2<T>(std::min(b1.pMin.x, b2.pMin.x), std::min(b1.pMin.y, b2.pMin.y)),
      Point2<T>(std::max(b1.pMax.x, b2.pMax.x),
                std::max(b1.pMax.y, b2.pMax.y)));
}

template <typename T>
Bounds3<T> Union(const Bounds3<T> &b1, const Bounds3<T> &b2) {
  return Bounds3<T>(
      Point3<T>(std::min(b1.pMin.x, b2.pMin.x), std::min(b1.pMin.y, b2.pMin.y),
                std::min(b1.pMin.z, b2.pMin.z)),
      Point3<T>(std::max(b1.pMax.x, b2.pMax.x), std::max(b1.pMax.y, b2.pMax.y),
                std::max(b1.pMax.z, b2.pMax.z)));
}

template <typename T>
Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T> &b2) {
  return Bounds2<T>(
      Point2<T>(std::max(b1.pMin.x, b2.pMin.x), std::max(b1.pMin.y, b2.pMin.y)),
      Point2<T>(std::min(b1.pMax.x, b2.pMax.x),
                std::min(b1.pMax.y, b2.pMax.y)));
}

template <typename T>
Bounds3<T> Intersect(const Bounds3<T> &b1, const Bounds3<T> &b2) {
  return Bounds3<T>(
      Point3<T>(std::max(b1.pMin.x, b2.pMin.x), std::max(b1.pMin.y, b2.pMin.y),
                std::max(b1.pMin.z, b2.pMin.z)),
      Point3<T>(std::min(b1.pMax.x, b2.pMax.x), std::min(b1.pMax.y, b2.pMax.y),
                std::min(b1.pMax.z, b2.pMax.z)));
}

template <typename T>
Bounds2<T> Overlaps(const Bounds2<T> &b1, const Bounds2<T> &b2) {
  bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
  bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
  return (x && y);
}

template <typename T>
Bounds3<T> Overlaps(const Bounds3<T> &b1, const Bounds3<T> &b2) {
  bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
  bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
  bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
  return (x && y && z);
}

template <typename T>
bool Inside(const Point2<T> &p, const Bounds2<T> &b) {
  return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
          p.y <= b.pMax.y);
}

template <typename T>
bool Inside(const Point3<T> &p, const Bounds3<T> &b) {
  return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
          p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
}

template <typename T>
bool InsideExclusive(const Point2<T> &p, const Bounds2<T> &b) {
  return (p.x >= b.pMin.x && p.x < b.pMax.x && p.y >= b.pMin.y &&
          p.y < b.pMax.y);
}

template <typename T>
bool InsideExclusive(const Point3<T> &p, const Bounds3<T> &b) {
  return (p.x >= b.pMin.x && p.x < b.pMax.x && p.y >= b.pMin.y &&
          p.y < b.pMax.y && p.z >= b.pMin.z && p.z < b.pMax.z);
}

template <typename T, typename U>
inline Bounds2<T> Expand(const Bounds2<T> &b, U delta) {
  return Bounds2<T>(b.pMin - Vector2<T>(delta, delta),
                    b.pMax + Vector2<T>(delta, delta));
}

template <typename T, typename U>
inline Bounds3<T> Expand(const Bounds3<T> &b, U delta) {
  return Bounds3<T>(b.pMin - Vector3<T>(delta, delta, delta),
                    b.pMax + Vector3<T>(delta, delta, delta));
}

#endif  // CORE_GEOMETRY_H
