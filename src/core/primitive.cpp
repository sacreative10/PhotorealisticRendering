#include "core/primitive.h"

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
