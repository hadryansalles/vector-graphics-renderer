// apply transformation to vector
inline RP2_tuple rotation::do_apply(rvgf x, rvgf y, rvgf w) const {
    return RP2_tuple{m_cos * x - m_sin * y, m_sin * x + m_cos * y, w};
}

inline R2_tuple rotation::do_apply(rvgf x, rvgf y) const {
    return R2_tuple{m_cos * x - m_sin * y, m_sin * x + m_cos * y};
}

inline RP2 rotation::do_apply(const RP2 &p) const {
    return RP2{m_cos * p[0] - m_sin * p[1],
        m_sin * p[0] + m_cos * p[1], p[2]};
}

inline R2 rotation::do_apply(const R2 &e) const {
    return R2{m_cos * e[0] - m_sin * e[1],
        m_sin * e[0] + m_cos * e[1]};
}

inline rotation rotation::do_transformed(const rotation &o) const {
#ifdef XFORM_DEBUG
    std::cerr << "rotation.operator*(const rotation &)\n";
#endif
    return rotation(m_cos*o.m_cos - m_sin*o.m_sin, m_sin*o.m_cos + m_cos*o.m_sin);
}

inline rotation rotation::do_rotated(rvgf cos, rvgf sin) const {
    return do_transformed(rotation{cos, sin});
}

inline linear rotation::do_scaled(rvgf sx, rvgf sy) const {
    return scaling{sx, sy} * (*this);
}

inline affinity rotation::do_translated(rvgf tx, rvgf ty) const {
    return translation{tx, ty} * (*this);
}

inline affinity rotation::do_windowviewport(const window &w, const viewport &v,
    e_align ax, e_align ay, e_aspect a) const {
    return make_windowviewport(w, v, ax, ay, a) * (*this);
}

inline bool rotation::do_is_equal(const rotation &o) const {
    return m_cos == o.m_cos && m_sin == o.m_sin;
}

inline bool rotation::do_is_almost_equal(const rotation &o) const {
    return util::is_almost_equal(m_cos, o.m_cos) &&
           util::is_almost_equal(m_sin, o.m_sin);
}

inline projectivity rotation::do_adjugate(void) const {
    return projectivity{
        m_cos, m_sin, 0,
       -m_sin, m_cos, 0,
            0,     0, do_det()
    };
}

inline bool rotation::do_is_identity(void) const {
    return m_cos == 1.f && m_sin == 0.f;
}

inline rotation rotation::do_inverse(void) const {
    rvgf s = 1.f/do_det();
    return rotation{m_cos*s, -m_sin*s};
}

inline rotation rotation::do_transpose(void) const {
    return rotation{m_cos, -m_sin};
}

inline rvgf rotation::do_det(void) const {
    return m_cos*m_cos + m_sin*m_sin;
}

inline rvgf rotation::get_cos(void) const {
    return m_cos;
}

inline rvgf rotation::get_sin(void) const {
    return m_sin;
}
