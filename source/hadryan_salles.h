#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "rvg-image.h"
#include "rvg-viewport.h"
#include "rvg-window.h"
#include "rvg-scene.h"
#include "rvg-point.h"
#include "rvg-winding-rule.h"
#include "rvg-paint.h"
#include "rvg-i-sink.h"
#include "rvg-i-input-path-f-forwarder.h"
#include "rvg-i-scene-data.h"
#include "rvg-i-input-path.h"
#include "rvg-i-monotonic-parameters.h"
#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-monotonize.h"
#include "rvg-input-path-f-close-contours.h"
#include "rvg-util.h"
#include "rvg-floatint.h"

#define XEPS 0.001f
#define BEZIER_EPS 0.00001

// --------------------------------------------------- HADRYAN_MAKE_NOT_INTERGER_H

namespace rvg {

template <typename SINK>
class input_path_not_interger final:
    public i_sink<input_path_not_interger<SINK>>,
    public i_input_path_f_forwarder<input_path_not_interger<SINK>> {
    SINK m_sink;
public:

    explicit input_path_not_interger(SINK &&sink):
        m_sink(std::forward<SINK>(sink)) {
        static_assert(meta::is_an_i_input_path<SINK>::value,
            "sink is not an i_input_path");
    }

private:

friend i_sink<input_path_not_interger<SINK>>;

    SINK &do_sink(void) {
        return m_sink;
    }

    const SINK &do_sink(void) const {
        return m_sink;
    }

    inline bool is_int(rvgf a) const {
        return (a - std::floor(a)) == 0.00000f;
    }

    inline rvgf f(rvgf a) const {
        return is_int(a) ? a+XEPS : a;
    }

friend i_input_path<input_path_not_interger<SINK>>;

    void do_begin_contour(rvgf x0, rvgf y0) {
        return m_sink.begin_contour(f(x0), f(y0));
    }

    void do_end_open_contour(rvgf x0, rvgf y0) {
        return m_sink.end_open_contour(f(x0), f(y0));
    }

    void do_end_closed_contour(rvgf x0, rvgf y0) {
        return m_sink.end_closed_contour(f(x0), f(y0));
    }

    void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1) {
        return m_sink.linear_segment(f(x0), f(y0), f(x1), f(y1));
    }

    void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2) {
        return m_sink.quadratic_segment(f(x0), f(y0), f(x1), f(y1), f(x2), f(y2));
    }

    void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf w1, rvgf x2, rvgf y2) {
        return m_sink.rational_quadratic_segment(f(x0), f(y0), f(x1), f(y1), f(w1), f(x2), f(y2));
    }

    void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2, rvgf x3, rvgf y3) {
        
        return m_sink.cubic_segment(f(x0), f(y0), f(x1), f(y1), f(x2), f(y2), f(x3), f(y3));
    }

};

template <typename SINK>
inline auto make_input_path_not_interger(SINK &&sink) {
    return input_path_not_interger<SINK>{std::forward<SINK>(sink)};
}

}

#ifndef RVG_INPUT_PATH_F_DOWNGRADE_DEGENERATE_H
#define RVG_INPUT_PATH_F_DOWNGRADE_DEGENERATE_H
// THIS FILE IS NOT PRESENT IN SRC-1.0.2

namespace rvg {

using util::is_almost_zero;
using util::det;

template<typename SINK>
class input_path_f_downgrade_degenerate :
    public i_input_path_f_forwarder<input_path_f_downgrade_degenerate<SINK>>,
    public i_sink<input_path_f_downgrade_degenerate<SINK>> {

    SINK m_sink;
public:

    input_path_f_downgrade_degenerate(SINK &&s) : m_sink(std::forward<SINK>(s)) {
        static_assert(meta::is_an_i_input_path<SINK>::value,
            "sink is not an i_input_path");
    }

friend i_sink<input_path_f_downgrade_degenerate<SINK>>;

    SINK &do_sink(void) {
        return m_sink;
    }

    const SINK &do_sink(void) const {
        return m_sink;
    }

friend i_input_path<input_path_f_downgrade_degenerate<SINK>>;
    void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2) {
        if (is_almost_zero(util::det(x1-x0, y1-y0, x2-x0, y2-y0))) {
            m_sink.linear_segment(x0, y0, x2, y2);
        } else {
            m_sink.quadratic_segment(x0, y0, x1, y1, x2, y2);
        }
    }

