#include "AllocAligned.h"

#include <malloc.h>

#include "phr.h"

#define PBRT_L1_CACHE_LINE_SIZE 64

void *AllocAligned(size_t size) {
#if defined(PBRT_IS_WINDOWS)
  return _aligned_malloc(size, PBRT_L1_CACHE_LINE_SIZE);
#elif defined(PBRT_IS_OPENBSD) || defined(PBRT_IS_OSX)
  void *ptr;
  if (posix_memalign(&ptr, PBRT_L1_CACHE_LINE_SIZE, size) != 0) ptr = nullptr;
  return ptr;
#else
  return memalign(PBRT_L1_CACHE_LINE_SIZE, size);
#endif
}

void FreeAligned(void *ptr) {
#if defined(PBRT_IS_WINDOWS)
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}

template <typename T>
void FreeAligned(T *ptr) {
  FreeAligned((void *)ptr);
}
