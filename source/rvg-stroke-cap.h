#ifndef RVG_STROKE_CAP_H
#define RVG_STROKE_CAP_H

#include <cstdint>

namespace rvg {

enum class e_stroke_cap: uint8_t {
    butt,
    round,
    square,
    triangle,
    fletching
};

} // namespace rvg

#endif
