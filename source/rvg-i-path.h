#ifndef RVG_I_PATH_H
#define RVG_I_PATH_H

#include "rvg-meta.h"
#include "rvg-i-input-path.h"
#include "rvg-i-regular-path.h"
#include "rvg-i-decorated-path.h"
#include "rvg-i-monotonic-parameters.h"
#include "rvg-i-cubic-parameters.h"
#include "rvg-i-offsetting-parameters.h"
#include "rvg-i-join-parameters.h"
#include "rvg-i-dashing-parameters.h"

namespace rvg {

template <typename DERIVED>
class i_path:
    public i_input_path<DERIVED>,
    public i_regular_path<DERIVED>,
    public i_decorated_path<DERIVED>,
    public i_monotonic_parameters<DERIVED>,
    public i_cubic_parameters<DERIVED>,
    public i_offsetting_parameters<DERIVED>,
    public i_join_parameters<DERIVED>,
    public i_dashing_parameters<DERIVED> { };
}

namespace rvg {
    namespace meta {

template <typename DERIVED>
using is_an_i_path = std::integral_constant<
    bool,
    is_an_i_input_path<DERIVED>::value &&
    is_an_i_regular_path<DERIVED>::value &&
    is_an_i_decorated_path<DERIVED>::value &&
    is_an_i_monotonic_parameters<DERIVED>::value &&
    is_an_i_cubic_parameters<DERIVED>::value &&
    is_an_i_offsetting_parameters<DERIVED>::value &&
    is_an_i_join_parameters<DERIVED>::value &&
    is_an_i_dashing_parameters<DERIVED>::value>;

} } // namespace rvg::meta

#endif
