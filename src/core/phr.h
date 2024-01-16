#ifndef PHR_CORE_PHR_H
#define PHR_CORE_PHR_H

#include <malloc.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#ifdef PHR_FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

static constexpr Float maxFloat = std::numeric_limits<Float>::max();
static constexpr Float Infinity = std::numeric_limits<Float>::infinity();

static constexpr Float Pi = 3.14159265358979323846;
static constexpr Float invPi = 0.31830988618379067154;

// typedef CoefficientSpectrum Spectrum;

#define PBRT_L1_CACHE_LINE_SIZE 64

static constexpr Float machineEpsilon =
    std::numeric_limits<Float>::epsilon() * 0.5;

const Float ShadowEpsilon = 0.0001f;

template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
  if (val < low) return low;
  if (val > high) return high;
  return val;
}

inline bool Quadratic(Float a, Float b, Float c, Float *t0, Float *t1) {
  // Find quadratic discriminant
  double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
  if (discrim < 0) return false;
  double rootDiscrim = std::sqrt(discrim);

  // Compute quadratic _t_ values
  double q;
  if (b < 0)
    q = -.5 * (b - rootDiscrim);
  else
    q = -.5 * (b + rootDiscrim);
  *t0 = q / a;
  *t1 = c / q;
  if (*t0 > *t1) std::swap(*t0, *t1);
  return true;
}

inline uint32_t FloatToBits(float f) {
  uint32_t ui;
  memcpy(&ui, &f, sizeof(float));
  return ui;
}

inline float BitsToFloat(uint32_t ui) {
  float f;
  memcpy(&f, &ui, sizeof(uint32_t));
  return f;
}

inline float nextFloatUp(float v) {
  if (std::isinf(v) && v > 0.) return v;
  if (v == -0.f) v = 0.f;
  uint32_t ui = FloatToBits(v);
  if (v >= 0)
    ++ui;
  else
    --ui;
  return BitsToFloat(ui);
}

inline float nextFloatDown(float v) {
  if (std::isinf(v) && v < 0.) return v;
  if (v == 0.f) v = -0.f;
  uint32_t ui = FloatToBits(v);
  if (v > 0)
    --ui;
  else
    ++ui;
  return BitsToFloat(ui);
}

inline constexpr Float gamma(int n) {
  return (n * machineEpsilon) / (1 - n * machineEpsilon);
}

#endif  // PHR_CORE_PHR_H
