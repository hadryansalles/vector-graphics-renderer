#ifndef RVG_DRIVER_SKIA_H
#define RVG_DRIVER_SKIA_H

#include <cstdio>
#include <string>
#include <vector>

#include "rvg-window.h"
#include "rvg-viewport.h"
#include "rvg-scene.h"

namespace rvg {
    namespace driver {
        namespace skia {

class accelerated;

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args =
        std::vector<std::string>());

} } } // namespace rvg::driver::skia

#endif
