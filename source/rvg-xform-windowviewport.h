#ifndef RVG_XFORM_WINDOWVIEWPORT_H
#define RVG_XFORM_WINDOWVIEWPORT_H

#include "rvg-window.h"
#include "rvg-viewport.h"
#include "rvg-align.h"
#include "rvg-aspect.h"

namespace rvg {

affinity make_windowviewport( const window &wid, const viewport &vp,
    e_align align_x = e_align::mid, e_align align_y = e_align::mid,
    e_aspect aspect = e_aspect::none);

} // namespace rvg

#endif
