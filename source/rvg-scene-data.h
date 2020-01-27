#ifndef RVG_SCENE_DATA_H
#define RVG_SCENE_DATA_H

#include <cstdint>

#include "rvg-ptr.h"
#include "rvg-i-scene-data.h"
#include "rvg-xform.h"
#include "rvg-patch.h"
#include "rvg-scene-element.h"
#include "rvg-scene-bracket.h"

namespace rvg {

class scene_data final:
    public i_scene_data<scene_data>,
    public boost::intrusive_ref_counter<scene_data> {

    std::vector<scene_bracket> m_brackets;
    std::vector<scene_element> m_elements;

public:

    using ptr = boost::intrusive_ptr<scene_data>;
    using const_ptr = boost::intrusive_ptr<const scene_data>;

    template <typename SF>
    void iterate(SF &forward) const {
        std::size_t bracket_index = 0, element_index = 0;
        while (element_index < m_elements.size() ||
               bracket_index < m_brackets.size()) {
            if (bracket_index < m_brackets.size() &&
                m_brackets[bracket_index].m_element_id <= element_index) {
                const auto &b = m_brackets[bracket_index];
                using BT = typename scene_bracket::e_type;
                switch (b.m_type) {
                    case BT::begin_clip:
                        forward.begin_clip(b.m_depth);
                        break;
                    case BT::activate_clip:
                        forward.activate_clip(b.m_depth);
                        break;
                    case BT::end_clip:
                        forward.end_clip(b.m_depth);
                        break;
                    case BT::begin_fade:
                        forward.begin_fade(b.m_depth, b.m_opacity);
                        break;
                    case BT::end_fade:
                        forward.end_fade(b.m_depth, b.m_opacity);
                        break;
                    case BT::begin_blur:
                        forward.begin_blur(b.m_depth, b.m_blur_radius);
                        break;
                    case BT::end_blur:
                        forward.end_blur(b.m_depth, b.m_blur_radius);
                        break;
                    case BT::begin_transform:
                        forward.begin_transform(b.m_depth, b.m_xf);
                        break;
                    case BT::end_transform:
                        forward.end_transform(b.m_depth, b.m_xf);
                        break;
                    case BT::empty:
                        break;
                }
                ++bracket_index;
            } else if (element_index < m_elements.size()) {
                const auto &e = m_elements[element_index];
                using ET = typename scene_element::e_type;
                switch (e.get_type()) {
                    case ET::painted_shape: {
                        const auto &p = e.get_painted_shape_data();
                        forward.painted_shape(p.get_winding_rule(),
                            p.get_shape(), p.get_paint());
                        break;
                    }
                    case ET::stencil_shape: {
                        const auto &p = e.get_stencil_shape_data();
                        forward.stencil_shape(p.get_winding_rule(),
                            p.get_shape());
                        break;
                    }
                    case ET::tensor_product_patch:
                        forward.tensor_product_patch(
                            e.get_tensor_product_patch());
                        break;
                    case ET::coons_patch:
                        forward.coons_patch(e.get_coons_patch());
                        break;
                    case ET::gouraud_triangle:
                        forward.gouraud_triangle(e.get_gouraud_triangle());
                        break;
                    case ET::empty:
                        break;
                }
                ++element_index;
            } else {
                break;
            }
        }
    }

    template <typename SF>
    void iterate(SF &&forward) const {
        this->iterate(forward);
    }

private:

    void push_clip_bracket(const typename scene_bracket::e_type &type,
        uint16_t depth) {
        m_brackets.emplace_back(type, static_cast<scene_bracket::element_id>(
                m_elements.size()), depth);
    }

    void push_fade_bracket(const typename scene_bracket::e_type &type,
        uint16_t depth, unorm8 opacity) {
        m_brackets.emplace_back(type, static_cast<scene_bracket::element_id>(
                m_elements.size()), depth, opacity);
    }

    void push_blur_bracket(const typename scene_bracket::e_type &type,
        uint16_t depth, float radius) {
        m_brackets.emplace_back(type, static_cast<scene_bracket::element_id>(
                m_elements.size()), depth, radius);
    }

    void push_transform_bracket(const typename scene_bracket::e_type &type,
        uint16_t depth, const xform &xf) {
        m_brackets.emplace_back(type, static_cast<scene_bracket::element_id>(
                m_elements.size()), depth, xf);
    }

    friend i_scene_data<scene_data>;

    void do_painted_shape(e_winding_rule winding_rule, const shape &s,
        const paint &p) {
        m_elements.emplace_back(painted_shape_data{winding_rule,
            make_intrusive<shape>(s),
            make_intrusive<paint>(p)});
    }

    void do_tensor_product_patch(const patch<16,4> &tpp) {
        m_elements.emplace_back(make_intrusive<const patch<16,4>>(tpp));
    }

    void do_coons_patch(const patch<12,4> &cp) {
        m_elements.emplace_back(make_intrusive<const patch<12,4>>(cp));
    }

    void do_gouraud_triangle(const patch<3,3> &gt) {
        m_elements.emplace_back(make_intrusive<const patch<3,3>>(gt));
    }

    void do_stencil_shape(e_winding_rule winding_rule, const shape &s) {
        m_elements.emplace_back(stencil_shape_data{winding_rule,
            make_intrusive<shape>(s)});
    }

    void do_begin_clip(uint16_t depth) {
        push_clip_bracket(scene_bracket::e_type::begin_clip, depth);
    }

    void do_activate_clip(uint16_t depth) {
        push_clip_bracket(scene_bracket::e_type::activate_clip, depth);
    }

    void do_end_clip(uint16_t depth) {
        push_clip_bracket(scene_bracket::e_type::end_clip, depth);
    }

    void do_begin_fade(uint16_t depth, unorm8 opacity) {
        push_fade_bracket(scene_bracket::e_type::begin_fade, depth, opacity);
    }

    void do_end_fade(uint16_t depth, unorm8 opacity) {
        push_fade_bracket(scene_bracket::e_type::end_fade, depth, opacity);
    }

    void do_begin_blur(uint16_t depth, float radius) {
        push_blur_bracket(scene_bracket::e_type::begin_blur, depth, radius);
    }

    void do_end_blur(uint16_t depth, float radius) {
        push_blur_bracket(scene_bracket::e_type::end_blur, depth, radius);
    }

    void do_begin_transform(uint16_t depth, const xform &xf) {
        push_transform_bracket(scene_bracket::e_type::begin_transform, depth, xf);
    }

    void do_end_transform(uint16_t depth, const xform &xf) {
        push_transform_bracket(scene_bracket::e_type::end_transform, depth, xf);
    }
};

} // namespace rvg

#endif
