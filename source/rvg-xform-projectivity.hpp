inline projectivity projectivity::do_scaled(rvgf sx, rvgf sy) const {
    return scaling{sx, sy} * (*this);
}

inline projectivity projectivity::do_rotated(rvgf cos, rvgf sin) const {
    return rotation{cos,sin} * (*this);
}

inline projectivity projectivity::do_translated(rvgf tx, rvgf ty) const {
    return translation{tx, ty} * (*this);
}

inline projectivity projectivity::do_windowviewport(const window &w,
    const viewport &v, e_align ax, e_align ay, e_aspect a) const {
    return make_windowviewport(w, v, ax, ay, a) * (*this);
}

inline RP2_tuple projectivity::do_apply(rvgf x, rvgf y, rvgf w) const {
    rvgf rx = m_m[0][0] * x + m_m[0][1] * y + m_m[0][2] * w;
    rvgf ry = m_m[1][0] * x + m_m[1][1] * y + m_m[1][2] * w;
    rvgf rw = m_m[2][0] * x + m_m[2][1] * y + m_m[2][2] * w;
    return RP2_tuple{rx, ry, rw};
}

inline RP2 projectivity::do_apply(const RP2 &p) const {
    rvgf rx = m_m[0][0] * p[0] + m_m[0][1] * p[1] + m_m[0][2] * p[2];
    rvgf ry = m_m[1][0] * p[0] + m_m[1][1] * p[1] + m_m[1][2] * p[2];
    rvgf rw = m_m[2][0] * p[0] + m_m[2][1] * p[1] + m_m[2][2] * p[2];
    return RP2{rx, ry, rw};
}

inline RP2_tuple projectivity::do_apply(rvgf x, rvgf y) const {
    rvgf rx = m_m[0][0] * x + m_m[0][1] * y + m_m[0][2];
    rvgf ry = m_m[1][0] * x + m_m[1][1] * y + m_m[1][2];
    rvgf rw = m_m[2][0] * x + m_m[2][1] * y + m_m[2][2];
    return RP2_tuple{rx, ry, rw};
}

inline RP2 projectivity::do_apply(const R2 &e) const {
    rvgf rx = m_m[0][0] * e[0] + m_m[0][1] * e[1] + m_m[0][2];
    rvgf ry = m_m[1][0] * e[0] + m_m[1][1] * e[1] + m_m[1][2];
    rvgf rw = m_m[2][0] * e[0] + m_m[2][1] * e[1] + m_m[2][2];
    return RP2{rx, ry, rw};
}

inline projectivity projectivity::do_transformed(const projectivity &o) const {
#ifdef XFORM_DEBUG
    std::cerr << "projectivity.transformed(const projectivity&)\n";
#endif
    return projectivity(
        m_m[0][0]*o.m_m[0][0] + m_m[1][0]*o.m_m[0][1] + m_m[2][0]*o.m_m[0][2],
        m_m[0][1]*o.m_m[0][0] + m_m[1][1]*o.m_m[0][1] + m_m[2][1]*o.m_m[0][2],
        m_m[0][2]*o.m_m[0][0] + m_m[1][2]*o.m_m[0][1] + m_m[2][2]*o.m_m[0][2],
        m_m[0][0]*o.m_m[1][0] + m_m[1][0]*o.m_m[1][1] + m_m[2][0]*o.m_m[1][2],
        m_m[0][1]*o.m_m[1][0] + m_m[1][1]*o.m_m[1][1] + m_m[2][1]*o.m_m[1][2],
        m_m[0][2]*o.m_m[1][0] + m_m[1][2]*o.m_m[1][1] + m_m[2][2]*o.m_m[1][2],
        m_m[0][0]*o.m_m[2][0] + m_m[1][0]*o.m_m[2][1] + m_m[2][0]*o.m_m[2][2],
        m_m[0][1]*o.m_m[2][0] + m_m[1][1]*o.m_m[2][1] + m_m[2][1]*o.m_m[2][2],
        m_m[0][2]*o.m_m[2][0] + m_m[1][2]*o.m_m[2][1] + m_m[2][2]*o.m_m[2][2]
    );
}