    void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf w1, rvgf x2, rvgf y2) {
        if (is_almost_zero(util::det(x1-x0*w1, y1-y0*w1, x2-x0, y2-y0))) {
            m_sink.linear_segment(x0, y0, x2, y2);
        } else {
            m_sink.rational_quadratic_segment(x0, y0, x1, y1, w1, x2, y2);
        }
    }

    void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2, rvgf x3, rvgf y3) {

        auto u1 = x1 - x0, u2 = x2 - x0, u3 = x3 - x0;
        auto v1 = y1 - y0, v2 = y2 - y0, v3 = y3 - y0;
        // -6[3s^3, s^2t, st^2, 3t^3] * [b0, b1, b2, b3]
        auto b3 = det(u2, v2, u1, v1);
        auto b2 = -det(u3, v3, u1, v1);
        auto b1 = -det(u3, v3, u2, v2);
        auto b0 = det(u3, v3, u2, v2, u1, v1);

        // 18[t^3, t^2, t, 1] * [d0=0, d1, d2, d3]
        auto d1 = 3 * b3 + 2 * b2 - b1;
        auto d2 = 3 * b3 + b2;
        auto d3 = b0;

        // degenerate to quadratic or linear
        if (is_almost_zero(d1) && is_almost_zero(d2)) {
            if (is_almost_zero(d3)) {
                m_sink.linear_segment(x0, y0, x3, y3);
            } else {
                m_sink.quadratic_segment(x0, y0, 1.5 * x1 - 0.5 * x0, 1.5 * y1 - 0.5 * y0, x3, y3);
            }
        } else {
            m_sink.cubic_segment(x0, y0, x1, y1, x2, y2, x3, y3);
        }
    }
};

template <typename SINK>
inline auto make_input_path_f_downgrade_degenerate(SINK &&sink) {
    return input_path_f_downgrade_degenerate<SINK>{std::forward<SINK>(sink)};
}

}
#endif

namespace rvg {
    namespace driver {
        namespace png {

// --------------------------------------------------- HADRYAN_BEZIER_H

// ----- bouding box

class bouding_box {
private:
    R2 m_p0;
    R2 m_p1;
public: 
    bouding_box();
    bouding_box(const R2 &first, const R2 &last);
    bool hit_up(const double x, const double y) const;
    bool hit_down(const double x, const double y) const;
    bool hit_left(const double x, const double y) const;
    bool hit_right(const double x, const double y) const;
    bool hit_inside(const double x, const double y) const;
    bool intersect(const bouding_box &rhs) const;
};

// ----- abstract path segment

class path_segment {
protected:
    const R2 m_pi;
    const R2 m_pf;
    const R2 m_right;

    int m_dir;
    int m_sh_dir;
public:
    const bouding_box m_bbox;
public:
    path_segment(const R2 &p0, const R2 &p1);
    virtual ~path_segment() = default;
    
    int implicit_value(double x, double y) const;
    virtual bool implicit_hit(double x, double y) const = 0;
    
    bool intersect(const double x, const double y) const;
    bool intersect_shortcut(const double x, const double y) const;
    
    int get_dir() const;
    int get_sh_dir() const;
    
