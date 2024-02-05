
#include "core/geometry.h"
#include "core/phr.h"
#include "core/spectrums/coefficientSpectrum.h"

static const int sampledLambdaStart = 400;
static const int sampledLambdaEnd = 700;
static const int nSpectralSamples = 60;

class SampledSpectrum : public CoefficientSpectrum<nSpectralSamples> {
 public:
  SampledSpectrum(Float v = 0.f) : CoefficientSpectrum<nSpectralSamples>(v) {}

 private:
  static SampledSpectrum FromSampled(const Float *lambda, const Float *v,
                                     int n) {
    // TODO: Sort samples
    SampledSpectrum r;
    for (int i = 0; i < nSpectralSamples; ++i) {
      Float lambda0 = Lerp(Float(i) / Float(nSpectralSamples),
                           sampledLambdaStart, sampledLambdaEnd);
      Float lambda1 = Lerp(Float(i + 1) / Float(nSpectralSamples),
                           sampledLambdaStart, sampledLambdaEnd);
      // TODO: Implement Average Spectrum Samples
      r.c[i] = AverageSpectrumSamples(lambda, v, n, lambda0, lambda1);
    }
  }
  Float AverageSpectrumSamples(const Float *lambda, const Float *vals, int n,
                               Float lambdaStart, Float lambdaEnd) {
    if (lambdaEnd <= lambda[0]) return vals[0];
    if (lambdaStart >= lambda[n - 1]) return vals[n - 1];
    if (n == 1) return vals[0];

    Float sum = 0;
    int i = 0;
    // Add constant regions outside of wavelength ideas;
    if (lambdaStart < lambda[0]) sum += vals[0] * (lambda[0] - lambdaStart);
    if (lambdaEnd > lambda[n - 1])
      sum += vals[n - 1] * (lambdaEnd - lambda[n - 1]);
    while (lambdaStart > lambda[i + 1]) ++i;

    auto interp = [lambda, vals](Float w, int i) {
      return Lerp((w - lambda[i]) / (lambda[i + 1] - lambda[i]), vals[i],
                  vals[i + 1]);
    };
    for (; i + 1 < n && lambdaEnd >= lambda[i]; ++i) {
      Float segLambdaStart = std::max(lambdaStart, lambda[i]);
      Float segLambdaEnd = std::min(lambdaEnd, lambda[i + 1]);
      sum += 0.5 * (interp(segLambdaStart, i) + interp(segLambdaEnd, i)) *
             (segLambdaEnd - segLambdaStart);
    }
  }
};