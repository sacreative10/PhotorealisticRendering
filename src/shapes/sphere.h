#ifndef PHR_SHAPES_SPHERE_H
#define PHR_SHAPES_SPHERE_H
#include "core/phr.h"
#include "core/shape.h"
#include "core/transform.h"

class Sphere : public Shape {
 public:
  Sphere(const Transform* o2w, const Transform* w2o, bool ro, Float rad,
         Float z0, Float z1, Float pm)
      : Shape(o2w, w2o, ro),
        radius(rad),
        zMin(Clamp(std::min(z0, z1), -rad, rad)),
        zMax(Clamp(std::max(z0, z1), -rad, rad)),
        thetaMin(std::acos(Clamp(zMin / rad, -1, 1))),
        thetaMax(std::acos(Clamp(zMax / rad, -1, 1))) {}

  Bounds3f objectBound() const override;
  bool intersect(const Ray& ray, Float* tHit, SurfaceInteraction* isect,
                 bool testAlphaTexture) const override;
  bool intersectP(const Ray& ray, Float* tHit,
                  bool testAlphaTexture) const override;

  Float Area() const override;

 private:
  const Float radius;
  const Float zMin, zMax;
  const Float thetaMin, thetaMax;
  const Float phiMax = 2 * Pi;
};

#endif  // PHR_SHAPES_SPHERE_H