    R2 first() const;
    R2 last()  const;
    R2 left()  const;
    R2 right() const;
    R2 top()   const;
    R2 bot()   const;
};

// ----- concrete linear segment

class linear : public path_segment {
private:
    const R2 m_d;
public:
    linear(const R2 &p0, const R2 &p1);
    bool implicit_hit(double x, double y) const;
};

// ----- concrete quadratic segment

class quadratic : public path_segment {
protected:
    const R2 m_p1;
    const R2 m_p2;
    const linear m_diag; 
    const bool m_cvx;
    const double m_A;
    const double m_B;
    const double m_C;
    const double m_D;
    const double m_E;
    const double m_der;
public:
    quadratic(const R2 &p0, const R2 &p1, const R2& p2, double w = 1.0);
    bool implicit_hit(double x, double y) const;
    bool hit_me(double x, double y) const;
};

// ------ concrete cubic segment

class cubic : public path_segment {
    double A;
    double B;
    double C;
    double D;
    double E;
    double F;
    double G;
    double H;
    double I;
    double m_der;
    std::vector<linear> m_tri; 
public:
    cubic(const R2 &p0, const R2 &p1, const R2 &p2, const R2 &p3);
    int triangle_hits(double x, double y) const;
    bool hit_me(double x, double y) const;
    bool implicit_hit(double x, double y) const;
};

// ----- bouding box

inline bool bouding_box::hit_up(const double x, const double y) const {
    (void) x;
    return y >= m_p1.get_y();
}

inline bool bouding_box::hit_down(const double x, const double y) const {
    (void) x;
    return y < m_p0.get_y();
}

inline bool bouding_box::hit_left(const double x, const double y) const {
    (void) y;
    return x <= m_p0.get_x();
}

inline bool bouding_box::hit_right(const double x, const double y) const {
    (void) y;
    return x > m_p1.get_x();
}

inline bool bouding_box::hit_inside(const double x, const double y) const {
    return y >= m_p0.get_y() && y < m_p1.get_y() && x >= m_p0.get_x() && x < m_p1.get_x();
}

inline bool bouding_box::intersect(const bouding_box &rhs) const {
    return m_p1[0] > rhs.m_p0[0] &&
           m_p1[1] > rhs.m_p0[1] &&
           rhs.m_p1[0] > m_p0[0] &&
           rhs.m_p1[1] > m_p0[1];
}

// ----- abstract path segment

inline bool path_segment::intersect(const double x, const double y) const {
    return !(m_bbox.hit_up(x,  y) || m_bbox.hit_right(x, y) || m_bbox.hit_down(x, y)) 
          &&(m_bbox.hit_left(x,y) || implicit_hit(x, y));
}

inline bool path_segment::intersect_shortcut(const double x, const double y) const {
    return (x < m_right[0] && y >= m_right[1]);
}

inline int path_segment::get_dir() const {
    return m_dir;
}

inline int path_segment::get_sh_dir() const  {
    return m_sh_dir;
}

inline R2 path_segment::first() const {
    return m_pi;
}

inline R2 path_segment::last() const {
    return m_pf;
}

inline R2 path_segment::left() const {
    return (m_pi[0] < m_pf[0]) ? m_pi : m_pf;
}

inline R2 path_segment::right() const {
    return (m_pi[0] >= m_pf[0]) ? m_pi : m_pf;
}

inline R2 path_segment::top() const {
    return (m_pi[1] >= m_pf[1]) ? m_pi : m_pf;
}

inline R2 path_segment::bot() const { 
    return (m_pi[1] < m_pf[1]) ? m_pi : m_pf;
}

// ----- linear concrete path segment

inline bool linear::implicit_hit(double x, double y) const {
    return (m_d[1]*((x - m_pi[0])*m_d[1] - (y - m_pi[1])*m_d[0]) <= 0);
}

// ----- quadratic concrete path segment

inline bool quadratic::implicit_hit(double x, double y) const {
    x -= m_pi[0];
    y -= m_pi[1];
    bool diag_hit = m_diag.implicit_hit(x, y);
    return(m_cvx && (diag_hit && hit_me(x, y)))
       ||(!m_cvx && (diag_hit || hit_me(x, y)));
}

inline bool quadratic::hit_me(double x, double y) const {
    return m_der*((y*(y*m_A + m_B) + x*(m_C + y*m_D + x*m_E))) <= 0;
}

// ----- cubic concrete path segment

inline bool cubic::hit_me(double x, double y) const {
    return (m_der*(y*(A + y*(y*(B) + C)) + x*(D + y*(E + y*F) + x*(G + y*H + x*I)))) <= 0;
}

inline bool cubic::implicit_hit(double x, double y) const {
    x -= m_pi[0];
    y -= m_pi[1];
    int hits = triangle_hits(x, y);
    return (hits == 2 || 
           (hits == 1 && hit_me(x, y)));
}

// --------------------------------------------------- HADRYAN_COLOR_H

// ----- abstract color solver

class color_solver {
protected:
    paint m_paint;
    const xform m_inv_xf;
    double spread(e_spread spread, double t) const;
public:
    color_solver(const paint& pat);
    virtual ~color_solver() = default;
    virtual RGBA8 solve(double x, double y) const;
};

// ----- abstract gradient solver

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

// ----- concrete linear gradient

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

// ----- concrete radial gradient

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

// ----- concrete texture solver

class texture_solver : public color_solver {
private:
    const i_image::const_ptr m_image_ptr;
    const e_spread m_spread;
    const int m_w, m_h;
public:
    texture_solver(const paint &pat);
    RGBA8 solve(double x, double y) const;
};

// --------------------------------------------------- HADRYAN_SHORTCUT_TREE_H

// ----- auxiliar functions

bool totally_inside(int xmin, int xmax, int ymin, int ymax, const path_segment* seg);
bool totally_inside(R2 bl, R2 tr, const path_segment* seg);
bool hit_v_bound(int cx, int ymin, int ymax, const path_segment* seg);
bool hit_h_bound(int cy, int xmin, int xmax, const path_segment* seg);

// ----- scene object

class scene_object {
private:
    e_winding_rule m_wrule;
    color_solver* color;
public:
    std::vector<path_segment*> m_path;
    bouding_box m_bbox;
public:
    scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint_in);
    scene_object(const scene_object &rhs) = delete;
    ~scene_object();
    scene_object& operator=(const scene_object &rhs) = delete;
    RGBA8 get_color(const double x, const double y) const;
    bool satisfy_wrule(int winding) const;
};

// ----- node object

class node_object {
    // only points to segments inside scene_object
    std::vector<const path_segment*> m_segments;
    std::vector<const path_segment*> m_shortcuts;
public:
    int m_w_increment = 0;
    const scene_object* m_ptr; 
public:
    node_object(const scene_object* ptr);
    void add_segment(const path_segment* segment, bool shortcut = false);
    bool hit(const double x, const double y) const;
    const std::vector<const path_segment*> get_all_segments() const;
    const std::vector<const path_segment*> get_shortcuts() const;
    RGBA8 get_color(const double x, const double y) const;
    int get_increment() const;
    int get_size() const;
    void increment(int inc);    
};

class leave_node;

// ----- abstract tree node

class tree_node {
protected:
    static int max_depth;
    static int min_segments;
    const double m_w;
    const double m_h;
    const bouding_box m_bbox;
    const R2 m_pc;
    const R2 m_p0;
    const R2 m_p1;
public:
    tree_node(const R2 &p0, const R2 &p1);
    virtual ~tree_node() = default;
    bool intersect(const bouding_box& bbox) const;
    virtual void destroy() {}
    bool is_in_cell(const double &x, const double &y) const;
    static void set_max_depth(int max_);
    static void set_min_segments(int min_);  
    virtual const leave_node* get_node_of(const double &x, const double &y) const = 0;
};

// ----- concrete intern node

class intern_node : public tree_node {
    tree_node* m_tr;
    tree_node* m_tl;
    tree_node* m_bl;
    tree_node* m_br;
public:
    intern_node(const R2 &p0, const R2 &p1, tree_node* tr,
            tree_node* tl, tree_node* bl, tree_node* br);
    void destroy();
    const leave_node* get_node_of(const double &x, const double &y) const;
};

// ----- concrete leave node

class leave_node : public tree_node {
    std::vector<node_object> m_objects;
    int m_n_segments;
public:
    leave_node(const R2 &p0, const R2 &p1);
    const leave_node* get_node_of(const double &x, const double &y) const;
    void add_node_object(const node_object &node_obj);
    const std::vector<node_object>& get_objects() const;
    tree_node* subdivide(int depth = 0);
};

// ----- auxiliar function

inline bool totally_inside(int xmin, int xmax, int ymin, int ymax, const path_segment* seg) {
    R2 left(seg->left());
    R2 right(seg->right());
    return (left[0] > xmin && left[0] < xmax && left[1] > ymin && left[1] < ymax) 
        || (right[0] > xmin && right[0] < xmax && right[1] > ymin && right[1] < ymax);
}

inline bool totally_inside(R2 bl, R2 tr, const path_segment* seg) {
    return totally_inside(bl[0], tr[0], bl[1], tr[1], seg);
}

inline bool hit_v_bound(int cx, int ymin, int ymax, const path_segment* seg) {
    R2 left(seg->left());
    R2 right(seg->right());
    R2 top(seg->top());
    R2 bot(seg->bot());
    if(left[0] < cx && right[0] > cx) {
        if(bot[1] > ymin && top[1] < ymax) {
            return true;
        } else if(seg->implicit_value(cx, ymin) * seg->implicit_value(cx, ymax) < 0) {
            return true;
        }
    }
    return false;
}

inline bool hit_h_bound(int cy, int xmin, int xmax, const path_segment* seg) {
    R2 left(seg->left());
    R2 right(seg->right());
    R2 top(seg->top());
    R2 bot(seg->bot());
    if(bot[1] < cy && top[1] > cy) {
        if(left[0] > xmin && right[0] < xmax) {
            return true;
        } else if(seg->implicit_value(xmin, cy) != seg->implicit_value(xmax, cy)) {
            return true;
        }
    }
    return false;
}

// ----- scene object

inline bool scene_object::satisfy_wrule(int winding) const {
    if(m_wrule == e_winding_rule::non_zero){
        return (winding != 0);
    }
    else if(m_wrule == e_winding_rule::odd){
        return ((winding % 2)!= 0);
    }
    return false;
}

inline RGBA8 scene_object::get_color(const double x, const double y) const {
    return color->solve(x, y);
}

// ----- node object

inline void node_object::add_segment(const path_segment* segment, bool shortcut) {
    if(shortcut) {
        m_shortcuts.push_back(segment);
    } else {
        m_segments.push_back(segment);
    }
}

inline void node_object::increment(int inc) {
    m_w_increment += inc;
}

inline RGBA8 node_object::get_color(const double x, const double y) const {
    return m_ptr->get_color(x, y);
}

inline int node_object::get_increment() const {
    return m_w_increment;
}

inline int node_object::get_size() const {
    return m_segments.size() + m_shortcuts.size();
}

inline const std::vector<const path_segment*> node_object::get_all_segments() const {
    std::vector<const path_segment*> all_seg;
    all_seg.insert(all_seg.end(), m_segments.begin(), m_segments.end());
    all_seg.insert(all_seg.end(), m_shortcuts.begin(), m_shortcuts.end());
    return all_seg;
}

inline const std::vector<const path_segment*> node_object::get_shortcuts() const {
    return m_shortcuts;
}

// ----- abstract tree node

inline bool tree_node::intersect(const bouding_box& bbox) const {
    return m_bbox.intersect(bbox);
}

inline bool tree_node::is_in_cell(const double &x, const double &y) const {
    return x >= (double) m_p0[0] && x < (double) m_p1[0] && y >= (double) m_p0[1] && y < (double) m_p1[1];
}

// ----- concrete leave node

inline void leave_node::add_node_object(const node_object &node_obj) {
    m_objects.push_back(node_obj);
    m_n_segments += node_obj.get_size();
}

inline const std::vector<node_object>& leave_node::get_objects() const {
    return m_objects;
}

// --------------------------------------------------- HADRYAN_DRIVER_PNG_H

class accelerated {
public:
    std::vector<scene_object*> objects;
    tree_node* root = nullptr;
    std::vector<R2> samples;
    int threads;
public:
    accelerated();
    void destroy();
    void add(scene_object* obj);
    void invert();
};

// ----- accelerated

inline accelerated::accelerated()
    : samples{make_R2(0, 0)}
    , threads(1)
{}

inline void accelerated::add(scene_object* obj){
    objects.push_back(obj);
}

inline void accelerated::invert() {
    std::reverse(objects.begin(), objects.end());
}

// --------------------------------------------------- HADRYAN_BUILDER_H

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


// --------------------------------------------------- EXPORTED FUNCTIONS

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args =
        std::vector<std::string>());

} } } // namespace rvg::driver::png

