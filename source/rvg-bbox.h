#ifndef RVG_BBOX_H
#define RVG_BBOX_H

#include <limits>
#include <array>
#include <tuple>

namespace rvg {

template <typename F>
class bbox {
    std::array<F, 4> m_corners;
    constexpr static F lo = std::numeric_limits<F>::lowest();
    constexpr static F hi = std::numeric_limits<F>::max();

public:
    using value_type = F;

    bbox(): m_corners{{hi, hi, lo, lo}} { ; }

    bbox(F xl, F yb, F xr, F yt): m_corners{{xl, yb, xr, yt}} { ; }

    std::tuple<F, F> bl(void) const {
        return std::tuple<F, F>(m_corners[0], m_corners[1]);
    }

    std::tuple<F, F> tr(void) const {
        return std::tuple<F, F>(m_corners[2], m_corners[3]);
    }

    void set_bl(F xl, F yb) {
        m_corners[0] = xl;
        m_corners[1] = yb;
    }

    void set_tr(F xr, F yt) {
        m_corners[2] = xr;
        m_corners[3] = yt;
    }

    const std::array<F, 4> &corners(void) const {
        return m_corners;
    }

    F operator[](int i) const {
        return m_corners[i];
    }

    F &operator[](int i) {
        return m_corners[i];
    }
};

} // namespace rvg

#endif
