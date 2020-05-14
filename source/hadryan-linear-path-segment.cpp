#include "hadryan-linear-path-segment.h"

using namespace rvg;

namespace hadryan {

linear::linear(const R2 &p0, const R2 &p1)
    : path_segment(p0, p1)
    , m_d(p1-p0) 
{}

bool linear::implicit_hit(double x, double y) const {
    return (m_d[1]*((x - m_pi[0])*m_d[1] - (y - m_pi[1])*m_d[0]) <= 0);
}

} // hadryan