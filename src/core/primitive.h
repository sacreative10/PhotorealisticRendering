
#ifndef CORE_PRIMITIVE_H
#define CORE_PRIMITIVE_H

#include <memory>

#include "core/geometry.h"
#include "core/interaction.h"
#include "core/shape.h"

class AreaLight;
class Material;
class MemoryArena;
class TransportMode {};

class Primitive {
 public:
  virtual Bounds3f WorldBound() const = 0;
  virtual bool Intersect(const Ray& r, SurfaceInteraction*) const = 0;
  virtual bool IntersectP(const Ray& r) const = 0;
  virtual void computeScatterFunctions(SurfaceInteraction* isect,
                                       MemoryArena& arena, TransportMode mode,
                                       bool allowMultipleLobes) const = 0;

  virtual const AreaLight* GetAreaLight() const = 0;

 public:
  const AreaLight* areaLight = nullptr;
  const Material* material = nullptr;
};

class GeometricPrimitive : public Primitive {
 public:
  GeometricPrimitive(std::shared_ptr<Shape> shape,
                     std::shared_ptr<Material> material,
                     std::shared_ptr<AreaLight> areaLight);
  bool Intersect(const Ray& r, SurfaceInteraction* isect) const override;
  bool IntersectP(const Ray& r) const override;
  const AreaLight* GetAreaLight()
      const override;  // TODO: Needs to be implemented (AreaLight)
  // TODO: This is not implemented (computeScatterFunctions)
  void computeScatterFunctions(SurfaceInteraction* isect, MemoryArena& arena,
                               TransportMode mode,
                               bool allowMultipleLobes) const override;
  Bounds3f WorldBound() const override;

 private:
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Material> material;
  std::shared_ptr<AreaLight> areaLight;
};

// TODO: Need to write TransformedPrimitive

// TODO: Cause an error, when any of the class functions are called.
class Aggregate : public Primitive {
 public:
  const AreaLight* GetAreaLight() const override;
  const Material* GetMaterial() const;
  void computeScatterFunctions(SurfaceInteraction* isect, MemoryArena& arena,
                               TransportMode mode,
                               bool allowMultipleLobes) const override;
};

#endif  // CORE_PRIMITIVE_H
