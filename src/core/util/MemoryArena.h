#ifndef PHR_CORE_UTIL_MEMORYARENA_H
#define PHR_CORE_UTIL_MEMORYARENA_H

#include <cstddef>
#include <cstdint>
#include <list>
#include <utility>

class MemoryArena {
 public:
  MemoryArena(size_t blockSize = 262144) : blockSize(blockSize) {}
  void* alloc(size_t nBytes);
  template <typename T>
  T* alloc(size_t n = 1, bool runConstructor = true) {
    T* ret = (T*)alloc(n * sizeof(T));
    if (runConstructor)
      for (size_t i = 0; i < n; ++i) new (&ret[i]) T();
    return ret;
  }

 private:
  const size_t blockSize;
  // NOTE: currentBlockPosition is the offset from the start of the current
  // block to the next free byte. currentAllocSize is the total size of the
  // current
  size_t currentBlockPosition = 0, currentAllocSize = 0;
  uint8_t* currentBlock = nullptr;
  std::list<std::pair<size_t, uint8_t*>> usedBlocks, availableBlocks;
};

#endif  // PHR_CORE_UTIL_MEMORYARENA_H
