#ifndef HADRYAN_ACCELERATED_BUILDER_H
#define HADRYAN_ACCELERATED_BUILDER_H

#include "rvg-xform.h"
#include "rvg-winding-rule.h"
#include "rvg-patch.h"
#include "rvg-i-scene-data.h"

#include "hadryan-accelerated.h"

using namespace rvg;

namespace hadryan {

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

} // hadryan

#endif // HADRYAN_ACCELERATED_BUILDER_H