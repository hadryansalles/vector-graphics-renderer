#ifndef HADRYAN_DRIVER_PNG_H
#define HADRYAN_DRIVER_PNG_H

#include <vector>

#include "rvg-viewport.h"
#include "rvg-scene.h"
#include "rvg-window.h"

#include "hadryan-accelerated.h"

using namespace rvg;

namespace hadryan {
    
const accelerated accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args =
        std::vector<std::string>());

} // hadryan

#endif // HADRYAN_DRIVER_PNG_H