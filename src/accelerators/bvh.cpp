#include "accelerators/bvh.h"

#include "core/geometry.h"
#include "core/primitive.h"
#include "core/util/MemoryArena.h"

struct BVHPrimitiveInfo {
  BVHPrimitiveInfo() {}
  BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f &bounds)
      : primitiveNumber(primitiveNumber),
        bounds(bounds),
        centroid(.5f * bounds.pMin + .5f * bounds.pMax) {}
  int primitiveNumber;
  Bounds3f bounds;
  Point3f centroid;
};

struct BVHBuildNode {
  void InitLeaf(int first, int n, const Bounds3f &b) {
    firstPrimOffset = first;
    nPrimitives = n;
    bounds = b;
    children[0] = children[1] = nullptr;
  }
  void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
    children[0] = c0;
    children[1] = c1;
    bounds = Union(c0->bounds, c1->bounds);
    splitAxis = axis;
    nPrimitives = 0;
  }
  Bounds3f bounds;
  BVHBuildNode *children[2];
  int splitAxis, firstPrimOffset, nPrimitives;
};

BVHAccelerator::BVHAccelerator(
    const std::vector<std::shared_ptr<Primitive>> &primitives,
    int maxPrimsInNode, BVHSplitMethod splitMethod)
    : maxPrimsInNode(maxPrimsInNode),
      splitMethod(splitMethod),
      primitives(primitives) {
  if (primitives.size() == 0) return;
  std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
  for (size_t i = 0; i < primitives.size(); ++i) {
    primitiveInfo[i] = {i, primitives[i]->WorldBound()};
  }

  MemoryArena arena(1024 * 1024);
  int totalNodes = 0;
  std::vector<std::shared_ptr<Primitive>> orderedPrims;
}
