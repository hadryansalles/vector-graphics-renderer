#ifndef RVG_FLOATINT_H
#define RVG_FLOATINT_H

#include <cstdint>

namespace rvg {

#ifdef RVG_FLOATINT64
using rvgf = double;
using rvgi = int64_t;
#else
using rvgf = float;
using rvgi = int32_t;
#endif

// This represents a float or integer
union floatint {
    floatint(rvgf ff ) {  f = ff; }
    floatint(rvgi ii) {  i = ii; }
    operator rvgf() const { return f; }
    operator rvgi() const { return i; }
    rvgf f;
    rvgi i;
};

} // namespace rvg

#endif
