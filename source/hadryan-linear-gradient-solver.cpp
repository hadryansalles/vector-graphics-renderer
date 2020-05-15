#include "hadryan-linear-gradient-solver.h"

using namespace rvg;

namespace hadryan {

linear_gradient_solver::linear_gradient_solver(const paint& pat)
    : color_gradient_solver(pat, pat.get_linear_gradient_data().get_color_ramp())
    , m_data(m_paint.get_linear_gradient_data())
    , m_p1(m_data.get_x1(), m_data.get_y1())
    , m_p2_p1(m_data.get_x2()-m_data.get_x1(), m_data.get_y2()-m_data.get_y1()) 
    , m_dot_p2_p1(dot(m_p2_p1, m_p2_p1))
{}

double linear_gradient_solver::convert(R2 p) const {
    return dot((p-m_p1), (m_p2_p1))/m_dot_p2_p1;
}

} // hadryan