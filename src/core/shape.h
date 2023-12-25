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

  virtual Bounds3f objectBound() const = 0;

  Bounds3f worldBound() const { return (*objectToWorld)(objectBound()); }

  virtual bool intersect(const Ray &ray, Float *tHit, SurfaceInteraction *isect,
                         bool testAlphaTexture = true) const = 0;

  virtual bool intersectP(const Ray &ray, Float *tHit,
                          bool testAlphaTexture = true) const {
    Float tHit = ray.tMax;
    SurfaceInteraction isect;
    return intersect(ray, &tHit, &isect, testAlphaTexture);
  }

  virtual Float Area() const = 0;

 public:
  const Transform *objectToWorld, *worldToObject;
  const bool reverseOrientation;
  const bool transformSwapsHandedness;
};

#endif  // PHR_CORE_SHAPE_H
