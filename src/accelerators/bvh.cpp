#include "accelerators/bvh.h"

#include <algorithm>
#include <cstdint>

#include "core/AllocAligned.h"
#include "core/geometry.h"
#include "core/phr.h"
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

struct LinearBVHNode {
  Bounds3f bounds;
  union {
    int primitivesOffset;
    int secondChildOffset;
  };
  uint16_t nPrimitives;
  uint8_t axis;
  uint8_t pad[1];
};

BVHBuildNode *BVHAccelerator::recursiveBuild(
    MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo, int start,
    int end, int *totalNodes,
    std::vector<std::shared_ptr<Primitive>> orderedPrims) {
  BVHBuildNode *node = arena.alloc<BVHBuildNode>();
  (*totalNodes)++;
  Bounds3f bounds;
  for (int i = start; i < end; ++i)
    bounds = Union(bounds, primitiveInfo[i].bounds);
  int nPrimitives = end - start;
  if (nPrimitives == 1) {
    int firstPrimOffset = orderedPrims.size();
    for (int i = start; i < end; ++i) {
      int primNum = primitiveInfo[i].primitiveNumber;
      orderedPrims.push_back(primitives[primNum]);
    }
    node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
    return node;
  } else {
    Bounds3f centroidBounds;
    for (int i = start; i < end; ++i)
      centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
    int dim = centroidBounds.MaximumExtent();
    int mid = (start + end) / 2;
    if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim]) {
      int firstPrimOffset = orderedPrims.size();
      for (int i = start; i < end; ++i) {
        int primNum = primitiveInfo[i].primitiveNumber;
        orderedPrims.push_back(primitives[primNum]);
      }
      node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
      return node;
    } else {
      // TODO Add partitioning scheme
      switch (splitMethod) {
        case BVHSplitMethod::Middle: {
          Float pmid =
              (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]) / 2;
          BVHPrimitiveInfo *midPtr =
              std::partition(&primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                             [dim, pmid](const BVHPrimitiveInfo &pi) {
                               return pi.centroid[dim] < pmid;
                             });
          mid = midPtr - &primitiveInfo[0];
          if (mid != start && mid != end) break;
        };
        case BVHSplitMethod::EqualCounts: {
          mid = (start + end) / 2;
          std::nth_element(
              &primitiveInfo[start], &primitiveInfo[mid],
              &primitiveInfo[end - 1] + 1,
              [dim](const BVHPrimitiveInfo &a, const BVHPrimitiveInfo &b) {
                return a.centroid[dim] < b.centroid[dim];
              });
        };
        case BVHSplitMethod::SAH:
        default: {
          if (nPrimitives <= 4) {
            mid = (start + end) / 2;
            std::nth_element(
                &primitiveInfo[start], &primitiveInfo[mid],
                &primitiveInfo[end - 1] + 1,
                [dim](const BVHPrimitiveInfo &a, const BVHPrimitiveInfo &b) {
                  return a.centroid[dim] < b.centroid[dim];
                });
          } else {
            // Actually do SAH
            constexpr int nBuckets = 12;
            struct BucketInfo {
              int count = 0;
              Bounds3f bounds;
            };
            BucketInfo buckets[nBuckets];
            for (int i = start; i < end; ++i) {
              int b = nBuckets *
                      centroidBounds.Offset(primitiveInfo[i].centroid)[dim];
              if (b == nBuckets) b = nBuckets - 1;
              buckets[b].count++;
              buckets[b].bounds =
                  Union(buckets[b].bounds, primitiveInfo[i].bounds);
            }
            // implementation is O(n^2), doesn't really matter now, but could
            // be a ineffciency later
            Float cost[nBuckets - 1];
            for (int i = 0; i < nBuckets - 1; ++i) {
              Bounds3f b0, b1;
              int count0 = 0, count1 = 0;
              for (int j = 0; j <= 1; ++j) {
                b0 = Union(b0, buckets[j].bounds);
                count0 += buckets[j].count;
              }
              for (int j = 0; j < nBuckets - 1; ++j) {
                b1 = Union(b1, buckets[j].bounds);
                count1 += buckets[j].count;
              }
              cost[i] = .125f + (count0 * b0.SurfaceArea() +
                                 count1 * b1.SurfaceArea()) /
                                    bounds.SurfaceArea();
            }
            Float minCost = cost[0];
            int minCostSplitBucket = 0;
            for (int i = 1; i < nBuckets - 1; ++i) {
              if (cost[i] < minCost) {
                minCost = cost[i];
                minCostSplitBucket = i;
              }
            }
            Float leafCost = nPrimitives;
            if (nPrimitives > maxPrimsInNode || minCost < leafCost) {
              BVHPrimitiveInfo *pmid = std::partition(
                  &primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                  [=](const BVHPrimitiveInfo &pi) {
                    int b = nBuckets * centroidBounds.Offset(pi.centroid)[dim];
                    if (b == nBuckets) b = nBuckets - 1;
                    return b <= minCostSplitBucket;
                  });
              mid = pmid - &primitiveInfo[0];
            } else {
              int firstPrimOffset = orderedPrims.size();
              for (int i = start; i < end; ++i) {
                int primNum = primitiveInfo[i].primitiveNumber;
                orderedPrims.push_back(primitives[primNum]);
              }
              node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
              return node;
            }
          }
        }
      }
      node->InitInterior(dim,
                         recursiveBuild(arena, primitiveInfo, start, mid,
                                        totalNodes, orderedPrims),
                         recursiveBuild(arena, primitiveInfo, mid, end,
                                        totalNodes, orderedPrims));
    }
  }
  return node;
}