// --------------------------------------------------- HADRYAN_BLUE_NOISE_H

std::vector<rvg::R2> blue_1 {
    rvg::make_R2(0, 0)
};

std::vector<rvg::R2> blue_8 {
    rvg::make_R2(-0.266471, 0.164718),
    rvg::make_R2(0.353688, 0.0396624),
    rvg::make_R2(-0.401679, -0.215021),
    rvg::make_R2(0.488846, 0.429684),
    rvg::make_R2(0.122459, 0.282964),
    rvg::make_R2(-0.0344567, -0.0841444),
    rvg::make_R2(-0.139007, -0.475235),
    rvg::make_R2(0.207413, -0.328058)
};

std::vector<rvg::R2> blue_16 {
    rvg::make_R2(-0.15588000, 0.47011700),
    rvg::make_R2(0.30975700, 0.36190700),
    rvg::make_R2(-0.26235400, -0.08156120),
    rvg::make_R2(-0.06816580, -0.25994500),
    rvg::make_R2(0.32275800, -0.28352800),
    rvg::make_R2(-0.32080600, -0.32277300),
    rvg::make_R2(0.05122570, 0.32281400),
    rvg::make_R2(-0.20235200, 0.18019800),
    rvg::make_R2(0.47831000, -0.46175200),
    rvg::make_R2(0.14847600, -0.11096600),
    rvg::make_R2(0.25791100, 0.12312200),
    rvg::make_R2(-0.01061200, 0.05680670),
    rvg::make_R2(-0.38847100, 0.34462000),
    rvg::make_R2(0.12599600, -0.42584800),
    rvg::make_R2(-0.47645700, 0.11612000),
    rvg::make_R2(0.45528900, -0.10737200)
};

