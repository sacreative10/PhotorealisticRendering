//
// Created by sohaibalam on 15/01/24.
//

#ifndef ALLOCALIGNED_H
#define ALLOCALIGNED_H

#include <cstddef>

void *AllocAligned(size_t size);
template <typename T>
T *AllocAligned(size_t count) {
  return (T *)AllocAligned(count * sizeof(T));
}
void FreeAligned(void *ptr);

template <typename T>
void FreeAligned(T *ptr);
#endif  // ALLOCALIGNED_H
