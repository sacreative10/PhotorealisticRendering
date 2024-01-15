#ifndef PHR_ACCELERATORS_BVH_H
#define PHR_ACCELERATORS_BVH_H

#include <memory>
#include <vector>

#include "core/geometry.h"
#include "core/interaction.h"
#include "core/primitive.h"
#include "core/util/MemoryArena.h"
enum class BVHSplitMethod { SAH, HLBVH, Middle, EqualCounts };

struct BVHPrimitiveInfo;
struct BVHBuildNode;
struct LinearBVHNode;

class BVHAccelerator : public Aggregate {
 public:
  BVHAccelerator(const std::vector<std::shared_ptr<Primitive>> &primitives,
                 int maxPrimsInNode, BVHSplitMethod splitMethod);
  BVHBuildNode *recursiveBuild(
      MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo,
      int start, int end, int *totalNodes,
      std::vector<std::shared_ptr<Primitive>> orderedPrims);
  int flattenBVHTree(BVHBuildNode *node, int *offset);
  bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
  bool IntersectP(const Ray &ray) const;
  ~BVHAccelerator();

 private:
  const int maxPrimsInNode;
  const BVHSplitMethod splitMethod;
  std::vector<std::shared_ptr<Primitive>> primitives;
  LinearBVHNode *nodes = nullptr;
};

#endif  // PHR_ACCELERATORS_BVH_H
