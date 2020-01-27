#ifndef RVG_I_CUBIC_PARAMETERS_H
#define RVG_I_CUBIC_PARAMETERS_H

#include "rvg-meta.h"
#include "rvg-floatint.h"

namespace rvg {

template <typename DERIVED>
class i_cubic_parameters {

    DERIVED &derived(void) {
        return *static_cast<DERIVED *>(this);
    }

    const DERIVED &derived(void) const {
        return *static_cast<const DERIVED *>(this);
    }

public:

    void inflection_parameter(rvgf t) {
        return derived().do_inflection_parameter(t);
    }

    void double_point_parameter(rvgf t) {
        return derived().do_double_point_parameter(t);
    }

};

}

namespace rvg {
    namespace meta {

template <typename DERIVED>
using is_an_i_cubic_parameters = std::integral_constant<
    bool,
    is_template_base_of<
        rvg::i_cubic_parameters,
        typename remove_reference_cv<DERIVED>::type
    >::value>;

namespace detail {
	class not_an_i_cubic_parameters { };
}

template <typename U, typename T>
using inherit_if_i_cubic_parameters =
	typename std::conditional<
		is_an_i_cubic_parameters<U>::value,
		T,
		detail::not_an_i_cubic_parameters
	>::type;


} } // namespace rvg::meta

#endif
