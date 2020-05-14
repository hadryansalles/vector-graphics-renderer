#ifndef HADRYAN_BOUDING_BOX_H
#define HADRYAN_BOUDING_BOX_H

#include "rvg-point.h"

using namespace rvg;

namespace hadryan {

class bouding_box {
public: 
    bouding_box();
    bouding_box(const R2 &first, const R2 &last);
    bool hit_up(const double x, const double y) const;
    bool hit_down(const double x, const double y) const;
    bool hit_left(const double x, const double y) const;
    bool hit_right(const double x, const double y) const;
    bool hit_inside(const double x, const double y) const;
    bool intersect(const bouding_box &rhs) const;
private:
    R2 m_p0;
    R2 m_p1;
};

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

} // hadryan

#endif // HADRYAN_BOUDING_BOX_H