int BVHAccelerator::flattenBVHTree(BVHBuildNode *node, int *offset) {
  LinearBVHNode *linearNode = &nodes[*offset];
  linearNode->bounds = node->bounds;
  int myOffset = (*offset)++;
  if (node->nPrimitives > 0) {
    linearNode->primitivesOffset = node->firstPrimOffset;
    linearNode->nPrimitives = node->nPrimitives;
  } else {
    linearNode->axis = node->splitAxis;
    linearNode->nPrimitives = 0;
    linearNode->secondChildOffset = flattenBVHTree(node->children[1], offset);
  }
}

BVHAccelerator::BVHAccelerator(
    const std::vector<std::shared_ptr<Primitive>> &primitives,
    int maxPrimsInNode, BVHSplitMethod splitMethod)
    : maxPrimsInNode(maxPrimsInNode),
      splitMethod(splitMethod),
      primitives(primitives) {
  if (primitives.empty()) return;
  std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
  for (size_t i = 0; i < primitives.size(); ++i) {
    primitiveInfo[i] = {i, primitives[i]->WorldBound()};
  }

  MemoryArena arena(1024 * 1024);
  int totalNodes = 0;
  std::vector<std::shared_ptr<Primitive>> orderedPrims;
  BVHBuildNode *root;
  root = recursiveBuild(arena, primitiveInfo, 0, primitives.size(), &totalNodes,
                        orderedPrims);
  this->primitives.swap(orderedPrims);

  nodes = AllocAligned<LinearBVHNode>(totalNodes);
  int offset = 0;
  flattenBVHTree(root, &offset);
}

bool BVHAccelerator::Intersect(const Ray &ray,
                               SurfaceInteraction *isect) const {
  if (!nodes) return false;
  bool hit = false;
  Vector3f invDir(1 / ray.d.x, 1 / ray.d.y, 1 / ray.d.z);
  int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};
  // Follow ray through BVH nodes to find primitive intersections
  int toVisitOffset = 0, currentNodeIndex = 0;
  int nodesToVisit[64];
  while (true) {
    const LinearBVHNode *node = &nodes[currentNodeIndex];
    // Check ray against BVH node
    if (node->bounds.IntersectP(ray, invDir, dirIsNeg)) {
      if (node->nPrimitives > 0) {
        // Intersect ray with primitives in leaf BVH node
        for (int i = 0; i < node->nPrimitives; ++i)
          if (primitives[node->primitivesOffset + i]->Intersect(ray, isect))
            hit = true;
        if (toVisitOffset == 0) break;
        currentNodeIndex = nodesToVisit[--toVisitOffset];
      } else {
        // Put far BVH node on _nodesToVisit_ stack, advance to near
        // node
        if (dirIsNeg[node->axis]) {
          nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
          currentNodeIndex = node->secondChildOffset;
        } else {
          nodesToVisit[toVisitOffset++] = node->secondChildOffset;
          currentNodeIndex = currentNodeIndex + 1;
        }
      }
    } else {
      if (toVisitOffset == 0) break;
      currentNodeIndex = nodesToVisit[--toVisitOffset];
    }
  }
  return hit;
}

bool BVHAccelerator::IntersectP(const Ray &ray) const {
  if (!nodes) return false;
  Vector3f invDir(1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z);
  int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};
  int nodesToVisit[64];
  int toVisitOffset = 0;
  int currentNodeIndex = 0;
  while (true) {
    const LinearBVHNode *node = &nodes[currentNodeIndex];
    if (node->bounds.IntersectP(ray, invDir, dirIsNeg)) {
      if (node->nPrimitives > 0) {
        for (int i = 0; i < node->nPrimitives; ++i) {
          if (primitives[node->primitivesOffset + i]->IntersectP(ray)) {
            return true;
          }
        }
        if (toVisitOffset == 0) break;
        currentNodeIndex = nodesToVisit[--toVisitOffset];
      } else {
        if (dirIsNeg[node->axis]) {
          nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
          currentNodeIndex = node->secondChildOffset;
        } else {
          nodesToVisit[toVisitOffset++] = node->secondChildOffset;
          currentNodeIndex = currentNodeIndex + 1;
        }
      }
    } else {
      if (toVisitOffset == 0) break;
      currentNodeIndex = nodesToVisit[--toVisitOffset];
    }
  }
  return false;
}
