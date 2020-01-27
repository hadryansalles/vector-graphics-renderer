#ifndef RVG_I_DASHING_PARAMETERS_F_FORWARDER_H
#define RVG_I_DASHING_PARAMETERS_F_FORWARDER_H

#include "rvg-i-dashing-parameters.h"

namespace rvg {

template <typename DERIVED>
class i_dashing_parameters_f_forwarder: public i_dashing_parameters<DERIVED> {

    DERIVED &derived(void) {
        return *static_cast<DERIVED *>(this);
    }

    const DERIVED &derived(void) const {
        return *static_cast<const DERIVED *>(this);
    }

public:

    i_dashing_parameters_f_forwarder(void) {
        static_assert(meta::is_an_i_sink<DERIVED>::value,
            "derived class is not an i_sink");
    }

private:

// i_dashing_parameters interface
friend i_dashing_parameters<DERIVED>;

    void do_begin_dash_parameter(rvgf t) {
        return derived().sink().begin_dash_parameter(t);
    }

    void do_end_dash_parameter(rvgf t) {
        return derived().sink().end_dash_parameter(t);
    }

    void do_backward_begin_dash_parameter(rvgf t) {
        return derived().sink().backward_begin_dash_parameter(t);
    }

    void do_backward_end_dash_parameter(rvgf t) {
        return derived().sink().backward_end_dash_parameter(t);
    }


};

} // namespace rvg

#endif
