#ifndef RVG_ASPECT_H
#define RVG_ASPECT_H

namespace rvg {

enum class e_aspect {
    none,   // do not preserve aspect ratio, blindly map window to viewport
    extend, // extend window to smallest rectangle with same aspect as viewport
    trim    // trim window to largest rectangle with same aspect as viewport
};

} // namespace rvg

#endif
