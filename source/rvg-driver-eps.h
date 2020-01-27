#ifndef RVG_DRIVER_EPS_H
#define RVG_DRIVER_EPS_H

#include <iosfwd>
#include <string>
#include <vector>

#include "rvg-window.h"
#include "rvg-viewport.h"
#include "rvg-scene.h"

namespace rvg {
    namespace driver {
        namespace eps {

const scene &accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const scene &c, const window &w, const viewport &v,
    std::ostream &out, const std::vector<std::string> &args =
        std::vector<std::string>());

} } } // namespace rvg::driver::eps

#endif
