#ifndef HADRYAN_QUADRATIC_PATH_SEGMENT_H
#define HADRYAN_QUADRATIC_PATH_SEGMENT_H

#include "hadryan-linear-path-segment.h"

using namespace rvg;

namespace hadryan {

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

inline bool quadratic::hit_me(double x, double y) const {
    return m_der*((y*(y*m_A + m_B) + x*(m_C + y*m_D + x*m_E))) <= 0;
}

} // hadryan

#endif // HADRYAN_QUADRATIC_PATH_SEGMENT_H