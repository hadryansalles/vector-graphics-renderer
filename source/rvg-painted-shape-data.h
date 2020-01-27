#ifndef RVG_PAINTED_SHAPE_DATA_H
#define RVG_PAINTED_SHAPE_DATA_H

#include "rvg-paint.h"
#include "rvg-shape.h"

namespace rvg {

class painted_shape_data {

    e_winding_rule m_winding_rule;
    shape::const_ptr m_shape_ptr;
    paint::const_ptr m_paint_ptr;

public:
    painted_shape_data(e_winding_rule winding_rule,
        shape::const_ptr shape_ptr, paint::const_ptr paint_ptr):
        m_winding_rule{winding_rule},
        m_shape_ptr{shape_ptr},
        m_paint_ptr{paint_ptr} {
        ;
    }

    e_winding_rule get_winding_rule(void) const {
        return m_winding_rule;
    }

    const shape &get_shape(void) const {
        return *m_shape_ptr;
    }

    const paint &get_paint(void) const {
        return *m_paint_ptr;
    }

    shape::const_ptr get_shape_ptr(void) const {
        return m_shape_ptr;
    }

    paint::const_ptr get_paint_ptr(void) const {
        return m_paint_ptr;
    }

};

} // namespace rvg

#endif
