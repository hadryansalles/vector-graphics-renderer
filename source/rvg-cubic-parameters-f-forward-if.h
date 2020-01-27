#ifndef RVG_CUBIC_PARAMETERS_FORWARD_IF_H
#define RVG_CUBIC_PARAMETERS_FORWARD_IF_H

#include "rvg-i-cubic-parameters.h"
#include "rvg-path-f-null.h"

namespace rvg {

//?? D maybe use traditional overloading here?
template <typename SINK,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_cubic_parameters<SINK>::value
    >::type
>
SINK &
make_cubic_parameters_f_forward_if(SINK &sink) {
    return sink;
}

template <typename SINK,
    typename = typename std::enable_if<
        !rvg::meta::is_an_i_cubic_parameters<SINK>::value
    >::type
>
path_f_null &
make_cubic_parameters_f_forward_if(SINK &) {
    static path_f_null pfn{};
    return pfn;
}

} // namespace rvg

#endif
