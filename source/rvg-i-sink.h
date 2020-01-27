#ifndef RVG_I_SINK_H
#define RVG_I_SINK_H

#include "rvg-meta.h"

namespace rvg {

template <typename DERIVED>
class i_sink {

    DERIVED &derived(void) {
        return *static_cast<DERIVED *>(this);
    }

    const DERIVED &derived(void) const {
        return *static_cast<const DERIVED *>(this);
    }

public:

    auto &sink(void) {
        return derived().do_sink();
    }

    const auto &sink(void) const {
        return derived().do_sink();
    }

};

}

namespace rvg {
    namespace meta {

template <typename DERIVED>
using is_an_i_sink = std::integral_constant<
    bool,
        is_template_base_of<
        rvg::i_sink,
        typename remove_reference_cv<DERIVED>::type
    >::value>;

} } // namespace rvg::meta

#endif
