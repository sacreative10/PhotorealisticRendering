#include "core/primitive.h"

#include <cstdio>
#include <stdexcept>

#include "core/geometry.h"

// TODO: Come back for MemoryArena
GeometricPrimitive::GeometricPrimitive(std::shared_ptr<Shape> shape,
                                       std::shared_ptr<Material> material,
                                       std::shared_ptr<AreaLight> areaLight)
    : shape(shape), material(material), areaLight(areaLight) {}

bool GeometricPrimitive::Intersect(const Ray& r,
                                   SurfaceInteraction* isect) const {
  Float tHit;
  if (!shape->intersect(r, &tHit, isect)) return false;
  r.tMax = tHit;
  isect->primitive = this;
  // TODO: Add code to set medium
  return true;
}

bool GeometricPrimitive::IntersectP(const Ray& r) const {
  return shape->intersectP(r);
}

const AreaLight* GeometricPrimitive::GetAreaLight() const {
  return areaLight.get();
}

void GeometricPrimitive::computeScatterFunctions(
    SurfaceInteraction* isect, MemoryArena& arena, TransportMode mode,
    bool allowMultipleLobes) const {}

Bounds3f GeometricPrimitive::WorldBound() const { return shape->worldBound(); }

const AreaLight* Aggregate::GetAreaLight() const {
  throw std::runtime_error(
      "Aggregate::GetAreaLight() called. This is an error.\n");
}

const Material* Aggregate::GetMaterial() const {
  throw std::runtime_error(
      "Aggregate::GetMaterial() called. This is an error.\n");
}

void Aggregate::computeScatterFunctions(SurfaceInteraction* isect,
                                        MemoryArena& arena, TransportMode mode,
                                        bool allowMultipleLobes) const {
  throw std::runtime_error(
      "Aggregate::computeScatterFunctions() called. This is an error.\n");
}
