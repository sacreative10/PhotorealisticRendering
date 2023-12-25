#ifndef PHR_CORE_PHR_H
#define PHR_CORE_PHR_H

#include <limits>

#ifdef PHR_FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

static constexpr Float maxFloat = std::numeric_limits<Float>::max();
static constexpr Float Infinity = std::numeric_limits<Float>::infinity();

static constexpr Float Pi = 3.14159265358979323846;

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

#endif  // PHR_CORE_PHR_H
