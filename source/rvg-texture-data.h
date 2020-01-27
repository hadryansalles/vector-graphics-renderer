#ifndef RVG_TEXTURE_DATA_H
#define RVG_TEXTURE_DATA_H

#include "rvg-ptr.h"
#include "rvg-spread.h"
#include "rvg-i-image.h"

namespace rvg {

class texture_data:
    public boost::intrusive_ref_counter<texture_data> {
private:
    e_spread m_spread;
    i_image::const_ptr m_image_ptr;
public:
    using ptr = boost::intrusive_ptr<texture_data>;
    using const_ptr = boost::intrusive_ptr<const texture_data>;
    texture_data(e_spread spread, i_image::const_ptr image_ptr):
        m_spread(spread),
        m_image_ptr(image_ptr) { ; }
    i_image::const_ptr get_image_ptr(void) const { return m_image_ptr; }
    const i_image &get_image(void) const { return *m_image_ptr; }
    e_spread get_spread(void) const { return m_spread; }
};

} // namespace rvg

#endif
