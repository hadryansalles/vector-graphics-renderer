#ifndef HADRYAN_COLOR_GRADIENT_SOLVER_H
#define HADRYAN_COLOR_GRADIENT_SOLVER_H

#include "hadryan-color-solver.h"

using namespace rvg;

namespace hadryan {

class color_gradient_solver : public color_solver {
protected:
    color_ramp m_ramp;
    std::vector<color_stop> m_stops;
    unsigned int m_stops_size;
    RGBA8 wrap(double t) const;
    virtual double convert(R2 p) const = 0;
public:
    color_gradient_solver(const paint &pat, const color_ramp &ramp);
    virtual ~color_gradient_solver() = default;
    virtual RGBA8 solve(double x, double y) const;
};

} // hadryan

#endif // HADRYAN_COLOR_GRADIENT_SOLVER_H 