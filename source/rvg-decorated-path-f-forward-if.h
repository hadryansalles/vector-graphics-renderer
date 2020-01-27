#ifndef RVG_DECORATED_PATH_FORWARD_IF_H
#define RVG_DECORATED_PATH_FORWARD_IF_H

#include "rvg-i-decorated-path.h"
#include "rvg-path-f-null.h"

namespace rvg {

template <typename SINK,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_decorated_path<SINK>::value
    >::type
>
SINK &
make_decorated_path_f_forward_if(SINK &sink) {
    return sink;
}

template <typename SINK,
    typename = typename std::enable_if<
        !rvg::meta::is_an_i_decorated_path<SINK>::value
        >::type
    >
path_f_null &
make_decorated_path_f_forward_if(SINK &) {
    static path_f_null pfn{};
    return pfn;
}

} // namespace rvg

#endif