inline bool projectivity::do_is_equal(const projectivity &o) const {
    return m_m == o.m_m;
}

inline bool projectivity::do_is_almost_equal(const projectivity &o) const {
    return util::is_almost_equal(m_m[0][0], o.m_m[0][0]) &&
           util::is_almost_equal(m_m[0][1], o.m_m[0][1]) &&
           util::is_almost_equal(m_m[0][2], o.m_m[0][2]) &&
           util::is_almost_equal(m_m[1][0], o.m_m[1][0]) &&
           util::is_almost_equal(m_m[1][1], o.m_m[1][1]) &&
           util::is_almost_equal(m_m[1][2], o.m_m[1][2]) &&
           util::is_almost_equal(m_m[2][0], o.m_m[2][0]) &&
           util::is_almost_equal(m_m[2][1], o.m_m[2][1]) &&
           util::is_almost_equal(m_m[2][2], o.m_m[2][2]);
}

inline projectivity projectivity::do_adjugate(void) const {
    return projectivity{
       -m_m[1][2]*m_m[2][1] + m_m[1][1]*m_m[2][2],
        m_m[0][2]*m_m[2][1] - m_m[0][1]*m_m[2][2],
       -m_m[0][2]*m_m[1][1] + m_m[0][1]*m_m[1][2],
        m_m[1][2]*m_m[2][0] - m_m[1][0]*m_m[2][2],
       -m_m[0][2]*m_m[2][0] + m_m[0][0]*m_m[2][2],
        m_m[0][2]*m_m[1][0] - m_m[0][0]*m_m[1][2],
       -m_m[1][1]*m_m[2][0] + m_m[1][0]*m_m[2][1],
        m_m[0][1]*m_m[2][0] - m_m[0][0]*m_m[2][1],
       -m_m[0][1]*m_m[1][0] + m_m[0][0]*m_m[1][1]
    };
}

inline projectivity projectivity::do_inverse(void) const {
    rvgf s = 1.f/do_det();
    return projectivity{
       (-m_m[1][2]*m_m[2][1] + m_m[1][1]*m_m[2][2])*s,
       ( m_m[0][2]*m_m[2][1] - m_m[0][1]*m_m[2][2])*s,
       (-m_m[0][2]*m_m[1][1] + m_m[0][1]*m_m[1][2])*s,
       ( m_m[1][2]*m_m[2][0] - m_m[1][0]*m_m[2][2])*s,
       (-m_m[0][2]*m_m[2][0] + m_m[0][0]*m_m[2][2])*s,
       ( m_m[0][2]*m_m[1][0] - m_m[0][0]*m_m[1][2])*s,
       (-m_m[1][1]*m_m[2][0] + m_m[1][0]*m_m[2][1])*s,
       ( m_m[0][1]*m_m[2][0] - m_m[0][0]*m_m[2][1])*s,
       (-m_m[0][1]*m_m[1][0] + m_m[0][0]*m_m[1][1])*s
    };
}

inline rvgf projectivity::do_det(void) const {
    return -m_m[0][2]*m_m[1][1]*m_m[2][0] + m_m[0][1]*m_m[1][2]*m_m[2][0] +
        m_m[0][2]*m_m[1][0]*m_m[2][1] - m_m[0][0]*m_m[1][2]*m_m[2][1] -
        m_m[0][1]*m_m[1][0]*m_m[2][2] + m_m[0][0]*m_m[1][1]*m_m[2][2];
}

inline bool projectivity::do_is_identity(void) const {
    return m_m[0][0] == 1.f && m_m[0][1] == 0.f && m_m[0][2] == 0.f &&
           m_m[1][0] == 0.f && m_m[1][1] == 1.f && m_m[1][2] == 0.f &&
           m_m[2][0] == 0.f && m_m[2][1] == 0.f && m_m[2][2] == 1.f;
}

inline projectivity projectivity::do_transpose(void) const {
    return projectivity{
        m_m[0][0], m_m[1][0], m_m[2][0],
        m_m[0][1], m_m[1][1], m_m[2][1],
        m_m[0][2], m_m[1][2], m_m[2][2]
    };
}

inline const std::array<rvgf,3> &projectivity::operator[](int i) const {
    return m_m[i];
}
