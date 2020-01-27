template <typename M, typename N,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_xform<M>::value &&
        rvg::meta::is_an_i_xform<N>::value &&
        !std::is_same<M, N>::value &&
        std::is_convertible<M,projectivity>::value &&
        std::is_convertible<N,projectivity>::value &&
        (!std::is_convertible<M,affinity>::value ||
         !std::is_convertible<N,affinity>::value)>::type>
projectivity operator*(const M &m, const N &n) {
#ifdef XFORM_DEBUG
    std::cerr << "projectivity operator*(static_cast<projectivity>, static_cast<projectivity>)\n";
#endif
    return static_cast<projectivity>(n).transformed(static_cast<projectivity>(m));
}

template <typename M, typename N,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_xform<M>::value &&
        rvg::meta::is_an_i_xform<N>::value &&
        !std::is_same<M, N>::value &&
        std::is_convertible<M,affinity>::value &&
        std::is_convertible<N,affinity>::value &&
        (!std::is_convertible<M,linear>::value ||
        !std::is_convertible<N,linear>::value)>::type>
affinity operator*(const M &m, const N &n) {
#ifdef XFORM_DEBUG
        std::cerr << "operator*(static_cast<affinity>, static_cast<affinity>)\n";
#endif
    return static_cast<affinity>(n).transformed(static_cast<affinity>(m));
}

template <typename M, typename N,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_xform<M>::value &&
        rvg::meta::is_an_i_xform<N>::value &&
        !std::is_same<M, N>::value &&
        std::is_convertible<M,linear>::value &&
        std::is_convertible<N,linear>::value>::type>
linear operator*(const M &m, const N &n) {
#ifdef XFORM_DEBUG
    std::cerr << "linear operator*(static_cast<linear>, static_cast<linear>)\n";
#endif
    return static_cast<linear>(n).transformed(static_cast<linear>(m));
}

template <typename M,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_xform<M>::value &&
        !std::is_same<M, identity>::value>::type>
M operator*(const M &m, const identity &) {
#ifdef XFORM_DEBUG
        std::cerr << "operator*(const i_xform &, const identity &)\n";
#endif
    return m;
}

template <typename M,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_xform<M>::value &&
        !std::is_same<M, identity>::value>::type>
M operator*(const identity &, const M &m) {
#ifdef XFORM_DEBUG
        std::cerr << "operator*(const i_xform &, const identity &)\n";
#endif
    return m;
}

template <typename M,
    typename = typename std::enable_if<
        rvg::meta::is_an_i_xform<M>::value>::type>
decltype(std::declval<M&>().scaled(rvgf()))
operator*(rvgf s, const M &m)
{
#ifdef XFORM_DEBUG
        std::cerr << "operator*(rvgf s, const i_xform &)\n";
#endif
    return m.scaled(s);
}
