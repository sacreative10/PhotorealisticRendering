#ifndef PHR_ACCELERATORS_BVH_H
#define PHR_ACCELERATORS_BVH_H

#include <memory>
#include <vector>

#include "core/primitive.h"
enum class BVHSplitMethod { SAH, HLBVH, Middle, EqualCounts };

struct BVHPrimitiveInfo;
struct BVHBuildNode;

class BVHAccelerator : public Aggregate {
 public:
  BVHAccelerator(const std::vector<std::shared_ptr<Primitive>> &primitives,
                 int maxPrimsInNode, BVHSplitMethod splitMethod);

 private:
  const int maxPrimsInNode;
  const BVHSplitMethod splitMethod;
  std::vector<std::shared_ptr<Primitive>> primitives;
};

#endif  // PHR_ACCELERATORS_BVH_H
