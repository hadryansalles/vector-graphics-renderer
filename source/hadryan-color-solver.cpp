#include "hadryan-color-solver.h"

using namespace rvg;

namespace hadryan {

color_solver::color_solver(const paint& pat)
    : m_paint(pat)
    , m_inv_xf(m_paint.get_xf().inverse())
{}

double color_solver::spread(e_spread spread, double t) const {
    double rt = t;
    if(t < 0 || t > 1) {
        switch(spread){
            case e_spread::clamp:
                rt = std::max(0.0, std::min(1.0, t));
                break;
            case e_spread::wrap:
                rt = t - std::floor(t);
                break;
            case e_spread::mirror:
                rt =  t - std::floor(t);
                if((int)rt%2 == 0){
                    rt = 1 - rt;
                }
                break;
            case e_spread::transparent:
                rt = -1;
                break;
            default:
                rt = -1;
                break;
        }
    }
    return rt;
}
            
RGBA8 color_solver::solve(double x, double y) const {
    (void) x;
    (void) y;
    RGBA8 color = m_paint.get_solid_color();
    return make_rgba8(
        color[0], color[1], color[2], color[3]*m_paint.get_opacity()
    );
}

} // hadryan