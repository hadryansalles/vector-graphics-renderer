#ifndef RVG_WINDOW_H
#define RVG_WINDOW_H

#include "rvg-bbox.h"
#include "rvg-floatint.h"

namespace rvg {

using window = bbox<rvgf>;

static inline window make_window(rvgf xl, rvgf yb, rvgf xr, rvgf yt) {
    return window(xl, yb, xr, yt);
}

} // namespace rvg

#endif
