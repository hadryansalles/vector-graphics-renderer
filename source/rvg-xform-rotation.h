#ifndef RVG_XFORM_ROTATION_H
#define RVG_XFORM_ROTATION_H

#include "rvg-util.h"

#include "rvg-i-xform.h"

namespace rvg {

class rotation final: public i_xform<rotation> {

    rvgf m_cos, m_sin;

public:
    // constructors
    rotation(const rotation &r) = default;

    rotation(rvgf cos, rvgf sin): m_cos(cos), m_sin(sin) {
#ifdef XFORM_DEBUG
        std::cerr << "rotation(rvgf, rvgf)\n";
#endif
    }

    rotation(): rotation{1.f, 0.f} {
#ifdef XFORM_DEBUG
        std::cerr << "rotation()\n";
#endif
    }

    explicit rotation(rvgf deg) {
        rvgf rad = util::rad(deg);
        m_cos = std::cos(rad);
        m_sin = std::sin(rad);
#ifdef XFORM_DEBUG
        std::cerr << "explicit rotation(rvgf)\n";
#endif
    }

    // promotions
    operator linear() const {
#ifdef XFORM_DEBUG
        std::cerr << "rotation.operator linear()\n";
#endif
        return linear(m_cos, -m_sin, m_sin, m_cos);
    }

    operator affinity() const {
#ifdef XFORM_DEBUG
        std::cerr << "rotation.operator affinity()\n";
#endif
        return affinity(m_cos, -m_sin, 0.f, m_sin, m_cos, 0.f);
    }

    operator projectivity() const {
#ifdef XFORM_DEBUG
        std::cerr << "rotation.operator projectivity()\n";
#endif
        return projectivity(m_cos, -m_sin, 0.f, m_sin, m_cos, 0.f, 0.f, 0.f, 1.f);
    }

    rvgf get_cos(void) const;

    rvgf get_sin(void) const;

private:
    friend i_xform<rotation>;

    RP2_tuple do_apply(rvgf x, rvgf y, rvgf w) const;

    R2_tuple do_apply(rvgf x, rvgf y) const;

    RP2 do_apply(const RP2 &p) const;

    R2 do_apply(const R2 &e) const;

    rotation do_transformed(const rotation &o) const;

    rotation do_rotated(rvgf cos, rvgf sin) const;

    linear do_scaled(rvgf sx, rvgf sy) const;

    affinity do_translated(rvgf tx, rvgf ty) const;

    affinity do_windowviewport(const window &w, const viewport& v,
        e_align align_x, e_align align_y, e_aspect aspect) const;

    std::ostream &do_print(std::ostream &out) const;

    bool do_is_equal(const rotation &o) const;

    bool do_is_almost_equal(const rotation &o) const;

    rotation do_transpose(void) const;

    bool do_is_identity(void) const;

    projectivity do_adjugate(void) const;

    rotation do_inverse(void) const;

    rvgf do_det(void) const;
};

} // namespace rvg

#endif
