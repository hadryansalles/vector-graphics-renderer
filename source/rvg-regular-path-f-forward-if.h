#ifndef RVG_REGULAR_PATH_FORWARD_IF_H
#define RVG_REGULAR_PATH_FORWARD_IF_H

#include "rvg-i-regular-path.h"
#include "rvg-path-f-null.h"

namespace rvg {

//?? D maybe use traditional overloading here?
template <typename SINK,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_regular_path<SINK>::value
    >::type
>
SINK &
make_regular_path_f_forward_if(SINK &sink) {
    return sink;
}

template <typename SINK,
    typename = typename std::enable_if<
        !rvg::meta::is_an_i_regular_path<SINK>::value
    >::type
>
path_f_null &
make_regular_path_f_forward_if(SINK &) {
    static path_f_null pfn{};
    return pfn;
}

} // namespace rvg

#endif
