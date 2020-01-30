
#include <string>
#include <sstream>
#include <cmath>
#include <memory>
#include <assert.h>

#include <lua.h>
#include "rvg-lua.h"
#include "rvg-pngio.h"
#include "rvg-image.h"
#include "rvg-shape.h"
#include "rvg-paint.h"
#include "rvg-rgba.h"
#include "rvg-winding-rule.h"
#include "rvg-xform-affinity.h"

#include "rvg-i-input-path.h"
#include "rvg-i-monotonic-parameters.h"
#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-monotonize.h"

#include "rvg-i-scene-data.h"

#include "rvg-lua-facade.h"

#include "rvg-driver-png.h"

#define EPS 0.000000001

namespace rvg {
    namespace driver {
        namespace png {

class scene_object {
private:
public:
    inline scene_object() {
        std::cout << "obj.\n";
    };
    inline ~scene_object()
    {};
    inline bool hit(const double x, const double y) const {
        return false;
    };
};

class accelerated {
public:
    std::vector<scene_object*> objects;
    inline accelerated()
    {};
    inline ~accelerated()
    {};
    inline void add(const paint &paint, e_winding_rule w_rule){
        scene_object* obj = new scene_object();
        objects.push_back(obj);
    };
    inline void destroy_objects(){
        scene_object* obj;
        for(unsigned int i = 0; i < objects.size(); i++){
            obj = objects[i];
            objects[i] = NULL;
            delete obj;    
        }
    };
};

class monotonic_builder final: public i_input_path<monotonic_builder> {
friend i_input_path<monotonic_builder>;
public:
    inline monotonic_builder() 
    {};
    inline ~monotonic_builder()
    {};
    inline void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1){
        printf("lin : (%.2f,%.2f), (%.2f,%.2f).\n", x0, y0, x1, y1);
    };
    inline void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2){
        printf("quad : (%.2f,%.2f), (%.2f,%.2f), (%.2f,%.2f).\n", x0, y0, x1, y1, x2, y2);
    }
    inline void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2){
        printf("rat quad : (%.2f,%.2f), (%.2f,%.2f), %.2f, (%.2f,%.2f).\n", x0, y0, x1, y1, w1, x2, y2);
    };
    inline void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3){
        printf("cubic : (%.2f,%.2f), (%.2f,%.2f), (%.2f,%.2f), (%.2f,%.2f).\n", x0, y0, x1, y1, x2, y2, x3, y3);
    };
    inline void do_begin_contour(rvgf x0, rvgf y0){
        printf("begin : (%.2f,%.2f).\n", x0, y0);
    };
    inline void do_end_open_contour(rvgf x0, rvgf y0){
        printf("end open : (%.2f,%.2f).\n", x0, y0);
    };
    inline void do_end_closed_contour(rvgf x0, rvgf y0){
        printf("end closed : (%.2f,%.2f).\n", x0, y0);
    };
};

class accelerated_builder final: public i_scene_data<accelerated_builder> {
private:
    friend i_scene_data<accelerated_builder>;
    accelerated acc;
    std::vector<xform> m_xf_stack;
    
    inline void push_xf(const xform &xf){
        m_xf_stack.push_back(top_xf() * xf);
    };
    inline void pop_xf(){
        if (m_xf_stack.size() > 0) {
            m_xf_stack.pop_back();
        }
    };
    inline const xform &top_xf() const{
        static xform id;
        if (m_xf_stack.empty()) return id;
        else return m_xf_stack.back();
    };
    inline void do_painted_shape(e_winding_rule wr, const shape &s, const paint &p){
        xform post;
        monotonic_builder path_builder;
        path_data::const_ptr path_data = s.as_path_data_ptr(post);
        // monotonize
        path_data->iterate(make_input_path_f_xform(post*top_xf()*s.get_xf(), 
                           make_input_path_f_monotonize(path_builder)));
        acc.add(p, wr);
    };

    inline void do_begin_transform(uint16_t depth, const xform &xf){
        (void) depth;
        push_xf(xf);
    };

    inline void do_end_transform(uint16_t depth, const xform &xf){
        (void) depth;
        (void) xf;
        pop_xf(); 
    };

