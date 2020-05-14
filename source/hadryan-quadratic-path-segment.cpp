#include "hadryan-quadratic-path-segment.h"

using namespace rvg;

namespace hadryan {

quadratic::quadratic(const R2 &p0, const R2 &p1, const R2& p2, double w) 
    : path_segment(p0, p2)
    , m_p1(p1-(p0*w))
    , m_p2(p2-p0)
    , m_diag(make_R2(0, 0), m_p2)
    , m_cvx(m_diag.implicit_hit(m_p1[0], m_p1[1]))
    , m_A(4.0*m_p1[0]*m_p1[0]-4.0*w*m_p1[0]*m_p2[0]+m_p2[0]*m_p2[0])
    , m_B(4.0*m_p1[0]*m_p2[0]*m_p1[1]-4.0*m_p1[0]*m_p1[0]*m_p2[1])
    , m_C(-4.0*m_p2[0]*m_p1[1]*m_p1[1]+4.0*m_p1[0]*m_p1[1]*m_p2[1])
    , m_D(-8.0*m_p1[0]*m_p1[1]+4.0*w*m_p2[0]*m_p1[1]+4.0*w*m_p1[0]*m_p2[1]-2.0*m_p2[0]*m_p2[1])
    , m_E(4.0*m_p1[1]*m_p1[1]-4.0*w*m_p1[1]*m_p2[1]+m_p2[1]*m_p2[1]) 
    , m_der((2*m_p2[1]*(-m_p2[0]*m_p1[1]+m_p1[0]*m_p2[1]))) 
{}

bool quadratic::implicit_hit(double x, double y) const {
    x -= m_pi[0];
    y -= m_pi[1];
    bool diag_hit = m_diag.implicit_hit(x, y);
    return(m_cvx && (diag_hit && hit_me(x, y)))
       ||(!m_cvx && (diag_hit || hit_me(x, y)));
}

} // hadryan
