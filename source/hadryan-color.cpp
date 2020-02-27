#pragma once

#include "rvg-paint.h"

namespace rvg {
    namespace driver {
        namespace png {

class color_solver {
protected:
    paint m_paint;
    const xform m_inv_xf;
    double spread(e_spread spread, double t) const;
public:
    color_solver(const paint& pat);
    virtual ~color_solver();
    virtual RGBA8 solve(double x, double y) const;
};

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

class linear_gradient_solver : public color_gradient_solver {
private:
    const linear_gradient_data m_data;
    const R2 m_p1;
    const R2 m_p2_p1;
    const double m_dot_p2_p1;
    double convert(R2 p) const;
public:
    linear_gradient_solver(const paint& pat);
};

class radial_gradient_solver : public color_gradient_solver {
private:
    const radial_gradient_data m_data;
    xform  m_xf;
    double m_B;
    double m_C;
    double convert(R2 p_in) const;
public:
    radial_gradient_solver(const paint& pat);
};  

class texture_solver : public color_solver {
    const i_image::const_ptr m_image_ptr;
    const e_spread m_spread;
    const int m_w, m_h;
public:
    texture_solver(const paint &pat);
    RGBA8 solve(double x, double y) const;
};

}}}


namespace rvg {
    namespace driver {
        namespace png {

// ----- abstract color solver

inline color_solver::color_solver(const paint& pat)
    : m_paint(pat)
    , m_inv_xf(m_paint.get_xf().inverse())
{}

inline double color_solver::spread(e_spread spread, double t) const {
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

// ----- abstract gradient solver

inline color_gradient_solver::color_gradient_solver(const paint &pat, const color_ramp &ramp) 
    : color_solver(pat)
    , m_ramp(ramp)
    , m_stops(m_ramp.get_color_stops())
    , m_stops_size(m_stops.size())
{}

inline RGBA8 color_gradient_solver::wrap(double t) const {
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

// ----- linear gradient solver

inline linear_gradient_solver::linear_gradient_solver(const paint& pat)
    : color_gradient_solver(pat, pat.get_linear_gradient_data().get_color_ramp())
    , m_data(m_paint.get_linear_gradient_data())
    , m_p1(m_data.get_x1(), m_data.get_y1())
    , m_p2_p1(m_data.get_x2()-m_data.get_x1(), m_data.get_y2()-m_data.get_y1()) 
    , m_dot_p2_p1(dot(m_p2_p1, m_p2_p1))
{}

double linear_gradient_solver::convert(R2 p) const {
    return dot((p-m_p1), (m_p2_p1))/m_dot_p2_p1;
}

// ----- radial gradient solver

inline radial_gradient_solver::radial_gradient_solver(const paint& pat)
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

// ----- texture color solver

inline texture_solver::texture_solver(const paint &pat)
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

}}}