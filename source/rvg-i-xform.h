#ifndef RVG_I_XFORM_H
#define RVG_I_XFORM_H

#include <utility>
#include <iosfwd>

#include "rvg-floatint.h"
#include "rvg-meta.h"
#include "rvg-point.h"
#include "rvg-util.h"
#include "rvg-window.h"
#include "rvg-viewport.h"
#include "rvg-align.h"
#include "rvg-aspect.h"

namespace rvg {

template <typename DERIVED>
class i_xform {
    DERIVED &derived(void) {
        return *static_cast<DERIVED *>(this);
    }

    const DERIVED &derived(void) const {
        return *static_cast<const DERIVED *>(this);
    }
public:
    // apply transformation to vector components
    auto apply(rvgf x, rvgf y, rvgf w) const {
        return derived().do_apply(x, y, w);
    }

    auto apply(rvgf x, rvgf y) const {
        return derived().do_apply(x, y);
    }

    // apply transformation to vector
    auto apply(const RP2 &p) const {
        return derived().do_apply(p);
    }

    auto apply(const R2 &e) const {
        return derived().do_apply(e);
    }

    // apply rotation
    auto rotated(rvgf deg) const {
        rvgf rad = util::rad(deg);
        return derived().do_rotated(std::cos(rad), std::sin(rad));
    }

    // apply rotation
    auto rotated(rvgf cos, rvgf sin) const {
        return derived().do_rotated(cos, sin);
    }

    // apply scaling
    auto scaled(rvgf sx, rvgf sy) const {
        return derived().do_scaled(sx, sy);
    }

    auto scaled(rvgf s) const {
        return derived().do_scaled(s, s);
    }

    // apply translation
    auto translated(rvgf tx, rvgf ty) const {
        return derived().do_translated(tx, ty);
    }

    // apply window-viewport
    auto windowviewport(const window &w, const viewport &v,
        e_align ax = e_align::mid, e_align ay = e_align::mid,
        e_aspect a = e_aspect::none) const {
        return derived().do_windowviewport(w, v, ax, ay, a);
    }

    // apply derived transformation to derived transformation
    DERIVED transformed(const DERIVED &o) const {
        return derived().do_transformed(o);
    }

    DERIVED operator*(const DERIVED &o) const {
        return o.transformed(derived());
    }

    auto operator*(rvgf s) const {
        return derived().do_scaled(s);
    }

    auto operator*(const R2 &r2) const {
        return derived().do_apply(r2);
    }

    auto operator*(const RP2 &rp2) const {
        return derived().do_apply(rp2);
    }

    bool operator==(const DERIVED &o) const {
        return derived().do_is_equal(o);
    }

    bool operator!=(const DERIVED &o) const {
        return !derived().do_is_equal(o);
    }

    // print to ostream
    std::ostream &print(std::ostream &out) const {
        return derived().do_print(out);
    }

    bool is_almost_equal(const DERIVED &o) const {
        return derived().do_is_almost_equal(o);
    }

    bool is_equal(const DERIVED &o) const {
        return derived().do_is_equal(o);
    }

    rvgf det(void) const {
        return derived().do_det();
    }

    // return matrix adjugate where A * A.ajugate() == A.det() * Identity()
    auto adjugate(void) const {
        return derived().do_adjugate();
    }

    // return matrix transform
    auto transpose(void) const {
        return derived().do_transpose();
    }

    // compute inverse transformation
    DERIVED inverse(void) const {
        return derived().do_inverse();
    }

    bool is_identity(void) const {
        return derived().do_is_identity();
    }

    DERIVED get_xf(void) const {
        return derived();
    }
};

template <typename DERIVED>
std::ostream &operator<<(std::ostream &out, const i_xform<DERIVED> &xf) {
    return xf.print(out);
}

} // namespace rvg

namespace rvg {
    namespace meta {

template <typename DERIVED>
using is_an_i_xform = std::integral_constant<
    bool,
    is_crtp_of<
        rvg::i_xform,
        typename remove_reference_cv<DERIVED>::type
    >::value>;

} } // rvg::meta

#endif