    inline void do_tensor_product_patch(const patch<16,4> &tpp){(void) tpp;};
    inline void do_coons_patch(const patch<12,4> &cp){(void) cp;};
    inline void do_gouraud_triangle(const patch<3,3> &gt){(void) gt;};
    inline void do_stencil_shape(e_winding_rule wr, const shape &s){(void) wr;(void) s;};
    inline void do_begin_clip(uint16_t depth){(void) depth;};
    inline void do_activate_clip(uint16_t depth){(void) depth;};
    inline void do_end_clip(uint16_t depth){(void) depth;};
    inline void do_begin_fade(uint16_t depth, unorm8 opacity){(void) depth;(void) opacity;};
    inline void do_end_fade(uint16_t depth, unorm8 opacity){(void) depth;(void) opacity;};
    inline void do_begin_blur(uint16_t depth, float radius){(void) depth;(void) radius;};
    inline void do_end_blur(uint16_t depth, float radius){(void) depth;(void) radius;};

public:
    inline accelerated_builder(const xform &screen_xf){
        push_xf(screen_xf);
    };
    inline accelerated get_acc() const{
        return acc;
    };
};

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v) {
    accelerated_builder builder(make_windowviewport(w, v) * c.get_xf());
    c.get_scene_data().iterate(builder);
    return builder.get_acc();
}

RGBA8 sample(const accelerated& a, float x, float y){
    for (auto obj = a.objects.rbegin(); obj != a.objects.rend(); ++obj){
        
    }
    return RGBA8(255, 255, 255, 255);
}

void render(accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args) {
    (void) w;
    (void) args;
    int xl, yb, xr, yt;
    std::tie(xl, yb) = v.bl();
    std::tie(xr, yt) = v.tr();
    int width = xr - xl;
    int height = yt - yb;

    image<uint8_t, 4> out_image;
    out_image.resize(width, height);

    RGBA8 s_color;
    for (int i = 1; i <= height; i++) {
        float y = yb+i-1.+.5f;
        for (int j = 1; j <= width; j++) {
            printf("\r%.2f%%", 100*(double)(i*width+j)/(height*width));
            float x = xl+j-1+.5f;
            s_color  = sample(a, x, y);
            out_image.set_pixel(j-1, i-1, s_color[0], s_color[1], s_color[2], s_color[3]);
        }
    }
    std::cout <<("\n");
    a.destroy_objects();
    store_png<uint8_t>(out, out_image);
}

} } } // namespace rvg::driver::png

// Lua version of the accelerate function.
// Since there is no acceleration, we simply
// and return the input scene unmodified.
static int luaaccelerate(lua_State *L) {
    rvg_lua_push<rvg::driver::png::accelerated>(L,
        rvg::driver::png::accelerate(
            rvg_lua_check<rvg::scene>(L, 1),
            rvg_lua_check<rvg::window>(L, 2),
            rvg_lua_check<rvg::viewport>(L, 3)));
    return 1;
}

// Lua version of render function
static int luarender(lua_State *L) {
    auto a = rvg_lua_check<rvg::driver::png::accelerated>(L, 1);
    auto w = rvg_lua_check<rvg::window>(L, 2);
    auto v = rvg_lua_check<rvg::viewport>(L, 3);
    auto o = rvg_lua_optargs(L, 5);
    rvg::driver::png::render(a, w, v, rvg_lua_check_file(L, 4), o);
    return 0;
}

// List of Lua functions exported into driver table
static const luaL_Reg modpngpng[] = {
    {"render", luarender },
    {"accelerate", luaaccelerate },
    {NULL, NULL}
};

// Lua function invoked to be invoked by require"driver.png"
extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_driver_png(lua_State *L) {
    rvg_lua_init(L);
    if (!rvg_lua_typeexists<rvg::driver::png::accelerated>(L, -1)) {
        rvg_lua_createtype<rvg::driver::png::accelerated>(L,
            "png accelerated", -1);
    }
    rvg_lua_facade_new_driver(L, modpngpng);
    return 1;
}
