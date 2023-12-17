#ifndef PHR_CORE_SHAPE_H
#define PHR_CORE_SHAPE_H

#include "geometry.h"
#include "transform.h"

class Shape {
 public:
  Shape(const Transform *objectToWorld, const Transform *worldToObject,
        bool reverseOrientation)
      : objectToWorld(objectToWorld),
        worldToObject(worldToObject),
        reverseOrientation(reverseOrientation),
        transformSwapsHandedness(objectToWorld->swapsHandedness()) {}

  virtual ~Shape() {}

  virtual Bounds3f ObjectBound() const = 0;

  Bounds3f WorldBound() const { return (*objectToWorld)(ObjectBound()); }

  virtual bool Intersect(const Ray &ray, Float *tHit, SurfaceInteraction *isect,
                         bool testAlphaTexture = true) const = 0;

  virtual bool IntersectP(const Ray &ray, bool testAlphaTexture = true) const {
    Float tHit = ray.tMax;
    SurfaceInteraction isect;
    return Intersect(ray, &tHit, &isect, testAlphaTexture);
  }

  virtual Float Area() const = 0;

 public:
  const Transform *objectToWorld, *worldToObject;
  const bool reverseOrientation;
  const bool transformSwapsHandedness;
};

#endif  // PHR_CORE_SHAPE_H
