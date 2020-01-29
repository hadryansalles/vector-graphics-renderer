#ifndef RVG_DRIVER_CAIRO_H
#define RVG_DRIVER_CAIRO_H

#include <iosfwd>
#include <string>
#include <vector>

#include "rvg-viewport.h"
#include "rvg-window.h"
#include "rvg-scene.h"

namespace rvg {
    namespace driver {
        namespace cairo {

class accelerated;

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args =
        std::vector<std::string>());

} } } // namespace rvg::driver::cairo

#endif
