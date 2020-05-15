#include "hadryan-scene-object.h"

#include "hadryan-radial-gradient-solver.h"
#include "hadryan-linear-gradient-solver.h"
#include "hadryan-texture-color-solver.h"

using namespace rvg;

namespace hadryan {

scene_object::scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint_in) 
    : m_wrule(wrule) {
    m_path = path;
    R2 bb0 = path[0]->first();
    R2 bb1 = path[0]->last();
    for(auto &seg : path){
        R2 f = seg->first();
        R2 l = seg->last();
        bb0 = make_R2(std::min(bb0[0], f[0]), std::min(bb0[1], f[1]));
        bb0 = make_R2(std::min(bb0[0], l[0]), std::min(bb0[1], l[1]));
        bb1 = make_R2(std::max(bb1[0], f[0]), std::max(bb1[1], f[1]));
        bb1 = make_R2(std::max(bb1[0], l[0]), std::max(bb1[1], l[1]));
    }
    m_bbox = bouding_box(bb0, bb1);
    if(paint_in.is_solid_color()) {
        m_color = std::make_unique<color_solver>(paint_in);
    } else if(paint_in.is_linear_gradient()) {
        m_color = std::make_unique<linear_gradient_solver>(paint_in);
    } else if(paint_in.is_radial_gradient()) {
        m_color = std::make_unique<radial_gradient_solver>(paint_in);
    } else if(paint_in.is_texture()) { 
        m_color = std::make_unique<texture_solver>(paint_in);
    } else {
        RGBA8 s_transparent(0, 0, 0, 0);
        unorm8 s_opacity(0);
        paint s_paint(s_transparent, s_opacity);
        m_color = std::make_unique<color_solver>(s_paint);
    }
}

scene_object::~scene_object() {
    for(auto &seg : m_path) {
        delete seg;
        seg = NULL;
    }
    m_path.clear();
}

} // hadryan