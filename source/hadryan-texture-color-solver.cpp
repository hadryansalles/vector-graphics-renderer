#include "hadryan-texture-color-solver.h"

using namespace rvg;

namespace hadryan {

texture_solver::texture_solver(const paint &pat)
    : color_solver(pat)
    , m_image_ptr(pat.get_texture_data().get_image_ptr())
    , m_spread(pat.get_texture_data().get_spread())
    , m_w(m_image_ptr->get_width())
    , m_h(m_image_ptr->get_height())
{}

RGBA8 texture_solver::solve(double x, double y) const {
    RGBA8 color(0, 0, 0, 0);
    R2 p(m_inv_xf.apply(make_R2(x, y)));
    double s_x = spread(m_spread, p[0]);
    double s_y = spread(m_spread, p[1]);
    if(s_x != -1 && s_y != -1) {
        s_x *= m_w;
        s_y *= m_h;
        int r = 255*m_image_ptr->get_unorm(s_x, s_y, 0);
        int g = 255*m_image_ptr->get_unorm(s_x, s_y, 1);
        int b = 255*m_image_ptr->get_unorm(s_x, s_y, 2);
        color = make_rgba8(r, g, b, m_paint.get_opacity());
    }
    return color;
}

} // hadryan