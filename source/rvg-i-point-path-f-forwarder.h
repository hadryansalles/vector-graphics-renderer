#ifndef RVG_I_POINT_PATH_F_FORWARDER_H
#define RVG_I_POINT_PATH_F_FORWARDER_H

#include "rvg-i-point-input-path-f-forwarder.h"
#include "rvg-i-point-regular-path-f-forwarder.h"
#include "rvg-i-point-decorated-path-f-forwarder.h"
#include "rvg-i-monotonic-parameters-f-forwarder.h"
#include "rvg-i-cubic-parameters-f-forwarder.h"
#include "rvg-i-offsetting-parameters-f-forwarder.h"
#include "rvg-i-join-parameters-f-forwarder.h"
#include "rvg-i-dashing-parameters-f-forwarder.h"

namespace rvg {

template <typename DERIVED>
class i_point_path_f_forwarder:
    public i_point_input_path_f_forwarder<DERIVED>,
    public i_point_regular_path_f_forwarder<DERIVED>,
    public i_point_decorated_path_f_forwarder<DERIVED>,
    public i_monotonic_parameters_f_forwarder<DERIVED>,
    public i_offsetting_parameters_f_forwarder<DERIVED>,
    public i_join_parameters_f_forwarder<DERIVED>,
    public i_dashing_parameters_f_forwarder<DERIVED>,
    public i_cubic_parameters_f_forwarder<DERIVED> {

};

} // namespace rvg

#endif
