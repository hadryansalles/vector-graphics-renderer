#ifndef RVG_XFORM_AFFINITY_H
#define RVG_XFORM_AFFINITY_H

#include "rvg-i-xform.h"

namespace rvg {

class affinity final: public i_xform<affinity> {

    std::array<std::array<rvgf,3>,2> m_m;

public:
    // constructors
    affinity(const affinity &a) = default;

    explicit affinity(const std::array<std::array<rvgf,3>,2> &m): m_m(m) {
#ifdef XFORM_DEBUG
        std::cerr << "affinity(const std::array<std::array<rvgf,3>,2> &)\n";
#endif
    }

    affinity(const R2 &c0, const R2 &c1, const R2 &c2):
        affinity{ {{ {{c0[0],c1[0],c2[0]}}, {{c0[1],c1[1],c2[1]}} }} } {
#ifdef XFORM_DEBUG
        std::cerr << "affinity(const R2 &, const R2 &, const R2 &)\n";
#endif
    }

    affinity(rvgf a, rvgf b, rvgf tx, rvgf c, rvgf d, rvgf ty):
        affinity{ {{ {{a,b,tx}} , {{c,d,ty}} }} } {
#ifdef XFORM_DEBUG
        std::cerr << "affinity(rvgf, ..., rvgf)\n";
#endif
    }

    affinity(): affinity{ {{ {{1.f,0.f,0.f}}, {{0.f,1.f,0.f}} }} } {
#ifdef XFORM_DEBUG
        std::cerr << "affinity()\n";
#endif
    }

    // promotion
    operator projectivity() const {
#ifdef XFORM_DEBUG
        std::cerr << "affinity.operator projectivity()\n";
#endif
        return projectivity{ {{m_m[0], m_m[1], {{0, 0, 1}} }} };
    }

    const std::array<rvgf, 3> &operator[](int i) const;

private:
    friend i_xform<affinity>;

    RP2_tuple do_apply(rvgf x, rvgf y, rvgf w) const;

    R2_tuple do_apply(rvgf x, rvgf y) const;

    RP2 do_apply(const RP2 &p) const;

    R2 do_apply(const R2 &e) const;

    affinity do_transformed(const affinity &o) const;

    affinity do_rotated(rvgf cos, rvgf sin) const;

    affinity do_scaled(rvgf sx, rvgf sy) const;

    affinity do_translated(rvgf tx, rvgf ty) const;

    affinity do_windowviewport(const window &w, const viewport& v,
        e_align align_x, e_align align_y, e_aspect aspect) const;

    std::ostream &do_print(std::ostream &out) const;

    bool do_is_equal(const affinity &o) const;

    bool do_is_almost_equal(const affinity &o) const;

    bool do_is_identity(void) const;

    projectivity do_adjugate(void) const;

    projectivity do_transpose(void) const;

    affinity do_inverse(void) const;

    rvgf do_det(void) const;
};

} // namespace rvg

#endif
