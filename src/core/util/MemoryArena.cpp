#include "MemoryArena.h"

void* MemoryArena::alloc(size_t nBytes) {
  nBytes = (nBytes + 15) & (~15);
  if (currentBlockPosition + nBytes > currentAllocSize) {
    if (currentBlock) {
      usedBlocks.push_back(std::make_pair(currentAllocSize, currentBlock));
      currentBlock = nullptr;
    }
    for (auto iter = availableBlocks.begin(); iter != availableBlocks.end();
         iter++) {
      if (iter->first >= nBytes) {
        currentAllocSize = iter->first;
        currentBlock = iter->second;
        availableBlocks.erase(iter);
        break;
      }
    }
    if (!currentBlock) {
      currentAllocSize = std::max(nBytes, blockSize);
      // TODO: come back for cache alignment (allocalign)
      currentBlock = new uint8_t[currentAllocSize];
    }
  }

  void* ret = currentBlock + currentBlockPosition;
  currentBlockPosition += nBytes;
  return ret;
}

template <typename T>
T* MemoryArena::alloc(size_t n, bool runConstructor) {
  T* ret = (T*)alloc(n * sizeof(T));
  if (runConstructor) {
    for (size_t i = 0; i < n; ++i) {
      new (&ret[i]) T();
    }
  }
  return ret;
}
