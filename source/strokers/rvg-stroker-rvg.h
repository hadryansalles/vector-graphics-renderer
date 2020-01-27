#ifndef RVG_STROKER_RVG_H
#define RVG_STROKER_RVG_H

#include <vector>
#include "rvg-stroke-style.h"
#include "rvg-shape.h"

namespace rvg {
    namespace stroker {

shape rvg(const shape &input_shape, const xform &screen_xf, float width,
    stroke_style::const_ptr style);

} }

#endif
