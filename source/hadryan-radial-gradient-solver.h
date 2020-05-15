#ifndef HADRYAN_RADIAL_GRADIENT_SOLVER_H
#define HADRYAN_RADIAL_GRADIENT_SOLVER_H

#include "hadryan-color-gradient-solver.h"

using namespace rvg;

namespace hadryan {

class radial_gradient_solver : public color_gradient_solver {
public:
    radial_gradient_solver(const paint& pat);

private:
    const radial_gradient_data m_data;
    xform  m_xf;
    double m_B;
    double m_C;
    double convert(R2 p_in) const;
};  

} // hadryan

#endif // HADRYAN_RADIAL_GRADIENT_SOLVER_H