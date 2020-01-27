#ifndef RVG_STROKE_METHOD_H
#define RVG_STROKE_METHOD_H

#include <cstdint>

namespace rvg {

enum class e_stroke_method: uint8_t {
    native,
#ifdef STROKER_RVG
    rvg,
#endif
#ifdef STROKER_LIVAROT
    livarot_stroke,
    livarot_outline,
#endif
#ifdef STROKER_DIRECT2D
    direct2d,
#endif
#ifdef STROKER_QUARTZ
    quartz,
#endif
#ifdef STROKER_AGG
    agg,
#endif
#ifdef STROKER_SKIA
    skia,
#endif
#ifdef STROKER_QT5
    qt5,
#endif
#ifdef STROKER_MUPDF
    mupdf,
#endif
#ifdef STROKER_CAIRO
    cairo_traps,
    cairo_polygon,
    cairo_tristrip,
#endif
#ifdef STROKER_GS
    gs,
    gs_compat,
    gs_fast,
#endif
#ifdef STROKER_OPENVG_RI
    openvg_ri,
#endif
};

} // namespace rvg

#endif
