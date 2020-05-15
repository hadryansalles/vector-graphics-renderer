#ifndef HADRYAN_TEXTURE_COLOR_SOLVER_H
#define HADRYAN_TEXTURE_COLOR_SOLVER_H

#include "hadryan-color-solver.h"

using namespace rvg;

namespace hadryan {

class texture_solver : public color_solver {
private:
    const i_image::const_ptr m_image_ptr;
    const e_spread m_spread;
    const int m_w, m_h;
public:
    texture_solver(const paint &pat);
    RGBA8 solve(double x, double y) const;
};

} // hadryan

#endif // HADRYAN_TEXTURE_COLOR_SOLVER_H