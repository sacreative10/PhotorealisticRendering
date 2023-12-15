#ifndef PHR_CORE_TRANSFORM_H
#define PHR_CORE_TRANSFORM_H

#include "geometry.h"
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
    Float la2 = glm::length2(Vector3f(m[0][0], m[0][1], m[0][2]));
    Float lb2 = glm::length2(Vector3f(m[1][0], m[1][1], m[1][2]));
    Float lc2 = glm::length2(Vector3f(m[2][0], m[2][1], m[2][2]));
#define NOT_ONE(x) ((x) < 0.999f || (x) > 1.001f)
    return (NOT_ONE(la2) || NOT_ONE(lb2) || NOT_ONE(lc2));
#undef NOT_ONE
  }

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
