#ifndef RVG_VIEWPORT_H
#define RVG_VIEWPORT_H

#include "rvg-bbox.h"

namespace rvg {

using viewport = bbox<int>;

static inline viewport make_viewport(int xl, int yb, int xr, int yt) {
    return viewport(xl, yb, xr, yt);
}

} // namespace rvg

#endif
