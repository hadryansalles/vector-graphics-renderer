#include "hadryan-radial-gradient-solver.h"

using namespace rvg;

namespace hadryan {

radial_gradient_solver::radial_gradient_solver(const paint& pat)
    : color_gradient_solver(pat, pat.get_radial_gradient_data().get_color_ramp())
    , m_data(pat.get_radial_gradient_data()) {
    m_xf = identity().translated(-m_data.get_cx(), -m_data.get_cy()).scaled(1/m_data.get_r());
    R2 f = R2(m_xf.apply(make_R2(m_data.get_fx(), m_data.get_fy())));
    double mod_f = std::sqrt(f[0]*f[0] + f[1]*f[1]);
    if(mod_f > 1.0f) { // if focus transformed is in circle boudary
        mod_f = 1.0f - 0.00001;
    }
    if(mod_f > 0.00001) { // if focus transformed isn't on origin
        m_xf = m_xf.rotated(-f[0]/mod_f, f[1]/mod_f).translated(mod_f, 0);
    }
    m_B = -mod_f;
    m_C = mod_f*mod_f - 1;
}   

double radial_gradient_solver::convert(R2 p_in) const {
    R2 p(m_xf.apply(p_in));
    double A = p[0]*p[0] + p[1]*p[1];
    double B = p[0]*m_B;
    double det = B*B - A*m_C;
    assert(det >= 0);
    det = std::sqrt(det);
    assert(std::abs(-B + det) > 0);
    return A/(-B + det);
}
    
} // hadryan