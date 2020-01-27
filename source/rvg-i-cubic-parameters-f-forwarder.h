#ifndef RVG_I_CUBIC_PARAMETERS_F_FORWARDER_H
#define RVG_I_CUBIC_PARAMETERS_F_FORWARDER_H

#include "rvg-i-cubic-parameters.h"

namespace rvg {

template <typename DERIVED>
class i_cubic_parameters_f_forwarder: public i_cubic_parameters<DERIVED> {

    DERIVED &derived(void) {
        return *static_cast<DERIVED *>(this);
    }

    const DERIVED &derived(void) const {
        return *static_cast<const DERIVED *>(this);
    }

public:

    i_cubic_parameters_f_forwarder(void) {
        static_assert(meta::is_an_i_sink<DERIVED>::value,
            "derived class is not an i_sink");
    }

private:

// i_cubics interface
friend i_cubic_parameters<DERIVED>;

    void do_inflection_parameter(rvgf t) {
        return derived().sink().inflection_parameter(t);
    }

    void do_double_point_parameter(rvgf t) {
        return derived().sink().double_point_parameter(t);
    }

};

} // namespace rvg

#endif
