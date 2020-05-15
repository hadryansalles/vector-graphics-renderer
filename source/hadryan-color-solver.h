#ifndef HADRYAN_COLOR_SOLVER_H
#define HADRYAN_COLOR_SOLVER_H

#include "rvg-paint.h"

using namespace rvg;

namespace hadryan {

class color_solver {
public:
    color_solver(const paint& pat);
    virtual ~color_solver() = default;
    virtual RGBA8 solve(double x, double y) const;

protected:
    paint m_paint;
    const xform m_inv_xf;
    double spread(e_spread spread, double t) const;
};

} // hadryan

#endif // HADRYAN_COLOR_SOLVER_H