std::vector<rvg::R2> blue_32 {
    rvg::make_R2(0.18936400, -0.34008000),
    rvg::make_R2(0.31758200, 0.40063000),
    rvg::make_R2(0.00903428, -0.37434000),
    rvg::make_R2(-0.21124700, -0.08741820),
    rvg::make_R2(-0.36328000, -0.43687600),
    rvg::make_R2(0.33291100, -0.04052820),
    rvg::make_R2(-0.49970400, -0.09869600),
    rvg::make_R2(-0.43663000, 0.12511500),
    rvg::make_R2(-0.26837500, 0.40422500),
    rvg::make_R2(0.37728700, 0.11086400),
    rvg::make_R2(-0.18975300, -0.43411500),
    rvg::make_R2(-0.48250600, -0.30842600),
    rvg::make_R2(-0.13179200, -0.26574100),
    rvg::make_R2(0.49235500, 0.47500100),
    rvg::make_R2(0.42711400, 0.26635400),
    rvg::make_R2(0.37090100, -0.24554700),
    rvg::make_R2(-0.31862800, -0.24432000),
    rvg::make_R2(0.02879450, 0.27314400),
    rvg::make_R2(0.04699840, -0.19358200),
    rvg::make_R2(-0.16154700, 0.27910400),
    rvg::make_R2(0.18745700, 0.13190500),
    rvg::make_R2(-0.35758100, -0.03301080),
    rvg::make_R2(0.19744000, 0.29668700),
    rvg::make_R2(0.21454900, -0.15357100),
    rvg::make_R2(-0.06650600, -0.05443540),
    rvg::make_R2(0.12811500, 0.47731600),
    rvg::make_R2(0.33646100, -0.43140000),
    rvg::make_R2(0.09921190, 0.00607759),
    rvg::make_R2(-0.05305180, 0.44544500),
    rvg::make_R2(-0.39892000, 0.30706200),
    rvg::make_R2(-0.06095580, 0.12207700),
    rvg::make_R2(-0.25435800, 0.13223200)
};

