#include "shapes/sphere.h"

Bounds3f Sphere::objectBound() const {
  return Bounds3f(Point3f(-radius, -radius, zMin),
                  Point3f(radius, radius, zMax));
}

bool Sphere::intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect,
                       bool testAlphaTexture) const {
  Float phi;
  Vector3f oErr, dErr;
  Point3f pHit;
  Ray rayLocal = (*worldToObject)(ray, &oErr, &dErr);
  // TODO: Come back and add Rounding Error checks
  Float ox = rayLocal.o.x, oy = rayLocal.o.y, oz = rayLocal.o.z;
  Float dx = rayLocal.d.x, dy = rayLocal.d.y, dz = rayLocal.d.z;

  Float a = dx * dx + dy * dy + dz * dz;
  Float b = 2 * (dx * ox + dy * oy + dz * oz);
  Float c = ox * ox + oy * oy + oz * oz - radius * radius;

  Float t0, t1;
  if (!Quadratic(a, b, c, &t0, &t1)) return false;

  if (t0 > rayLocal.tMax || t1 <= 0) return false;

  Float tShapeHit = t0;
  if (tShapeHit <= 0) {
    tShapeHit = t1;
    if (tShapeHit > rayLocal.tMax) return false;
  }

  pHit = rayLocal((Float)tShapeHit);

  if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5f * radius;
  phi = std::atan2(pHit.y, pHit.x);
  if (phi < 0) phi += 2 * Pi;

  if ((zMin > -radius && pHit.z < zMin) || (zMax < radius && pHit.z > zMax) ||
      phi > phiMax) {
    if (tShapeHit == t1) return false;
    if (t1 > rayLocal.tMax) return false;
    tShapeHit = t1;
    pHit = rayLocal((Float)tShapeHit);
    if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5f * radius;
    phi = std::atan2(pHit.y, pHit.x);
    if (phi < 0) phi += 2 * Pi;
    if ((zMin > -radius && pHit.z < zMin) || (zMax < radius && pHit.z > zMax) ||
        phi > phiMax)
      return false;
  }

  Float u = phi / phiMax;
  Float theta = std::acos(Clamp(pHit.z / radius, -1, 1));
  Float v = (theta - thetaMin) / (thetaMax - thetaMin);

  Float zRadius = std::sqrt(pHit.x * pHit.x + pHit.y * pHit.y);
  Float invZRadius = 1 / zRadius;
  Float cosPhi = pHit.x * invZRadius;
  Float sinPhi = pHit.y * invZRadius;
  Vector3f dpdu(-phiMax * pHit.y, phiMax * pHit.x, 0);
  Vector3f dpdv =
      (thetaMax - thetaMin) *
      Vector3f(pHit.z * cosPhi, pHit.z * sinPhi, -radius * std::sin(theta));

  Vector3f d2Pduu = -phiMax * phiMax * Vector3f(pHit.x, pHit.y, 0);
  Vector3f d2Pduv =
      (thetaMax - thetaMin) * pHit.z * phiMax * Vector3f(-sinPhi, cosPhi, 0.);
  Vector3f d2Pdvv = -(thetaMax - thetaMin) * (thetaMax - thetaMin) *
                    Vector3f(pHit.x, pHit.y, pHit.z);

  Float E = Dot(dpdu, dpdu);
  Float F = Dot(dpdu, dpdv);
  Float G = Dot(dpdv, dpdv);
  Vector3f N = Normalize(Cross(dpdu, dpdv));
  Float e = Dot(N, d2Pduu);
  Float f = Dot(N, d2Pduv);
  Float g = Dot(N, d2Pdvv);

  Float invEGF2 = 1 / (E * G - F * F);
  Normal3f dndu = Normal3f((f * F - e * G) * invEGF2 * dpdu +
                           (e * F - f * E) * invEGF2 * dpdv);
  Normal3f dndv = Normal3f((g * F - f * G) * invEGF2 * dpdu +
                           (f * F - g * E) * invEGF2 * dpdv);

  // TODO: Compute error bounds for sphere intersection
  const Vector3f pError = Vector3f(0, 0, 0);

  *isect = (*objectToWorld)(SurfaceInteraction(pHit, pError, Point2f(u, v),
                                               -ray.d, dpdu, dpdv, dndu, dndv,
                                               ray.time, this));

  *tHit = (Float)tShapeHit;

  return true;
}

bool Sphere::intersectP(const Ray& ray, Float* tHit,
                        bool testAlphaTexture) const {
  Float phi;
  Vector3f oErr, dErr;
  Point3f pHit;
  Ray rayLocal = (*worldToObject)(ray, &oErr, &dErr);
  // TODO: Come back and add Rounding Error checks
  Float ox = rayLocal.o.x, oy = rayLocal.o.y, oz = rayLocal.o.z;
  Float dx = rayLocal.d.x, dy = rayLocal.d.y, dz = rayLocal.d.z;

  Float a = dx * dx + dy * dy + dz * dz;
  Float b = 2 * (dx * ox + dy * oy + dz * oz);
  Float c = ox * ox + oy * oy + oz * oz - radius * radius;

  Float t0, t1;
  if (!Quadratic(a, b, c, &t0, &t1)) return false;

  if (t0 > rayLocal.tMax || t1 <= 0) return false;

  Float tShapeHit = t0;
  if (tShapeHit <= 0) {
    tShapeHit = t1;
    if (tShapeHit > rayLocal.tMax) return false;
  }

  pHit = rayLocal((Float)tShapeHit);

  if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5f * radius;
  phi = std::atan2(pHit.y, pHit.x);
  if (phi < 0) phi += 2 * Pi;

  if ((zMin > -radius && pHit.z < zMin) || (zMax < radius && pHit.z > zMax) ||
      phi > phiMax) {
    if (tShapeHit == t1) return false;
    if (t1 > rayLocal.tMax) return false;
    tShapeHit = t1;
    pHit = rayLocal((Float)tShapeHit);
    if (pHit.x == 0 && pHit.y == 0) pHit.x = 1e-5f * radius;
    phi = std::atan2(pHit.y, pHit.x);
    if (phi < 0) phi += 2 * Pi;
    if ((zMin > -radius && pHit.z < zMin) || (zMax < radius && pHit.z > zMax) ||
        phi > phiMax)
      return false;
  }

  *tHit = (Float)tShapeHit;

  return true;
}

Float Sphere::Area() const { return phiMax * radius * (zMax - zMin); }
