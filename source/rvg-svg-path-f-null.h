#ifndef RVG_SVG_PATH_F_NULL_H
#define RVG_SVG_PATH_F_NULL_H

#include <iostream>

#include "rvg-i-svg-path.h"

namespace rvg {

class svg_path_f_null final: public i_svg_path<svg_path_f_null> {

friend i_svg_path<svg_path_f_null>;

    void do_move_to_abs(rvgf, rvgf) { }
    void do_move_to_rel(rvgf, rvgf) { }

    void do_close_path(void) { }

    void do_line_to_abs(rvgf, rvgf) { }
    void do_line_to_rel(rvgf, rvgf) { }

    void do_hline_to_abs(rvgf) { }
    void do_hline_to_rel(rvgf) { }

    void do_vline_to_abs(rvgf) { }
    void do_vline_to_rel(rvgf) { }

    void do_quad_to_abs(rvgf, rvgf, rvgf, rvgf) { }
    void do_quad_to_rel(rvgf, rvgf, rvgf, rvgf) { }

    void do_squad_to_abs(rvgf, rvgf) { }
    void do_squad_to_rel(rvgf, rvgf) { }

    void do_rquad_to_abs(rvgf, rvgf, rvgf, rvgf, rvgf) { }
    void do_rquad_to_rel(rvgf, rvgf, rvgf, rvgf, rvgf) { }

    void do_svg_arc_to_abs(rvgf, rvgf, rvgf, rvgf, rvgf, rvgf, rvgf) { }
    void do_svg_arc_to_rel(rvgf, rvgf, rvgf, rvgf, rvgf, rvgf, rvgf) { }

    void do_cubic_to_abs(rvgf, rvgf, rvgf, rvgf, rvgf, rvgf) { }
    void do_cubic_to_rel(rvgf, rvgf, rvgf, rvgf, rvgf, rvgf) { }

    void do_scubic_to_abs(rvgf, rvgf, rvgf, rvgf) { }
    void do_scubic_to_rel(rvgf, rvgf, rvgf, rvgf) { }
};

inline svg_path_f_null& make_svg_path_f_null(void) {
    static svg_path_f_null n;
    return n;
}

} // namespace rvg

#endif
