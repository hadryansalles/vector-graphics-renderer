#include "hadryan-path-segment.h"

using namespace rvg;

namespace hadryan {

path_segment::path_segment(const R2 &p0, const R2 &p1)
    : m_pi(p0)
    , m_pf(p1)
    , m_right((m_pf[0] > m_pi[0]) ? m_pf : m_pi)
    , m_dir((m_pi[1] > m_pf[1]) ? -1 : 1) 
    , m_sh_dir(-1*m_dir)  
    , m_bbox(m_pi, m_pf) { 
    if((m_right == m_pf && m_dir == 1)
        ||(m_right == m_pi && m_dir == -1)) {
        m_sh_dir *= -1;
    }
} 

int path_segment::implicit_value(double x, double y) const {
    if(m_bbox.hit_inside(x, y)) {
        return implicit_hit(x, y) ? 1 : -1;
    } else if((m_right == m_pi && m_dir == -1) || (m_right == m_pf && m_dir == 1)) {
        // 0 bottom left
        // 0 top right
        if((m_bbox.hit_left(x,y) && m_bbox.hit_down(x,y)) || (m_bbox.hit_right(x,y) && m_bbox.hit_up(x,y))) {
            return 0;
        } else if(m_bbox.hit_up(x,y) || m_bbox.hit_left(x,y)) {
            return 1;
        } else if(m_bbox.hit_down(x,y) || m_bbox.hit_right(x,y)) {
            return -1;
        }
    } else if((m_right == m_pi && m_dir == 1) || (m_right == m_pf && m_dir == -1)) {
        // 0 top left
        // 0 bottom right
        if((m_bbox.hit_left(x,y) && m_bbox.hit_up(x,y)) || (m_bbox.hit_right(x,y) && (m_bbox.hit_down(x,y)))) {
            return 0;
        } else if(m_bbox.hit_up(x,y) || m_bbox.hit_right(x,y)) {
            return -1;
        } else if(m_bbox.hit_down(x,y) || m_bbox.hit_left(x,y)) {
            return 1;
        }
    }
    return 0;
}

} // hadryan