#ifndef RVG_I_XFORMABLE_H
#define RVG_I_XFORMABLE_H

#include "rvg-xform.h"

namespace rvg {

template <typename DERIVED>
class i_xformable {

    DERIVED &derived(void) {
        return *static_cast<DERIVED *>(this);
    }

    const DERIVED &derived(void) const {
        return *static_cast<const DERIVED *>(this);
    }

public:

    DERIVED rotated(rvgf deg) const & {
        auto copy = derived();
        copy.set_xf(m_xf.rotated(deg));
        return copy;
    }

    DERIVED &&rotated(rvgf deg) && {
        set_xf(m_xf.rotated(deg));
        return std::move(derived());
    }

    DERIVED rotated(rvgf deg, rvgf cx, rvgf cy) const & {
        auto copy = derived();
        copy.set_xf(m_xf.translated(-cx, -cy).rotated(deg).translated(cx, cy));
        return copy;
    }

    DERIVED &&rotated(rvgf deg, rvgf cx, rvgf cy) && {
        set_xf(m_xf.translated(-cx, -cy).rotated(deg).translated(cx, cy));
        return std::move(derived());
    }

    DERIVED translated(rvgf tx, rvgf ty) const & {
        auto copy = derived();
        copy.set_xf(m_xf.translated(tx, ty));
        return copy;
    }

    DERIVED &&translated(rvgf tx, rvgf ty) && {
        set_xf(m_xf.translated(tx, ty));
        return std::move(derived());
    }

    DERIVED scaled(rvgf sx, rvgf sy, rvgf cx, rvgf cy) const & {
        auto copy = derived();
        copy.set_xf(m_xf.translated(-cx, -cy).scaled(sx, sy).
            translated(cx, cy));
        return copy;
    }

    DERIVED &&scaled(rvgf sx, rvgf sy, rvgf cx, rvgf cy) && {
        set_xf(m_xf.translated(-cx, -cy).scaled(sx, sy).translated(cx, cy));
        return std::move(derived());
    }

    DERIVED scaled(rvgf s, rvgf cx, rvgf cy) const & {
        auto copy = derived();
        copy.set_xf(m_xf.translated(-cx, -cy).scaled(s).
            translated(cx, cy));
        return copy;
    }

    DERIVED &&scaled(rvgf s, rvgf cx, rvgf cy) && {
        set_xf(m_xf.translated(-cx, -cy).scaled(s).translated(cx, cy));
        return std::move(derived());
    }

    DERIVED scaled(rvgf sx, rvgf sy) const & {
        auto copy = derived();
        copy.set_xf(m_xf.scaled(sx, sy));
        return copy;
    }

    DERIVED &&scaled(rvgf sx, rvgf sy) && {
        set_xf(m_xf.scaled(sx, sy));
        return std::move(derived());
    }

    DERIVED scaled(rvgf s) const & {
        auto copy = derived();
        copy.set_xf(m_xf.scaled(s, s));
        return copy;
    }

    DERIVED &&scaled(rvgf s) && {
        set_xf(m_xf.scaled(s, s));
        return std::move(derived());
    }

    DERIVED transformed(const xform &other) const & {
        auto copy = derived();
        copy.set_xf(m_xf.transformed(other));
        return copy;
    }

    DERIVED &&transformed(const xform &other) && {
        set_xf(m_xf.transformed(other));
        return std::move(derived());
    }

    DERIVED affine(rvgf a, rvgf b, rvgf tx,
        rvgf c, rvgf d, rvgf ty) const & {
        auto copy = derived();
        copy.set_xf(m_xf.transformed(affinity(a, b, tx, c, d, ty)));
        return copy;
    }

    DERIVED &&affine(rvgf a, rvgf b, rvgf tx,
        rvgf c, rvgf d, rvgf ty) && {
        set_xf(m_xf.transformed(affinity(a, b, tx, c, d, ty)));
        return std::move(derived());
    }

    DERIVED &linear(rvgf a, rvgf b, rvgf c, rvgf d) const & {
        auto copy = derived();
        copy.set_xf(m_xf.transformed(linear(a, b, c, d)));
        return copy;
    }

    DERIVED &&linear(rvgf a, rvgf b, rvgf c, rvgf d) && {
        set_xf(m_xf.transformed(linear(a, b, c, d)));
        return std::move(derived());
    }

    DERIVED windowviewport(const window &wnd, const viewport &vp,
        e_align ax = e_align::mid, e_align ay = e_align::mid,
        e_aspect a = e_aspect::none) const & {
        auto copy = derived();
        xform wv = make_windowviewport(wnd, vp, ax, ay, a);
        copy.set_xf(wv * m_xf);
        return copy;
    }

    DERIVED &&windowviewport(const window &wnd, const viewport &vp,
        e_align ax = e_align::mid, e_align ay = e_align::mid,
        e_aspect a = e_aspect::none) && {
        xform wv = make_windowviewport(wnd, vp, ax, ay, a);
        set_xf(wv * m_xf);
        return std::move(derived());
    }

    const xform &get_xf(void) const {
        return m_xf;
    }

    void set_xf(const xform &xf) {
        m_xf = xf;
    }

private:
    xform m_xf;
};

} // namespace rvg

#endif
