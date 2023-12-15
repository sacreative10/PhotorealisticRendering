
#include "transform.h"

#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"

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
