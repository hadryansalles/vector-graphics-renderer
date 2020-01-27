inline linear linear::do_scaled(rvgf sx, rvgf sy) const {
    return scaling{sx, sy} * (*this);
}

inline linear linear::do_rotated(rvgf cos, rvgf sin) const {
    return rotation{cos, sin} * (*this);
}

inline affinity linear::do_translated(rvgf tx, rvgf ty) const {
    return translation{tx, ty} * (*this);
}

inline affinity linear::do_windowviewport(const window &w, const viewport &v,
    e_align ax, e_align ay, e_aspect a) const {
    return make_windowviewport(w, v, ax, ay, a) * (*this);
}

inline RP2_tuple linear::do_apply(rvgf x, rvgf y, rvgf w) const {
    rvgf rx = m_m[0][0] * x + m_m[0][1] * y;
    rvgf ry = m_m[1][0] * x + m_m[1][1] * y;
    return RP2_tuple{rx, ry, w};
}

inline RP2 linear::do_apply(const RP2 &p) const {
    rvgf rx = m_m[0][0] * p[0] + m_m[0][1] * p[1];
    rvgf ry = m_m[1][0] * p[0] + m_m[1][1] * p[1];
    return RP2{rx, ry, p[2]};
}

inline R2_tuple linear::do_apply(rvgf x, rvgf y) const {
    rvgf rx = m_m[0][0] * x + m_m[0][1] * y;
    rvgf ry = m_m[1][0] * x + m_m[1][1] * y;
    return R2_tuple{rx, ry};
}

inline R2 linear::do_apply(const R2 &e) const {
    rvgf rx = m_m[0][0] * e[0] + m_m[0][1] * e[1];
    rvgf ry = m_m[1][0] * e[0] + m_m[1][1] * e[1];
    return R2{rx, ry};
}

inline linear linear::do_transformed(const linear &o) const {
#ifdef XFORM_DEBUG
std::cerr << "linear.operator*(linear)\n";
#endif
    return linear{m_m[0][0]*o.m_m[0][0] + m_m[1][0]*o.m_m[0][1],
        m_m[0][1]*o.m_m[0][0] + m_m[1][1]*o.m_m[0][1],
        m_m[0][0]*o.m_m[1][0] + m_m[1][0]*o.m_m[1][1],
        m_m[0][1]*o.m_m[1][0] + m_m[1][1]*o.m_m[1][1]};
}

inline bool linear::do_is_almost_equal(const linear &o) const {
    return util::is_almost_equal(m_m[0][0], o.m_m[0][0]) &&
           util::is_almost_equal(m_m[0][1], o.m_m[0][1]) &&
           util::is_almost_equal(m_m[1][0], o.m_m[1][0]) &&
           util::is_almost_equal(m_m[1][1], o.m_m[1][1]);
}

inline bool linear::do_is_equal(const linear &o) const {
    return m_m == o.m_m;
}

inline bool linear::do_is_identity(void) const {
    return m_m[0][0] == 1.f && m_m[0][1] == 0.f &&
           m_m[1][0] == 0.f && m_m[1][1] == 1.f;
}

inline linear linear::do_adjugate(void) const {
    return linear{
        m_m[1][1], -m_m[0][1],
       -m_m[1][0],  m_m[0][0],
    };
}

inline linear linear::do_inverse(void) const {
    rvgf s = 1.f/do_det();
    return linear{
        m_m[1][1]*s, -m_m[0][1]*s,
       -m_m[1][0]*s,  m_m[0][0]*s
    };
}

inline linear linear::do_transpose(void) const {
    return linear{m_m[0][0], m_m[1][0], m_m[0][1], m_m[1][1]};
}

inline rvgf linear::do_det(void) const {
    return m_m[0][0]*m_m[1][1] - m_m[0][1]*m_m[1][0];
}

inline const std::array<rvgf,2> &linear::operator[](int i) const {
    return m_m[i];
}

static inline
RP2 intersection(const R2 &p0, const R2 &d0, const R2 &p1, const R2 &d1) {
    auto d01 = p1-p0;
    auto M = linear{d0, -d1}.adjugate();
    auto u = M.det();
    auto v = M.apply(d01);
    return RP2{p0*u + v[0]*d0, u};
}

static inline
bool intersectq(const R2 &p0, const R2 &q0, const R2 &p1, const R2 &q1) {
    auto d01 = p1-p0;
    auto M = linear{q0-p0, p1-q1}.adjugate();
    auto u = M.det();
    auto su = rvgf(util::sgn(u));
    auto au = std::fabs(u);
    auto v = su*M.apply(d01);
    return 0 < v[0] && v[0] < au && 0 < v[1] && v[1] < au;
}
