//
// Created by sohaibalam on 26/12/23.
//

#ifndef EFLOAT_H
#define EFLOAT_H
#include "core/phr.h"

class EFloat {
 public:
  EFloat() {}
  EFloat(float v, float err = 0.f) : v(v), err(err) {
#ifndef NDEBUG
    ld = v;
#endif
  }
  EFloat operator+(EFloat f) const {
    EFloat r;
    r.v = v + f.v;
#ifndef NDEBUG
    r.ld = ld + f.ld;
#endif
    r.err = err + f.err + gamma(1) * (std::abs(v + f.v) + err + f.err);
    return r;
  }
  EFloat operator-(EFloat f) const {
    EFloat r;
    r.v = v - f.v;
#ifndef NDEBUG
    r.ld = ld - f.ld;
#endif
    r.err = err + f.err + gamma(1) * (std::abs(v - f.v) + err + f.err);
    return r;
  }

  EFloat operator*(EFloat f) const {
    EFloat r;
    r.v = v * f.v;
#ifndef NDEBUG
    r.ld = ld * f.ld;
#endif
    r.err =
        gamma(1) * (std::abs(f.v) * err + std::abs(v) * f.err) + err * f.err;
    return r;
  }

  EFloat operator/(EFloat f) const {
    EFloat r;
    r.v = v / f.v;
#ifndef NDEBUG
    r.ld = ld / f.ld;
#endif
    r.err = gamma(1) * (std::abs(v) * f.err + err * std::abs(f.v)) +
            err * f.err / (f.v * f.v);
    return r;
  }

  explicit operator float() const { return v; }

  float GetAbsoluteError() const { return err; }

  float UpperBound() const { return nextFloatUp(v + err); }
  float LowerBound() const { return nextFloatDown(v - err); }

#ifndef NDEBUG
  float GetRelativeError() const { return std::abs((ld - v) / ld); }
  long double PreciseValue() const { return ld; }
#endif

 private:
  float v;
  float err;
#ifndef NDEBUG
  long double ld;
#endif
  friend inline bool Quadratic(EFloat A, EFloat B, EFloat C, EFloat *t0,
                               EFloat *t1);
};

inline EFloat operator*(float f, EFloat fe) { return EFloat(f) * fe; }
inline EFloat operator/(float f, EFloat fe) { return EFloat(f) / fe; }
inline EFloat operator+(float f, EFloat fe) { return EFloat(f) + fe; }
inline EFloat operator-(float f, EFloat fe) { return EFloat(f) - fe; }

inline bool Quadratic(EFloat A, EFloat B, EFloat C, EFloat *t0, EFloat *t1);
inline bool Quadratic(EFloat A, EFloat B, EFloat C, EFloat *t0, EFloat *t1) {
  double discrim = (double)B.v * (double)B.v - 4. * (double)A.v * (double)C.v;
  if (discrim < 0.) return false;
  double rootDiscrim = std::sqrt(discrim);

  EFloat floatRootDiscrim(rootDiscrim, gamma(3) * rootDiscrim);

  EFloat q;
  if ((float)B < 0)
    q = -.5 * (B - floatRootDiscrim);
  else
    q = -.5 * (B + floatRootDiscrim);
  *t0 = q / A;
  *t1 = C / q;
  if ((float)*t0 > (float)*t1) std::swap(*t0, *t1);
  return true;
}

#endif  // EFLOAT_H
