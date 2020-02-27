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

