#ifndef RVG_I_JOIN_PARAMETERS_F_FORWARDER_H
#define RVG_I_JOIN_PARAMETERS_F_FORWARDER_H

#include "rvg-i-join-parameters.h"

namespace rvg {

template <typename DERIVED>
class i_join_parameters_f_forwarder: public i_join_parameters<DERIVED> {

    DERIVED &derived(void) {
        return *static_cast<DERIVED *>(this);
    }

    const DERIVED &derived(void) const {
        return *static_cast<const DERIVED *>(this);
    }

public:

    i_join_parameters_f_forwarder(void) {
        static_assert(meta::is_an_i_sink<DERIVED>::value,
            "derived class is not an i_sink");
    }

private:

// i_join_parameters interface
friend i_join_parameters<DERIVED>;

    void do_join_tangent_parameter(rvgf t) {
        return derived().sink().join_tangent_parameter(t);
    }

    void do_join_vertex_parameter(rvgf t) {
        return derived().sink().join_vertex_parameter(t);
    }

};

} // namespace rvg

#endif
