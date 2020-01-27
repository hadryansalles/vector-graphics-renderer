#ifndef RVG_I_PARAMETERS_F_FORWARDER_H
#define RVG_I_PARAMETERS_F_FORWARDER_H

#include "rvg-i-monotonic-parameters-f-forwarder.h"
#include "rvg-i-cubic-parameters-f-forwarder.h"
#include "rvg-i-offsetting-parameters-f-forwarder.h"
#include "rvg-i-join-parameters-f-forwarder.h"
#include "rvg-i-dashing-parameters-f-forwarder.h"

namespace rvg {

template <typename SINK, typename DERIVED>
class i_parameters_f_forwarder:
    public meta::inherit_if_i_monotonic_parameters<SINK,
        i_monotonic_parameters_f_forwarder<DERIVED>>,
    public meta::inherit_if_i_cubic_parameters<SINK,
        i_cubic_parameters_f_forwarder<DERIVED>>,
    public meta::inherit_if_i_offsetting_parameters<SINK,
        i_offsetting_parameters_f_forwarder<DERIVED>>,
    public meta::inherit_if_i_join_parameters<SINK,
        i_join_parameters_f_forwarder<DERIVED>>,
    public meta::inherit_if_i_dashing_parameters<SINK,
        i_dashing_parameters_f_forwarder<DERIVED>> {

public:

    i_parameters_f_forwarder(void) {
        static_assert(meta::is_an_i_sink<DERIVED>::value,
            "derived class is not an i_sink");
    }

};

} // namespace rvg

#endif
