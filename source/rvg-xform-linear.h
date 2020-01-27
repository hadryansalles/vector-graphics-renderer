#ifndef RVG_XFORM_LINEAR_H
#define RVG_XFORM_LINEAR_H

#include <array>

#include "rvg-i-xform.h"

namespace rvg {

class linear final: public i_xform<linear> {

    std::array<std::array<rvgf,2>,2> m_m;

public:
    // constructors
    linear(const linear &l) = default;

    explicit linear(const std::array<std::array<rvgf,2>,2> &m): m_m(m) {
#ifdef XFORM_DEBUG
        std::cerr << "linear(const std::array<std::array<rvgf,2>,2> &)\n";
#endif
    }

    linear(const R2 &c0, const R2 &c1):
        linear{ {{ {{c0[0],c1[0]}}, {{c0[1],c1[1]}} }} } {
#ifdef XFORM_DEBUG
        std::cerr << "linear(const R2 &, const R2 &)\n";
#endif
    }

    linear(rvgf a, rvgf b, rvgf c, rvgf d):
        linear{ {{ {{a, b}} ,{{c, d}} }}} {
#ifdef XFORM_DEBUG
        std::cerr << "linear(rvgf, ..., rvgf)\n";
#endif
    }

    linear(): linear{ {{ {{1.f, 0.f}}, {{0.f, 1.f}} }} } {
#ifdef XFORM_DEBUG
        std::cerr << "linear()\n";
#endif
    }

    // promotions
    operator affinity() const {
#ifdef XFORM_DEBUG
        std::cerr << "linear.operator affinity()\n";
#endif
        return affinity{m_m[0][0], m_m[0][1], 0.f, m_m[1][0], m_m[1][1], 0.f};
    }

    operator projectivity() const {
#ifdef XFORM_DEBUG
        std::cerr << "linear.operator projectivity()\n";
#endif
        return projectivity{m_m[0][0], m_m[0][1], 0.f, m_m[1][0], m_m[1][1], 0.f, 0.f, 0.f, 1.f};
    }

    const std::array<rvgf,2> &operator[](int i) const;

private:
    friend i_xform<linear>;

    RP2_tuple do_apply(rvgf x, rvgf y, rvgf w) const;

    R2_tuple do_apply(rvgf x, rvgf y) const;

    RP2 do_apply(const RP2 &p) const;

    R2 do_apply(const R2 &e) const;

    linear do_transformed(const linear &o) const;

    linear do_rotated(rvgf cos, rvgf sin) const;

    linear do_scaled(rvgf sx, rvgf sy) const;

    affinity do_translated(rvgf tx, rvgf ty) const;

    affinity do_windowviewport(const window &w, const viewport& v,
        e_align align_x, e_align align_y, e_aspect aspect) const;

    std::ostream &do_print(std::ostream &out) const;

    bool do_is_equal(const linear &o) const;

    bool do_is_almost_equal(const linear &o) const;

    bool do_is_identity(void) const;

    linear do_adjugate(void) const;

    linear do_transpose(void) const;

    linear do_inverse(void) const;

    rvgf do_det(void) const;
};


} // namespace rvg

#endif
