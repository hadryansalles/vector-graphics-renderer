#ifndef HADRYAN_LINEAR_GRADIENT_SOLVER_H
#define HADRYAN_LINEAR_GRADIENT_SOLVER_H

#include "hadryan-color-gradient-solver.h"

using namespace rvg;

namespace hadryan {

class linear_gradient_solver : public color_gradient_solver {
public:
    linear_gradient_solver(const paint& pat);

private:
    const linear_gradient_data m_data;
    const R2 m_p1;
    const R2 m_p2_p1;
    const double m_dot_p2_p1;
    double convert(R2 p) const;
};

} // hadryan

#endif // HADRYAN_LINEAR_GRADIENT_SOLVER_H