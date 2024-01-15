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
