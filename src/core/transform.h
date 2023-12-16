#ifndef PHR_CORE_TRANSFORM_H
#define PHR_CORE_TRANSFORM_H

#include "geometry.h"
#include "interaction.h"
#include "phr.h"

class Transform {
 public:
  Transform() {}
  Transform(const Float mat[4][4]) {
    m = glm::mat4(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0],
                  mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1],
                  mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2],
                  mat[3][3]);
    mInv = glm::inverse(m);
  }
  Transform(const glm::mat4 &m, const glm::mat4 &mInv) : m(m), mInv(mInv) {}
  friend Transform Inverse(const Transform &t) {
    return Transform(t.mInv, t.m);
  }
  friend Transform Transpose(const Transform &t) {
    return Transform(glm::transpose(t.m), glm::transpose(t.mInv));
  }
  bool isEqual(const Transform &t) const {
    return (t.m == m && t.mInv == mInv);
  }
  bool isIdentity() const { return (m == glm::mat4(1.f)); }

  bool hasScale() const {
    Float la2 = (*this)(Vector3f(1, 0, 0)).lengthSquared();
    Float lb2 = (*this)(Vector3f(0, 1, 0)).lengthSquared();
    Float lc2 = (*this)(Vector3f(0, 0, 1)).lengthSquared();
#define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
    return (NOT_ONE(la2) || NOT_ONE(lb2) || NOT_ONE(lc2));
#undef NOT_ONE
  }
  template <typename T>
  inline Point3<T> operator()(const Point3<T> &p) const;

  template <typename T>
  inline Vector3<T> operator()(const Vector3<T> &v) const;

  template <typename T>
  inline Normal3<T> operator()(const Normal3<T> &n) const;

  inline Ray operator()(const Ray &r) const;

  inline RayDifferential operator()(const RayDifferential &r) const;

  Bounds3f operator()(const Bounds3f &b) const;

  Transform operator*(const Transform &t2) const;
  SurfaceInteraction operator()(const SurfaceInteraction &si) const;

  bool swapsHandedness() const;

 private:
  glm::mat4 m, mInv;
};

Transform Translate(const Vector3f &delta);
Transform Scale(Float x, Float y, Float z);
Transform RotateX(Float theta);
Transform RotateY(Float theta);
Transform RotateZ(Float theta);
Transform Rotate(Float theta, const Vector3f &axis);
Transform LookAt(const Point3f &pos, const Point3f &look, const Vector3f &up);

#endif
