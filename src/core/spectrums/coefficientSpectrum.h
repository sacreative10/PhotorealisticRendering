#ifndef PHR_CORE_SPECTRUM_COEFFICIENT_SPECTRUM_H
#define PHR_CORE_SPECTRUM_COEFFICIENT_SPECTRUM_H

#include <cmath>

#include "core/phr.h"
#include "spectrum.h"

// TODO: Add Lerp functionality
static const int nCIESamples = 471;
extern const Float CIE_X[nCIESamples];
extern const Float CIE_Y[nCIESamples];
extern const Float CIE_Z[nCIESamples];

template <int nSpectrumSamples>
class CoefficientSpectrum {
 public:
  CoefficientSpectrum(Float v = 0.f) {
    for (int i = 0; i < nSpectrumSamples; ++i) {
      c[i] = v;
    }
  }

  static void Init() {
    for (int i = 0; i < nSpectralSamples; ++i) {
      Float w10 = Lerp(Float(i) / Float(nSpectralSamples), sampledLambdaStart,
                       sampledLambdaEnd);
      Float w11 = Lerp(Float)
    }
  }

  bool IsBlack() const {
    for (int i = 0; i < nSpectrumSamples; ++i) {
      if (c[i] != 0.) return false;
      return true;
    }
  }

  CoefficientSpectrum Clamp(Float low = 0, Float high = Infinity) const {
    CoefficientSpectrum ret;
    for (int i = 0; i < nSpectrumSamples; ++i) {
      ret.c[i] = ::Clamp(c[i], low, high);
    }
  }

  friend CoefficientSpectrum Sqrt(const CoefficientSpectrum &s) {
    CoefficientSpectrum ret;
    for (int i = 0; i < nSpectrumSamples; ++i) {
      ret.c[i] = std::sqrt(s.c[i]);
    }
  }
  friend CoefficientSpectrum Pow(const CoefficientSpectrum &s, Float pow) {
    CoefficientSpectrum ret;
    for (int i = 0; i < nSpectrumSamples; ++i) {
      ret.c[i] = std::pow(s.c[i], pow);
    }
  }
  friend CoefficientSpectrum Exp(const CoefficientSpectrum &s) {
    CoefficientSpectrum ret;
    for (int i = 0; i < nSpectrumSamples; ++i) {
      ret.c[i] = std::exp(s.c[i]);
    }
  }

  CoefficientSpectrum &operator+=(const CoefficientSpectrum &s2) {
    for (int i = 0; i < nSpectrumSamples; ++i) {
      c[i] += s2.c[i];
    }
  }
  CoefficientSpectrum operator+(const CoefficientSpectrum &s2) const {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < nSpectrumSamples; ++i) {
      ret.c[i] += s2.c[i];
    }
    return ret;
  }

  Float &operator[](int i) { return c[i]; }

  bool isNan() const {
    for (int i = 0; i < nSpectrumSamples; ++i)
      if (std::isnan(c[i])) return true;
    return false;
  }

 protected:
  static const int nSamples = nSpectrumSamples;
  Float c[nSpectrumSamples];  // a constant across all wavelangths
  SampledSpectrum X, Y, Z;
};
#endif  // !PHR_CORE_SPECTRUM_COEFFICIENT_SPECTRUM_H
