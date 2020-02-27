#ifndef HADRYAN_MAKE_NOT_INTERGER
#define HADRYAN_MAKE_NOT_INTERGER

#include "rvg-i-sink.h"
#include "rvg-i-input-path-f-forwarder.h"
#define XEPS 0.001f

namespace rvg {

template <typename SINK>
class input_path_not_interger final:
    public i_sink<input_path_not_interger<SINK>>,
    public i_input_path_f_forwarder<input_path_not_interger<SINK>> {
    SINK m_sink;
public:

    explicit input_path_not_interger(SINK &&sink):
        m_sink(std::forward<SINK>(sink)) {
        static_assert(meta::is_an_i_input_path<SINK>::value,
            "sink is not an i_input_path");
    }

private:

friend i_sink<input_path_not_interger<SINK>>;

    SINK &do_sink(void) {
        return m_sink;
    }

    const SINK &do_sink(void) const {
        return m_sink;
    }

    inline bool is_int(rvgf a) const {
        return (a - std::floor(a)) == 0.00000f;
    }

    inline rvgf f(rvgf a) const {
        return is_int(a) ? a+XEPS : a;
    }

friend i_input_path<input_path_not_interger<SINK>>;

    void do_begin_contour(rvgf x0, rvgf y0) {
        return m_sink.begin_contour(f(x0), f(y0));
    }

    void do_end_open_contour(rvgf x0, rvgf y0) {
        return m_sink.end_open_contour(f(x0), f(y0));
    }

    void do_end_closed_contour(rvgf x0, rvgf y0) {
        return m_sink.end_closed_contour(f(x0), f(y0));
    }

    void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1) {
        return m_sink.linear_segment(f(x0), f(y0), f(x1), f(y1));
    }

    void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2) {
        return m_sink.quadratic_segment(f(x0), f(y0), f(x1), f(y1), f(x2), f(y2));
    }

    void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf w1, rvgf x2, rvgf y2) {
        return m_sink.rational_quadratic_segment(f(x0), f(y0), f(x1), f(y1), f(w1), f(x2), f(y2));
    }

    void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,
        rvgf x2, rvgf y2, rvgf x3, rvgf y3) {
        
        return m_sink.cubic_segment(f(x0), f(y0), f(x1), f(y1), f(x2), f(y2), f(x3), f(y3));
    }

};

template <typename SINK>
auto make_input_path_not_interger(SINK &&sink) {
    return input_path_not_interger<SINK>{std::forward<SINK>(sink)};
}

}

#endif // !HADRYAN_MAKE_NOT_INTERGER
