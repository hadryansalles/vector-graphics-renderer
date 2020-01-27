#ifndef RVG_STROKE_JOIN_H
#define RVG_STROKE_JOIN_H

#include <cstdint>

namespace rvg {

enum class e_stroke_join: uint8_t {
    arcs,
    miter_clip,
    miter_or_bevel,
    round,
    bevel
};

} // namespace rvg

#endif
