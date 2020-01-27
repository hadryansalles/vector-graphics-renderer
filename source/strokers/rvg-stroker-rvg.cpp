#include "rvg-stroker-rvg.h"
#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-stroke.h"

namespace rvg {
    namespace stroker {

shape rvg(const shape &input_shape, const xform &screen_xf, float width,
    stroke_style::const_ptr style) {
    auto output_path = make_intrusive<path_data>();
    input_shape.as_path_data_ptr(input_shape.get_xf().
        transformed(screen_xf))->iterate(
            make_input_path_f_xform(input_shape.get_xf(),
                make_input_path_f_stroke(
                    width, style, *output_path)));
    return shape{output_path};
}

} }
