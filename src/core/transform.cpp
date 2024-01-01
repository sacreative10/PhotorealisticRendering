
#include "transform.h"

#include "geometry.h"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"
#include "interaction.h"

Transform Translate(const Vector3f &delta) {
  glm::mat4 m(1.f);
  m[3][0] = delta.x;
  m[3][1] = delta.y;
  m[3][2] = delta.z;
  return Transform(m, glm::transpose(m));
}

Transform Scale(Float x, Float y, Float z) {
  glm::mat4 m(1.f);
  m[0][0] = x;
  m[1][1] = y;
  m[2][2] = z;
  return Transform(m, glm::transpose(m));
}

Transform RotateX(Float theta) {
  Float sinTheta = glm::sin(glm::radians(theta));
  Float cosTheta = glm::cos(glm::radians(theta));
  glm::mat4 mat(1, 0, 0, 0, 0, cosTheta, -sinTheta, 0, 0, sinTheta, cosTheta, 0,
                0, 0, 0, 0);
  return Transform(mat, glm::transpose(mat));
}
Transform RotateY(Float theta) {
  Float sinTheta = glm::sin(glm::radians(theta));
  Float cosTheta = glm::cos(glm::radians(theta));
  glm::mat4 mat(cosTheta, 0, sinTheta, 0, 0, 1, 0, 0, -sinTheta, 0, cosTheta, 0,
                0, 0, 0, 1);
  return Transform(mat, glm::transpose(mat));
}

Transform RotateZ(Float theta) {
  Float sinTheta = glm::sin(glm::radians(theta));
  Float cosTheta = glm::cos(glm::radians(theta));
  glm::mat4 mat(cosTheta, -sinTheta, 0, 0, sinTheta, cosTheta, 0, 0, 0, 0, 1, 0,
                0, 0, 0, 1);
  return Transform(mat, glm::transpose(mat));
}

Transform Rotate(Float theta, const Vector3f &axis) {
  Vector3f a = glm::normalize(axis);
  Float sinTheta = glm::sin(glm::radians(theta));
  Float cosTheta = glm::cos(glm::radians(theta));
  glm::mat4 m;
  m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
  m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
  m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
  m[0][3] = 0;
  m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
  m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
  m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
  m[1][3] = 0;

  m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
  m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
  m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
  m[2][3] = 0;

  return Transform(m, glm::transpose(m));
}

Transform LookAt(const Point3f &pos, const Point3f &look, const Vector3f &up) {
  glm::mat4 cameraToWorld;
  cameraToWorld[0][3] = pos.x;
  cameraToWorld[1][3] = pos.y;
  cameraToWorld[2][3] = pos.z;
  cameraToWorld[3][3] = 1;

  Vector3f dir = glm::normalize(look - pos);
  Vector3f right = glm::normalize(glm::cross(glm::normalize(up), dir));
  Vector3f newUp = glm::cross(dir, right);
  cameraToWorld[0][0] = right.x;
  cameraToWorld[1][0] = right.y;
  cameraToWorld[2][0] = right.z;
  cameraToWorld[3][0] = 0.;
  cameraToWorld[0][1] = newUp.x;
  cameraToWorld[1][1] = newUp.y;
  cameraToWorld[2][1] = newUp.z;
  cameraToWorld[3][1] = 0.;
  cameraToWorld[0][2] = dir.x;
  cameraToWorld[1][2] = dir.y;
  cameraToWorld[2][2] = dir.z;
  cameraToWorld[3][2] = 0.;

  return Transform(glm::inverse(cameraToWorld), cameraToWorld);
}

template <typename T>
inline Point3<T> Transform::operator()(const Point3<T> &p) const {
  T x = p.x, y = p.y, z = p.z;
  T xp = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3];
  T yp = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3];
  T zp = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3];
  T wp = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3];
  if (wp == 1)
    return Point3<T>(xp, yp, zp);
  else
    return Point3<T>(xp, yp, zp) / wp;
}

// template <typename T>
// inline Point3<T> Transform::operator()(const Point3<T> &p,
//                                        Vector3f *pError) const {
//   T x = p.x, y = p.y, z = p.z;
//   T xp = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3];
//   T yp = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3];
//   T zp = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3];
//   T wp = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3];
//   // TODO Compute transformed error bounds for transformed vector
//   *pError =
//       Vector3f(glm::abs((m[0][0] + m[0][3]) * x + (m[0][1] + m[0][3]) * y +
//                         (m[0][2] + m[0][3]) * z),
//                glm::abs((m[1][0] + m[1][3]) * x + (m[1][1] + m[1][3]) * y +
//                         (m[1][2] + m[1][3]) * z),
//                glm::abs((m[2][0] + m[2][3]) * x + (m[2][1] + m[2][3]) * y +
//                         (m[2][2] + m[2][3]) * z));
//   if (wp == 1)
//     return Point3<T>(xp, yp, zp);
//   else
//     return Point3<T>(xp, yp, zp) / wp;
// }

template <typename T>
inline Vector3<T> Transform::operator()(const Vector3<T> &v) const {
  T x = v.x, y = v.y, z = v.z;
  return Vector3<T>(m[0][0] * x + m[0][1] * y + m[0][2] * z,
                    m[1][0] * x + m[1][1] * y + m[1][2] * z,
                    m[2][0] * x + m[2][1] * y + m[2][2] * z);
}

