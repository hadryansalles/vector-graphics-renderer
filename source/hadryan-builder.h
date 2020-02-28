#pragma once

#include "hadryan-make-not-interger.h"
#include "hadryan-driver-png.h"

#include "rvg-i-scene-data.h"
#include "rvg-i-input-path.h"
#include "rvg-i-monotonic-parameters.h"
#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-monotonize.h"
#include "rvg-input-path-f-close-contours.h"
#include "rvg-input-path-f-downgrade-degenerate.h"

namespace rvg {
    namespace driver {
        namespace png {

class monotonic_builder final: public i_input_path<monotonic_builder> {
friend i_input_path<monotonic_builder>;
    std::vector<path_segment*> m_path;
    R2 m_last_move;
public:
    monotonic_builder();
    ~monotonic_builder() = default;
    void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1);
    void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2);
    void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2);
    void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3);
    void do_begin_contour(rvgf x0, rvgf y0);
    void do_end_open_contour(rvgf x0, rvgf y0);
    void do_end_closed_contour(rvgf x0, rvgf y0);
    std::vector<path_segment*>& get();
};

class accelerated_builder final: public i_scene_data<accelerated_builder> {
private:
    friend i_scene_data<accelerated_builder>;
    accelerated &acc;
    std::vector<xform> m_xf_stack;
    
    void pop_xf();
    void push_xf(const xform &xf);
    const xform &top_xf() const;
    
    void do_begin_transform(uint16_t depth, const xform &xf);
    void do_end_transform(uint16_t depth, const xform &xf);
    void do_painted_shape(e_winding_rule wr, const shape &s, const paint &p);
    inline void do_tensor_product_patch(const patch<16,4> &tpp){(void) tpp;};
    inline void do_coons_patch(const patch<12,4> &cp){(void) cp;};
    inline void do_gouraud_triangle(const patch<3,3> &gt){(void) gt;};
    inline void do_stencil_shape(e_winding_rule wr, const shape &s){(void) wr;(void) s;};
    inline void do_begin_clip(uint16_t depth){(void) depth;};
    inline void do_activate_clip(uint16_t depth){(void) depth;};
    inline void do_end_clip(uint16_t depth){(void) depth;};
    inline void do_begin_fade(uint16_t depth, unorm8 opacity){(void) depth;(void) opacity;};
    inline void do_end_fade(uint16_t depth, unorm8 opacity){(void) depth;(void) opacity;};
    inline void do_begin_blur(uint16_t depth, float radius){(void) depth;(void) radius;};
    inline void do_end_blur(uint16_t depth, float radius){(void) depth;(void) radius;};
public:
    inline accelerated_builder(accelerated &acc_in, 
        const std::vector<std::string> &args, const xform &screen_xf);
    void unpack_args(const std::vector<std::string> &args);
};

// ----- monotonic builder

inline monotonic_builder::monotonic_builder() 
    : m_last_move(make_R2(0, 0))
{}

inline void monotonic_builder::do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1) {
    std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1)};
    m_path.push_back(new linear(points[0], points[1]));
}

inline void monotonic_builder::do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2) {
    m_path.push_back(new quadratic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2)));
}

inline void monotonic_builder::do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2) {
    m_path.push_back(new quadratic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), w1));    
}

inline void monotonic_builder::do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3) {
    m_path.push_back(new cubic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), make_R2(x3, y3)));
}

inline void monotonic_builder::do_begin_contour(rvgf x0, rvgf y0) {
    m_last_move = make_R2(x0, y0);
}

inline void monotonic_builder::do_end_open_contour(rvgf x0, rvgf y0) {
    this->do_linear_segment(x0, y0, m_last_move[0], m_last_move[1]);
}

inline void monotonic_builder::do_end_closed_contour(rvgf x0, rvgf y0) {
    (void) x0;
    (void) y0;
}

inline std::vector<path_segment*>& monotonic_builder::get() {
    return m_path;
}

// ----- accelerated builder

inline accelerated_builder::accelerated_builder(accelerated &acc_in, 
    const std::vector<std::string> &args, const xform &screen_xf)
    :   acc(acc_in) {
    unpack_args(args);
    push_xf(screen_xf);
}

inline void accelerated_builder::pop_xf() {
    if (m_xf_stack.size() > 0) {
        m_xf_stack.pop_back();
    }
}
inline void accelerated_builder::push_xf(const xform &xf) {
    m_xf_stack.push_back(top_xf() * xf);
}
inline const xform &accelerated_builder::top_xf() const {
    static xform id;
    if (m_xf_stack.empty()) return id;
    else return m_xf_stack.back();
}
inline void accelerated_builder::do_begin_transform(uint16_t depth, const xform &xf) {
    (void) depth;
    push_xf(xf);
}
inline void accelerated_builder::do_end_transform(uint16_t depth, const xform &xf) {
    (void) depth;
    (void) xf;
    pop_xf(); 
}

}}}