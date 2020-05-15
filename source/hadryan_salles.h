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
#include "rvg-input-path-f-downgrade-degenerate.h"
#include "rvg-floatint.h"

#include "hadryan-bouding-box.h"
#include "hadryan-quadratic-path-segment.h"
#include "hadryan-cubic-path-segment.h"
#include "hadryan-input-path-not-interger.h"
#include "hadryan-texture-color-solver.h"
#include "hadryan-linear-gradient-solver.h"
#include "hadryan-radial-gradient-solver.h"
#include "hadryan-quad-tree-auxiliar.h"
#include "hadryan-node-object.h"
#include "hadryan-intern-node.h"
#include "hadryan-leave-node.h"

using namespace rvg;

namespace hadryan {
    
class leave_node;

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

} // hadryan
 
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