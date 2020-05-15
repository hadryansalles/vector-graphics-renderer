#include "hadryan-color-gradient-solver.h"

namespace hadryan {

color_gradient_solver::color_gradient_solver(const paint &pat, const color_ramp &ramp) 
    : color_solver(pat)
    , m_ramp(ramp)
    , m_stops(m_ramp.get_color_stops())
    , m_stops_size(m_stops.size())
{}

RGBA8 color_gradient_solver::wrap(double t) const {
    RGBA8 color(0, 0, 0, 0);
    if(m_stops_size > 0) {
        if(t <= m_stops[0].get_offset()){
            color = m_stops[0].get_color();
        }
        else if(t >= m_stops[m_stops_size-1].get_offset()){
            color = m_stops[m_stops_size-1].get_color();
        }
        else if(m_stops_size > 1) {
            for(unsigned int i = 0, j = 1; j < m_stops_size; i++, j++){
                if(m_stops[j].get_offset() >= t){
                    double amp = m_stops[j].get_offset() - m_stops[i].get_offset();
                    t -= m_stops[i].get_offset();
                    t /= amp;
                    RGBA8 c1 = m_stops[i].get_color();
                    RGBA8 c2 = m_stops[j].get_color();
                    return make_rgba8(
                        c1[0]*(1-t) + c2[0]*t,
                        c1[1]*(1-t) + c2[1]*t,
                        c1[2]*(1-t) + c2[2]*t,
                        c1[3]*(1-t) + c2[3]*t 
                    );
                }
            }
        }
    }
    return color;
}

RGBA8 color_gradient_solver::solve(double x, double y) const {
    RGBA8 color(0, 0, 0, 0);
    R2 p(m_inv_xf.apply(make_R2(x, y)));
    double t = spread(m_ramp.get_spread(), convert(p));
    if(t != -1) {
        color = wrap(t);
    }
    return make_rgba8(color[0], color[1], color[2], color[3]*m_paint.get_opacity());
}

} // hadryan