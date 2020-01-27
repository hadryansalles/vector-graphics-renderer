#ifndef RVG_SCENE_F_NULL_H
#define RVG_SCENE_F_NULL_H

#include "rvg-lua.h"
#include "rvg-i-scene-data.h"

namespace rvg {

class scene_f_null: public i_scene_data<scene_f_null> {

friend i_scene_data<scene_f_null>;

	void do_painted_shape(e_winding_rule, const shape &, const paint &) { }
	void do_tensor_product_patch(const patch<16,4> &) { }
	void do_coons_patch(const patch<12,4> &) { }
	void do_gouraud_triangle(const patch<3,3> &) { }

	void do_stencil_shape(e_winding_rule, const shape &) { }
	void do_begin_clip(uint16_t) { }
	void do_end_clip(uint16_t) { }
	void do_activate_clip(uint16_t) { }

    void do_begin_fade(uint16_t, unorm8) { }
    void do_end_fade(uint16_t, unorm8) { }

    void do_begin_blur(uint16_t, float) { }
    void do_end_blur(uint16_t, float) { }

    void do_begin_transform(uint16_t, const xform &) { }
    void do_end_transform(uint16_t, const xform &) { }
};

static inline auto
make_scene_f_null(void) {
    return scene_f_null{};
}

} // namespace rvg

#endif
