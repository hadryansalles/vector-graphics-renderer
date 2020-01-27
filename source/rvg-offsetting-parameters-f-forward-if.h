#ifndef RVG_OFFSETTING_PARAMETERS_F_FORWARD_IF_H
#define RVG_OFFSETTING_PARAMETERS_F_FORWARD_IF_H

#include "rvg-i-offsetting-parameters.h"
#include "rvg-path-f-null.h"

namespace rvg {

//?? D maybe use traditional overloading here?
template <typename SINK,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_offsetting_parameters<SINK>::value
    >::type
>
SINK &
make_offsetting_parameters_f_forward_if(SINK &sink) {
    return sink;
}

template <typename SINK,
    typename = typename std::enable_if<
        !rvg::meta::is_an_i_offsetting_parameters<SINK>::value
    >::type
>
path_f_null &
make_offsetting_parameters_f_forward_if(SINK &) {
    static path_f_null pfn{};
    return pfn;
}

} // namespace rvg

#endif
