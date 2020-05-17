#ifndef HADRYAN_MONOTONIC_PATH_BUILDER_H
#define HADRYAN_MONOTONIC_PATH_BUILDER_H

#include <vector>

#include "rvg-i-input-path.h"

using namespace rvg;

namespace hadryan {

class path_segment;

class monotonic_builder final: public i_input_path<monotonic_builder> {
friend i_input_path<monotonic_builder>;

private:
    std::vector<path_segment*> m_path;
    R2 m_last_move;

public:
    monotonic_builder();
    ~monotonic_builder() = default;
    
    void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1);
    void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2);
    void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2);
    void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3);
    void do_begin_contour(rvgf x0, rvgf y0);
    void do_end_open_contour(rvgf x0, rvgf y0);
    void do_end_closed_contour(rvgf x0, rvgf y0);

    std::vector<path_segment*>& get();
};

inline monotonic_builder::monotonic_builder() 
    : m_last_move(make_R2(0, 0))
{}

inline void monotonic_builder::do_begin_contour(rvgf x0, rvgf y0) {
    m_last_move = make_R2(x0, y0);
}

inline void monotonic_builder::do_end_open_contour(rvgf x0, rvgf y0) {
    this->do_linear_segment(x0, y0, m_last_move[0], m_last_move[1]);
}

inline void monotonic_builder::do_end_closed_contour(rvgf x0, rvgf y0) {
    (void) x0;
    (void) y0;
}

inline std::vector<path_segment*>& monotonic_builder::get() {
    return m_path;
}

} // hadryan

#endif // HADRYAN_MONOTONIC_PATH_BUILDER_H