#ifndef RVG_XFORM_IDENTITY_H
#define RVG_XFORM_IDENTITY_H

#include "rvg-i-xform.h"

namespace rvg {

class identity final: public i_xform<identity> {
public:
    // promotions
    operator scaling() const {
#ifdef XFORM_DEBUG
        std::cerr << "identity.operator scaling()\n";
#endif
        return scaling(1.0f, 1.0f);
    }

    operator rotation() const {
#ifdef XFORM_DEBUG
        std::cerr << "identity.operator rotation()\n";
#endif
        return rotation(1.f, 0.f);
    }

    operator translation() const {
#ifdef XFORM_DEBUG
        std::cerr << "identity.operator translation()\n";
#endif
        return translation(0.f, 0.f);
    }

    operator linear() const {
#ifdef XFORM_DEBUG
        std::cerr << "identity.operator linear()\n";
#endif
        return linear(1.f, 0.f, 0.f, 1.f);
    }

    operator affinity() const {
#ifdef XFORM_DEBUG
        std::cerr << "identity.operator affinity()\n";
#endif
        return affinity(1.f, 0.f, 0.f, 0.f, 1.f, 0.f);
    }

    operator projectivity() const {
#ifdef XFORM_DEBUG
        std::cerr << "identity.operator projectivity()\n";
#endif
        return projectivity(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f);
    }

    // constructors
    identity() = default;

private:
    friend i_xform<identity>;

    RP2_tuple do_apply(rvgf x, rvgf y, rvgf w) const;

    R2_tuple do_apply(rvgf x, rvgf y) const;

    RP2 do_apply(const RP2 &p) const;

    R2 do_apply(const R2 &e) const;

    identity do_transformed(const identity &) const;

    rotation do_rotated(rvgf cos, rvgf sin) const;

    translation do_translated(rvgf tx, rvgf ty) const;

    scaling do_scaled(rvgf sx, rvgf sy) const;

    affinity do_windowviewport(const window &w, const viewport& v,
        e_align ax, e_align ay, e_aspect a) const;

    std::ostream &do_print(std::ostream &out) const;

    bool do_is_equal(const identity &) const;

    bool do_is_almost_equal(const identity &) const;

    bool do_is_identity(void) const;

    identity do_transpose(void) const;

    identity do_adjugate(void) const;

    identity do_inverse(void) const;

    rvgf do_det(void) const;
};

} // namespace rvg

#endif
