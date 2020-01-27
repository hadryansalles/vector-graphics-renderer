// apply transformation to vector
inline RP2_tuple scaling::do_apply(rvgf x, rvgf y, rvgf w) const {
    return RP2_tuple{x * m_sx, y * m_sy, w};
}

inline R2_tuple scaling::do_apply(rvgf x, rvgf y) const {
    return R2_tuple{x * m_sx, y * m_sy};
}

inline RP2 scaling::do_apply(const RP2 &p) const {
    return RP2{p[0] * m_sx, p[1] * m_sy, p[2]};
}

inline R2 scaling::do_apply(const R2 &e) const {
    return R2{e[0] * m_sx, e[1] * m_sy};
}

inline scaling scaling::do_transformed(const scaling &o) const {
#ifdef XFORM_DEBUG
    std::cerr << "scaling.operator*(const scaling &)\n";
#endif
    return scaling(m_sx*o.m_sx, m_sy*o.m_sy);
}

inline linear scaling::do_rotated(rvgf cos, rvgf sin) const {
    return rotation{cos, sin} * (*this);
}

inline scaling scaling::do_scaled(rvgf sx, rvgf sy) const {
    return scaling(sx, sy) * (*this);
}

inline affinity scaling::do_translated(rvgf tx, rvgf ty) const {
    return translation(tx, ty) * (*this);
}

inline affinity scaling::do_windowviewport(const window &w, const viewport &v,
    e_align ax, e_align ay, e_aspect a) const {
    return make_windowviewport(w, v, ax, ay, a) * (*this);
}

inline bool scaling::do_is_almost_equal(const scaling &o) const {
    return util::is_almost_equal(m_sx, o.m_sx) &&
           util::is_almost_equal(m_sy, o.m_sy);
}

inline bool scaling::do_is_equal(const scaling &o) const {
    return m_sx == o.m_sx && m_sy == o.m_sy;
}

inline projectivity scaling::do_adjugate(void) const {
    return projectivity{m_sy, 0, 0, 0, m_sx, 0, 0, 0, do_det()};
}

inline scaling scaling::do_transpose(void) const {
    return *this;
}

inline bool scaling::do_is_identity(void) const {
    return m_sx == 1.f && m_sy == 1.f;
}

inline scaling scaling::do_inverse(void) const {
    return scaling{1.f/m_sx, 1.f/m_sy};
}

inline rvgf scaling::do_det(void) const {
    return m_sx*m_sy;
}

inline rvgf scaling::get_sx(void) const {
    return m_sx;
}

inline rvgf scaling::get_sy(void) const {
    return m_sy;
}
