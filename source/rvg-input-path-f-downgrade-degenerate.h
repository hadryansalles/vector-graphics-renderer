#ifndef RVG_INPUT_PATH_F_DOWNGRADE_DEGENERATE_H
#define RVG_INPUT_PATH_F_DOWNGRADE_DEGENERATE_H

#include "rvg-util.h"
#include "rvg-i-input-path-f-forwarder.h"

namespace rvg {

using util::is_almost_zero;
using util::det;

template<typename SINK>
class input_path_f_downgrade_degenerate :
    public i_input_path_f_forwarder<input_path_f_downgrade_degenerate<SINK>>,
    public i_sink<input_path_f_downgrade_degenerate<SINK>> {

    SINK m_sink;
public:

    input_path_f_downgrade_degenerate(SINK &&s) : m_sink(std::forward<SINK>(s)) {
        static_assert(meta::is_an_i_input_path<SINK>::value,
            "sink is not an i_input_path");
    }

friend i_sink<input_path_f_downgrade_degenerate<SINK>>;

    SINK &do_sink(void) {
        return m_sink;
    }

    const SINK &do_sink(void) const {
        return m_sink;
    }

friend i_input_path<input_path_f_downgrade_degenerate<SINK>>;
    void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2) {
        if (is_almost_zero(util::det(x1-x0, y1-y0, x2-x0, y2-y0))) {
            m_sink.linear_segment(x0, y0, x2, y2);
        } else {
            m_sink.quadratic_segment(x0, y0, x1, y1, x2, y2);
        }
    }

    void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf w1, rvgf x2, rvgf y2) {
        if (is_almost_zero(util::det(x1-x0*w1, y1-y0*w1, x2-x0, y2-y0))) {
            m_sink.linear_segment(x0, y0, x2, y2);
        } else {
            m_sink.rational_quadratic_segment(x0, y0, x1, y1, w1, x2, y2);
        }
    }

    void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2, rvgf x3, rvgf y3) {

        auto u1 = x1 - x0, u2 = x2 - x0, u3 = x3 - x0;
        auto v1 = y1 - y0, v2 = y2 - y0, v3 = y3 - y0;
        // -6[3s^3, s^2t, st^2, 3t^3] * [b0, b1, b2, b3]
        auto b3 = det(u2, v2, u1, v1);
        auto b2 = -det(u3, v3, u1, v1);
        auto b1 = -det(u3, v3, u2, v2);
        auto b0 = det(u3, v3, u2, v2, u1, v1);

        // 18[t^3, t^2, t, 1] * [d0=0, d1, d2, d3]
        auto d1 = 3 * b3 + 2 * b2 - b1;
        auto d2 = 3 * b3 + b2;
        auto d3 = b0;

        // degenerate to quadratic or linear
        if (is_almost_zero(d1) && is_almost_zero(d2)) {
            if (is_almost_zero(d3)) {
                m_sink.linear_segment(x0, y0, x3, y3);
            } else {
                m_sink.quadratic_segment(x0, y0, 1.5 * x1 - 0.5 * x0, 1.5 * y1 - 0.5 * y0, x3, y3);
            }
        } else {
            m_sink.cubic_segment(x0, y0, x1, y1, x2, y2, x3, y3);
        }
    }
};

template <typename SINK>
inline auto make_input_path_f_downgrade_degenerate(SINK &&sink) {
    return input_path_f_downgrade_degenerate<SINK>{std::forward<SINK>(sink)};
}

}
#endif