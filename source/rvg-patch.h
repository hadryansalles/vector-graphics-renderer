#ifndef RVG_PATCH_H
#define RVG_PATCH_H

#include <algorithm>

#include "rvg-ptr.h"
#include "rvg-patch-data.h"
#include "rvg-unorm.h"
#include "rvg-i-xformable.h"

namespace rvg {

template <size_t P, size_t C>
class patch final:
    public i_xformable<patch<P, C>>,
    public boost::intrusive_ref_counter<patch<P, C>> {

public:
    using patch_data_type = patch_data<P, C>;

private:

    typename patch_data_type::const_ptr m_data;
    unorm8 m_opacity;

public:

    using ptr = boost::intrusive_ptr<patch<P, C>>;
    using const_ptr = boost::intrusive_ptr<const patch<P, C>>;

    patch(const R2 (&points)[P], const RGBA8 (&colors)[C], unorm8 opacity):
        m_data{make_intrusive<patch_data_type>(points, colors)},
        m_opacity{opacity} {
        ;
    }

    patch(typename patch_data_type::const_ptr data, unorm8 opacity):
        m_data{data},
        m_opacity{opacity} {
        ;
    }

    typename patch_data_type::const_ptr get_patch_data_ptr(void) const {
        return m_data;
    }

    const patch_data_type &get_patch_data(void) const {
        return *m_data;
    }

    unorm8 get_opacity(void) const {
        return m_opacity;
    }

};

} // namespace rvg

#endif
