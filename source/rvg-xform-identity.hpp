inline RP2_tuple identity::do_apply(rvgf x, rvgf y, rvgf w) const {
    return RP2_tuple{x, y, w};
}

inline R2_tuple identity::do_apply(rvgf x, rvgf y) const {
    return R2_tuple{x, y};
}

inline RP2 identity::do_apply(const RP2 &p) const {
    return p;
}

inline R2 identity::do_apply(const R2 &e) const {
    return e;
}

inline identity identity::do_transformed(const identity &) const {
#ifdef XFORM_DEBUG
    std::cerr << "identity.operator*(const identity &)\n";
#endif
    return identity{};
}

inline rotation identity::do_rotated(rvgf cos, rvgf sin) const {
    return rotation{cos, sin};
}

inline translation identity::do_translated(rvgf tx, rvgf ty) const {
    return translation{tx, ty};
}

inline affinity identity::do_windowviewport(const window &w, const viewport &v,
    e_align ax, e_align ay, e_aspect a) const {
    return make_windowviewport(w, v, ax, ay, a) * (*this);
}

inline scaling identity::do_scaled(rvgf sx, rvgf sy) const {
    return scaling{sx, sy};
}

inline bool identity::do_is_equal(const identity &) const {
    return true;
}

inline bool identity::do_is_almost_equal(const identity &) const {
    return true;
}

inline identity identity::do_adjugate(void) const {
    return identity{};
}

inline bool identity::do_is_identity(void) const {
    return true;
}

inline identity identity::do_inverse(void) const {
    return identity{};
}

inline identity identity::do_transpose(void) const {
    return identity{};
}

inline rvgf identity::do_det(void) const {
    return 1.f;
}
