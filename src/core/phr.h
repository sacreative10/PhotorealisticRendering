

#include <limits>

#ifdef PHR_FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

static constexpr Float maxFloat = std::numeric_limits<Float>::max();
static constexpr Float Infinity = std::numeric_limits<Float>::infinity();
