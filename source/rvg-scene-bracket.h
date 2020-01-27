#ifndef RVG_SCENE_BRACKET_H
#define RVG_SCENE_BRACKET_H

#include "rvg-xform.h"

namespace rvg {

//??D make this a proper class!
struct scene_bracket {

    using element_id = uint32_t;

    enum class e_type: uint8_t {
        begin_clip,
        activate_clip,
        end_clip,
        begin_fade,
        end_fade,
        begin_blur,
        end_blur,
        begin_transform,
        end_transform,
        empty
    };

    // ??D Some of these values are specific to certain
    // bracket types. Perhaps we could move them to a
    // union. Here they are ordered to minimize the
    // structure size due to alignment
    xform m_xf;              // xform for transform group
    float m_blur_radius;     // standard deviation for blur group
    element_id m_element_id; // index in element array where bracket lives
    uint16_t m_depth;        // nesting depth
    e_type m_type;           // bracket type
    unorm8 m_opacity;       // opacity for fade group

    scene_bracket(const e_type &type, const element_id &eid, uint16_t depth):
        m_xf(),
        m_blur_radius(0.f),
        m_element_id(eid),
        m_depth(depth),
        m_type(type),
        m_opacity(255)
        { ; }

    scene_bracket(const e_type &type, element_id eid, uint16_t depth,
        unorm8 opacity):
        m_xf(),
        m_blur_radius(0.f),
        m_element_id(eid),
        m_depth(depth),
        m_type(type),
        m_opacity(opacity)
        { ; }

    scene_bracket(const e_type &type, element_id eid, uint16_t depth,
        const xform &xf):
        m_xf(xf),
        m_blur_radius(0.f),
        m_element_id(eid),
        m_depth(depth),
        m_type(type),
        m_opacity(255)
        { ; }

    scene_bracket(const e_type &type, element_id eid, uint16_t depth,
        float blur_radius):
        m_xf(),
        m_blur_radius(blur_radius),
        m_element_id(eid),
        m_depth(depth),
        m_type(type),
        m_opacity(255)
        { ; }
};


} // namespace rvg
#endif