std::vector<rvg::R2> blue_64 {
    rvg::make_R2(0.16914500, 0.03865330),
    rvg::make_R2(-0.18599500, 0.12891000),
    rvg::make_R2(-0.33949000, -0.07929130),
    rvg::make_R2(0.46276000, 0.26089400),
    rvg::make_R2(-0.14555200, -0.23657300),
    rvg::make_R2(0.02382560, -0.40556900),
    rvg::make_R2(-0.12388600, -0.37030500),
    rvg::make_R2(-0.36297100, 0.17069000),
    rvg::make_R2(-0.15521500, 0.42105700),
    rvg::make_R2(0.12082400, -0.30798700),
    rvg::make_R2(0.47381800, -0.18262100),
    rvg::make_R2(0.05882570, 0.28990200),
    rvg::make_R2(0.42593500, 0.04988550),
    rvg::make_R2(0.18165400, -0.40046500),
    rvg::make_R2(-0.23570100, -0.45028000),
    rvg::make_R2(-0.06343560, -0.05378760),
    rvg::make_R2(-0.30054000, 0.44889500),
    rvg::make_R2(-0.47561100, -0.06744120),
    rvg::make_R2(0.27591500, -0.30610000),
    rvg::make_R2(0.30878300, 0.31793800),
    rvg::make_R2(-0.43197600, 0.04322680),
    rvg::make_R2(-0.46146900, -0.31194400),
    rvg::make_R2(0.22082300, 0.48983600),
    rvg::make_R2(0.36676800, 0.17863100),
    rvg::make_R2(-0.07761080, 0.06874050),
    rvg::make_R2(0.43883100, 0.36045500),
    rvg::make_R2(-0.01210630, 0.17083500),
    rvg::make_R2(-0.24254300, -0.31853300),
    rvg::make_R2(-0.35023200, -0.34708800),
    rvg::make_R2(0.08191250, 0.49015400),
    rvg::make_R2(0.34700700, -0.18083700),
    rvg::make_R2(0.24516200, -0.07076280),
    rvg::make_R2(-0.39442700, -0.46397700),
    rvg::make_R2(-0.40244700, 0.29623300),
    rvg::make_R2(-0.20657100, -0.00285104),
    rvg::make_R2(-0.06457980, 0.31579300),
    rvg::make_R2(0.24474400, 0.17248800),
    rvg::make_R2(0.32878800, 0.44304200),
    rvg::make_R2(-0.02097480, -0.28866600),
    rvg::make_R2(0.40715100, -0.30340300),
    rvg::make_R2(-0.08692060, -0.47918800),
    rvg::make_R2(-0.40150200, -0.19901100),
    rvg::make_R2(0.04443170, 0.02485010),
    rvg::make_R2(0.09260740, -0.19953500),
    rvg::make_R2(-0.25475600, 0.34622000),
    rvg::make_R2(-0.26848000, 0.23358400),
    rvg::make_R2(-0.00398004, 0.41603200),
    rvg::make_R2(-0.03452200, -0.16271600),
    rvg::make_R2(0.38275600, -0.06430740),
    rvg::make_R2(0.29766500, 0.05252960),
    rvg::make_R2(0.15961100, 0.38493000),
    rvg::make_R2(-0.14090000, 0.24279000),
    rvg::make_R2(-0.28150000, -0.19772900),
    rvg::make_R2(0.45914900, 0.47610600),
    rvg::make_R2(-0.30685600, 0.05622170),
    rvg::make_R2(-0.18739400, -0.11503200),
    rvg::make_R2(0.21961000, -0.19201000),
    rvg::make_R2(0.11079300, 0.15561900),
    rvg::make_R2(-0.48974200, 0.15522800),
    rvg::make_R2(0.18730100, 0.27639900),
    rvg::make_R2(0.10202200, -0.08547970),
    rvg::make_R2(0.33154300, -0.42511700),
    rvg::make_R2(0.47246600, -0.41633300),
    rvg::make_R2(-0.42490700, 0.40949100)
};