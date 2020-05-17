#include "hadryan-monotonic-path-builder.h"

#include "hadryan-cubic-path-segment.h"
#include "hadryan-quadratic-path-segment.h"

using namespace rvg;

namespace hadryan {

void monotonic_builder::do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1) {
    std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1)};
    m_path.push_back(new linear(points[0], points[1]));
}

void monotonic_builder::do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2) {
    m_path.push_back(new quadratic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2)));
}

void monotonic_builder::do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2) {
    m_path.push_back(new quadratic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), w1));    
}

void monotonic_builder::do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3) {
    m_path.push_back(new cubic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), make_R2(x3, y3)));
}

} // hadryan