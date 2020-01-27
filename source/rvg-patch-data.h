#ifndef RVG_PATCH_DATA_H
#define RVG_PATCH_DATA_H

#include <algorithm>

#include "rvg-ptr.h"
#include "rvg-point.h"
#include "rvg-rgba.h"

namespace rvg {

template <size_t P, size_t C>
class patch_data final:
    public boost::intrusive_ref_counter<patch_data<P, C>> {

    std::array<R2, P> m_points;
    std::array<RGBA8, C> m_colors;

public:

    static constexpr size_t points_size = P;
    static constexpr size_t colors_size = C;

    using ptr = boost::intrusive_ptr<patch_data<P, C>>;
    using const_ptr = boost::intrusive_ptr<const patch_data<P, C>>;

    patch_data(void) = default;

    patch_data(const R2 (&points)[P], const RGBA8 (&colors)[C]) {
        std::copy(points, points+P, m_points.begin());
        std::copy(colors, colors+C, m_colors.begin());
    }

    const std::array<R2, P> &get_points(void) const {
        return m_points;
    }

    std::array<R2, P> &get_points(void) {
        return m_points;
    }

    const std::array<RGBA8, C> &get_colors(void) const {
        return m_colors;
    }

    std::array<RGBA8, C> &get_colors(void) {
        return m_colors;
    }

};

} // namespace rvg

#endif
