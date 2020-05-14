#ifndef HADRYAN_PATH_SEGMENT_H
#define HADRYAN_PATH_SEGMENT_H

#include "rvg-point.h"

#include "hadryan-bouding-box.h"

using namespace rvg;

namespace hadryan {

class path_segment {
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

protected:
    const R2 m_pi;
    const R2 m_pf;
    const R2 m_right;

    int m_dir;
    int m_sh_dir;

public:
    const bouding_box m_bbox;
};

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

} // hadryan

#endif // HADRYAN_PATH_SEGMENT_H