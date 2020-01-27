#ifndef RVG_DRIVER_SVG_H
#define RVG_DRIVER_SVG_H

#include <iosfwd>
#include <string>
#include <vector>

#include "rvg-viewport.h"
#include "rvg-window.h"
#include "rvg-scene.h"

namespace rvg {
    namespace driver {
        namespace svg {

const scene &accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const scene &c, const window &w, const viewport &v,
    std::ostream &out, const std::vector<std::string> &args =
        std::vector<std::string>());

} } } // namespace rvg::driver::svg

#endif
