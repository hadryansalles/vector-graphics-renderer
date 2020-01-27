inline xform make_identity(void) {
    return static_cast<projectivity>(identity{});
}

inline xform make_rotation(rvgf deg) {
    return static_cast<projectivity>(rotation{deg});
}

inline xform make_rotation(rvgf deg, rvgf cx, rvgf cy) {
    return static_cast<projectivity>(
        translation{-cx, -cy}.rotated(deg).translated(cx, cy));
}

inline xform make_translation(rvgf tx, rvgf ty) {
    return static_cast<projectivity>(translation{tx, ty});
}

inline xform make_scaling(rvgf sx, rvgf sy, rvgf cx, rvgf cy) {
    return static_cast<projectivity>(
        translation{-cx, -cy}.scaled(sx, sy).translated(cx, cy));
}

inline xform make_scaling(rvgf s, rvgf cx, rvgf cy) {
    return static_cast<projectivity>(
        translation{-cx, -cy}.scaled(s).translated(cx, cy));
}

inline xform make_scaling(rvgf sx, rvgf sy) {
    return static_cast<projectivity>(scaling{sx, sy});
}

inline xform make_scaling(rvgf s) {
    return static_cast<projectivity>(scaling{s, s});
}

inline xform make_linear(rvgf a, rvgf b, rvgf c, rvgf d) {
    return static_cast<projectivity>(linear{a, b, c, d});
}

inline xform make_affinity(rvgf a, rvgf b, rvgf tx, rvgf c, rvgf d, rvgf ty) {
    return static_cast<projectivity>(affinity{a, b, tx, c, d, ty});
}

inline xform make_projectivity(rvgf a, rvgf b, rvgf c,
    rvgf d, rvgf e, rvgf f, rvgf g, rvgf h, rvgf i) {
    return projectivity{a, b, c, d, e, f, g, h, i};
}
