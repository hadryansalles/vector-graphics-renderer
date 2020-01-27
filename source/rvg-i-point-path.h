#ifndef RVG_I_POINT_PATH_H
#define RVG_I_POINT_PATH_H

#include "rvg-meta.h"
#include "rvg-i-point-input-path.h"
#include "rvg-i-point-regular-path.h"
#include "rvg-i-point-decorated-path.h"

namespace rvg {

template <typename DERIVED>
class i_point_path:
    public i_point_input_path<DERIVED>,
    public i_point_regular_path<DERIVED>,
    public i_point_decorated_path<DERIVED> {

};


} // namespace rvg

namespace rvg {
    namespace meta {

template <typename DERIVED>
using is_an_i_point_path = std::integral_constant<
    bool,
    is_an_i_point_input_path<DERIVED>::value &&
    is_an_i_point_regular_path<DERIVED>::value &&
    is_an_i_point_decorated_path<DERIVED>::value>;

} } // namespace rvg::meta

#endif