// template <typename T>
// inline Vector3<T> Transform::operator()(const Vector3<T> &v,
//                                         Vector3f *vError) const {
//   T x = v.x, y = v.y, z = v.z;
//   // TODO Compute transformed error bounds for transformed vector
//   *vError = Vector3f(glm::abs(m[0][0] * x + m[0][1] * y + m[0][2] * z),
//                      glm::abs(m[1][0] * x + m[1][1] * y + m[1][2] * z),
//                      glm::abs(m[2][0] * x + m[2][1] * y + m[2][2] * z));
//   return Vector3<T>(m[0][0] * x + m[0][1] * y + m[0][2] * z,
//                     m[1][0] * x + m[1][1] * y + m[1][2] * z,
//                     m[2][0] * x + m[2][1] * y + m[2][2] * z);
// }

template <typename T>
inline Normal3<T> Transform::operator()(const Normal3<T> &n) const {
  T x = n.x, y = n.y, z = n.z;
  return Normal3<T>(mInv[0][0] * x + mInv[1][0] * y + mInv[2][0] * z,
                    mInv[0][1] * x + mInv[1][1] * y + mInv[2][1] * z,
                    mInv[0][2] * x + mInv[1][2] * y + mInv[2][2] * z);
}

inline Ray Transform::operator()(const Ray &r) const {
  Point3f o = (*this)(r.o);
  Vector3f d = (*this)(r.d);
  Float lengthSqr = d.lengthSquared();
  Float tmax = r.tMax;
  if (lengthSqr > 0) {
    Float dt = glm::dot(Abs(d), r.tMax * d / glm::sqrt(lengthSqr));
    // HACK: should be +=, but the way i have implemented it makes it not
    // possible.
    o = o + d * dt;
    tmax -= dt;
  }

  return Ray(o, d, tmax, r.time, r.medium);
}

// inline Ray Transform::operator()(const Ray &r, Vector3f *oError,
//                                  Vector3f *dError) const {
//   Point3f o = (*this)(r.o, oError);
//   Vector3f d = (*this)(r.d, dError);
//   // TODO Offset ray origin
//   return Ray(o, d, r.tMax, r.time, r.medium);
// }

// inline RayDifferential Transform::operator()(const RayDifferential &r) const
// {
//   Ray tr = (*this)(Ray(r));
//   RayDifferential ret(tr.o, tr.d, tr.tMax, tr.time, tr.medium);
//   ret.hasDifferentials = r.hasDifferentials;
//   ret.rxOrigin = (*this)(r.rxOrigin);
//   ret.ryOrigin = (*this)(r.ryOrigin);
//   ret.rxDirection = (*this)(r.rxDirection);
//   ret.ryDirection = (*this)(r.ryDirection);
//   return ret;
// }

Bounds3f Transform::operator()(const Bounds3f &b) const {
  const Transform &M = *this;
  Bounds3f ret(M(Point3f(b.pMin.x, b.pMin.y, b.pMin.z)));
  ret = Union(ret, M(Point3f(b.pMax.x, b.pMin.y, b.pMin.z)));
  ret = Union(ret, M(Point3f(b.pMin.x, b.pMax.y, b.pMin.z)));
  ret = Union(ret, M(Point3f(b.pMin.x, b.pMin.y, b.pMax.z)));
  ret = Union(ret, M(Point3f(b.pMin.x, b.pMax.y, b.pMax.z)));
  ret = Union(ret, M(Point3f(b.pMax.x, b.pMax.y, b.pMin.z)));
  ret = Union(ret, M(Point3f(b.pMax.x, b.pMin.y, b.pMax.z)));
  ret = Union(ret, M(Point3f(b.pMax.x, b.pMax.y, b.pMax.z)));
  return ret;
}

SurfaceInteraction Transform::operator()(const SurfaceInteraction &si) const {
  SurfaceInteraction ret;
  ret.p = (*this)(si.p, &ret.pError);

  const Transform &t = *this;
  ret.n = glm::normalize(t(si.n));
  ret.wo = glm::normalize(t(si.wo));
  ret.time = si.time;
  ret.mediumInterface = si.mediumInterface;
  ret.uv = si.uv;
  ret.shape = si.shape;
  ret.dndu = si.dndu;
  ret.dndv = si.dndv;
  ret.dpdu = t(si.dpdu);
  ret.dpdv = t(si.dpdv);
  ret.shading.n = glm::normalize(t(si.shading.n));
  ret.shading.dpdu = t(si.shading.dpdu);
  ret.shading.dpdv = t(si.shading.dpdv);
  ret.shading.dndu = t(si.shading.dndu);
  ret.shading.dndu = t(si.shading.dndv);
  ret.shading.n = glm::normalize(t(si.shading.n));
  ret.shading.n = FaceForward(ret.shading.n, ret.n);

  return ret;
}
Transform Transform::operator*(const Transform &t2) const {
  glm::mat4 m1 = m * t2.m;
  glm::mat4 m2 = t2.mInv * mInv;
  return Transform(m1, m2);
}

bool Transform::swapsHandedness() const {
  Float det = glm::determinant(m);
  return det < 0;
}
