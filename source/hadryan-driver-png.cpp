
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
#include "rvg-color-ramp.h"
#include "rvg-spread.h"
#include "rvg-rgba.h"
#include "rvg-xform.h"
#include "omp.h"

#include "rvg-i-input-path.h"
#include "rvg-i-monotonic-parameters.h"

#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-monotonize.h"
#include "rvg-input-path-f-close-contours.h"
#include "rvg-input-path-f-downgrade-degenerate.h"

#include "blue-noise.h"
#include "hadryan-color.h"
#include "hadryan-bezier.h"
#include "hadryan-shortcut-tree.h"
#include "hadryan-make-not-interger.h"
#include "hadryan-driver-png.h"

#include "rvg-i-scene-data.h"
#include "rvg-lua-facade.h"

namespace rvg {
    namespace driver {
        namespace png {

class accelerated {
public:
    std::vector<scene_object*> objects;
    tree_node* root = nullptr;
    std::vector<R2> samples;
    int threads;
    R2 debug;
    bool debugging;
    inline accelerated()
        : samples{make_R2(0, 0)}
        , threads(1)
        , debug(0, 0)
        , debugging(false)
    {}
    inline void destroy() { 
        for(auto &obj : objects) {
            delete obj;
            obj = NULL;
        }
        root->destroy();
        delete root;
    }
    inline void add(scene_object* obj){
        objects.push_back(obj);
    }
    inline void invert() {
        std::reverse(objects.begin(), objects.end());
    }
};

class monotonic_builder final: public i_input_path<monotonic_builder> {
friend i_input_path<monotonic_builder>;
    std::vector<path_segment*> m_path;
    R2 m_last_move;
public:
    inline monotonic_builder() 
        : m_last_move(make_R2(0, 0))
    {};
    inline ~monotonic_builder()
    {};
    inline void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1){
        std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1)};
        m_path.push_back(new linear(points[0], points[1]));
    };
    inline void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2){
        m_path.push_back(new quadratic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2)));
    }
    inline void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2){
        m_path.push_back(new quadratic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), w1));    
    };
    inline void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3){
        m_path.push_back(new cubic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), make_R2(x3, y3)));
    };
    inline void do_begin_contour(rvgf x0, rvgf y0){
        m_last_move = make_R2(x0, y0);
    };
    inline void do_end_open_contour(rvgf x0, rvgf y0){
        this->do_linear_segment(x0, y0, m_last_move[0], m_last_move[1]);
    };
    inline void do_end_closed_contour(rvgf x0, rvgf y0){
        (void) x0;
        (void) y0;
    };
    std::vector<path_segment*>& get() {
        return m_path;
    }
};

class accelerated_builder final: public i_scene_data<accelerated_builder> {
private:
    friend i_scene_data<accelerated_builder>;
    accelerated &acc;
    std::vector<xform> m_xf_stack;
    inline void pop_xf(){
        if (m_xf_stack.size() > 0) {
            m_xf_stack.pop_back();
        }
    }
    inline void push_xf(const xform &xf){
        m_xf_stack.push_back(top_xf() * xf);
    }
    inline const xform &top_xf() const{
        static xform id;
        if (m_xf_stack.empty()) return id;
        else return m_xf_stack.back();
    }
    inline void do_painted_shape(e_winding_rule wr, const shape &s, const paint &p){
        xform post;
        monotonic_builder path_builder;
        path_data::const_ptr path_data = s.as_path_data_ptr(post);
        const xform s_xf = post*top_xf()*s.get_xf();
        path_data->iterate(make_input_path_f_close_contours(
                           make_input_path_f_xform(s_xf,
                           make_input_path_f_downgrade_degenerate(
                           make_input_path_f_monotonize(
                           make_input_path_not_interger(
                           path_builder))))));
        if(path_builder.get().size() > 0) {
            acc.add(new scene_object(path_builder.get(), wr, p.transformed(top_xf())));
        } 
    }
    inline void do_begin_transform(uint16_t depth, const xform &xf){
        (void) depth;
        push_xf(xf);
    }
    inline void do_end_transform(uint16_t depth, const xform &xf){
        (void) depth;
        (void) xf;
        pop_xf(); 
    }
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
    inline accelerated_builder(accelerated &acc_in, const std::vector<std::string> &args, 
        const xform &screen_xf)
        :   acc(acc_in) {
        unpack_args(args);
        push_xf(screen_xf);
    }
    inline void unpack_args(const std::vector<std::string> &args) {
        acc.samples = blue_1;
        double tx = 0;
        double ty = 0;
        for (auto &arg : args) {
            std::string delimiter = ":";
            std::string command = arg.substr(0, arg.find(delimiter)); 
            std::string value = arg.substr(arg.find(delimiter)+1, arg.length()); 
            if(command == std::string{"-pattern"}) {
                if(value == std::string{"1"}) {
                    acc.samples = blue_1;
                } else if(value == std::string{"8"}) {
                    acc.samples = blue_8;
                } else if(value == std::string{"16"}) {
                    acc.samples = blue_16;
                } else if(value == std::string{"32"}) {
                    acc.samples = blue_32;
                } else if(value == std::string{"64"}) {
                    acc.samples = blue_64;
                }
            } else if(command == std::string{"-tx"}) {
                tx = std::stof(value);
            } else if(command == std::string{"-ty"}) {
                ty = std::stof(value);
            } else if(command == std::string{"-j"}) {
                acc.threads = std::stoi(value);
            } else if(command == std::string{"-depth"}) {
                tree_node::set_max_depth(std::stoi(value));
            } else if(command == std::string{"-min_seg"}) {
                tree_node::set_min_segments(std::stoi(value));
            } else if(command == std::string{"-debug_x"}) {
                acc.debug = make_R2(std::stof(value), acc.debug[1]);
                acc.debugging = true;
            } else if(command == std::string{"-debug_y"}) {
                acc.debug = make_R2(acc.debug[0], std::stof(value));
                acc.debugging = true;
            }
        }
        push_xf(translation(tx, ty));
    }
};

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v, const std::vector<std::string> &args) {
    int xl, yb, xr, yt;
    std::tie(xl, yb) = v.bl();
    std::tie(xr, yt) = v.tr();
    int max_depth_ = std::log2(std::min(xr-xl, yt-yb)/2.0);
    tree_node::set_max_depth(max_depth_); // depth to each cell contain at least 4 sampless
    accelerated acc;
    accelerated_builder builder(acc, args, make_windowviewport(w, v) * c.get_xf());
    c.get_scene_data().iterate(builder);
    acc.invert();
    leave_node* first_leave = new leave_node(make_R2(xl, yb), make_R2(xr, yt));
    for(auto &obj : acc.objects) {
        node_object node_obj(obj);
        // insert in a node if collides with cell
        if(first_leave->intersect(obj->m_bbox)){
            for(auto &seg : obj->m_path) {
                bool hit_br_tr = hit_v_bound(xr, yb, yt, seg);
                bool hit_bl_br = hit_h_bound(yb, xl, xr, seg);
                bool hit_bl_tl = hit_v_bound(xl, yb, yt, seg);
                bool hit_tl_tr = hit_h_bound(yt, xl, xr, seg);
                bool total_inside = totally_inside(xl, xr, yb, yt, seg);
                bool bbox_hit = first_leave->intersect(seg->m_bbox);
                bool inside = (total_inside || hit_br_tr || hit_bl_br || hit_bl_tl || hit_tl_tr);
                if(inside) {
                    node_obj.add_segment(seg, hit_br_tr);
                }
                bool hit_br(seg->intersect(xr, yb));
                if(hit_br) {
                    node_obj.increment(seg->get_dir());
                }
                if(acc.debugging && inside) {
                    printf("seg (%.2f,%.2f) (%.2f,%.2f) bbox:%d hit_br_inf:%d hit_br_tr:%d hit_bl_br:%d hit_bl_tl:%d hit_tl_tr:%d tinside:%d\n",
                        seg->first()[0], seg->first()[1], seg->last()[0], seg->last()[1], bbox_hit, hit_br, hit_br_tr, hit_bl_br, 
                        hit_bl_tl, hit_tl_tr, total_inside 
                    );
                } else if(acc.debugging) {
                    printf("seg REMOVED (%.2f,%.2f) (%.2f,%.2f) bbox:%d hit_br_inf:%d hit_br_tr:%d hit_bl_br:%d hit_bl_tl:%d hit_tl_tr:%d tinside:%d\n",
                        seg->first()[0], seg->first()[1], seg->last()[0], seg->last()[1], bbox_hit, hit_br, hit_br_tr, hit_bl_br,  
                        hit_bl_tl, hit_tl_tr, total_inside 
                    );
                }
            }
            if(node_obj.get_size() || node_obj.get_increment() != 0) {
                first_leave->add_node_object(node_obj);
            }
        }
    }
    acc.root = first_leave->subdivide();
    if(first_leave != acc.root) {
        delete first_leave;
    }
    return acc;
}

void debug(const accelerated& a, float x, float y) {
    if(a.root != nullptr) {
        auto nod = a.root->get_node_of(x, y);
        for(auto &nobj : nod->get_objects()) {
            nobj.debug(x, y);
        }
    }
}

inline RGBA8 sample_cell(const leave_node* nod, const double &x, const double &y) {
    RGBA8 c = make_rgba8(0, 0, 0, 0);
    for(auto &nobj : nod->get_objects()) {
        if(nobj.hit(x, y)) {
            c = over(c, pre_multiply(nobj.get_color(x, y)));
            if((int) c[3] == 255) {
                return c;
            }
        }
    }   
    return over(c, make_rgba8(255, 255, 255, 255)); 
}

inline RGBA8 sample(const accelerated& a, float x, float y){
    if(a.debugging && std::floor(x) == a.debug[0] && std::floor(y) == a.debug[1]) {
        printf("debug (%.2f,%.2f)\n", x, y);
        debug(a, x, y);
        return make_rgba8(0, 255, 0, 255);
    }
    if(a.root != nullptr) {
        auto nod = a.root->get_node_of(x, y);
        if(nod != nullptr) {
            std::vector<int> color{0, 0, 0, 255};
            for(auto &sp : a.samples) {
                double mx = x + sp[0];
                double my = y + sp[1];
                RGBA8 sp_color(remove_gamma(sample_cell(nod, mx, my)));
                color[0] += (int)sp_color[0];
                color[1] += (int)sp_color[1];
                color[2] += (int)sp_color[2];
            }
            color[0] /= a.samples.size();
            color[1] /= a.samples.size();
            color[2] /= a.samples.size();
            return add_gamma(make_rgba8(color[0], color[1], color[2], color[3]));
        }
    }
    return RGBA8(255,255,255,255);
}

void render(accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args) {
    (void) args;
    (void) w;
    int xl, yb, xr, yt;
    std::tie(xl, yb) = v.bl();
    std::tie(xr, yt) = v.tr();
    int width = xr - xl;
    int height = yt - yb;
    image<uint8_t, 4> out_image;
    out_image.resize(width, height);
    #pragma omp parallel for num_threads(a.threads)
    for (int i = 1; i <= height; i++) {
        for (int j = 1; j <= width; j++) {
            double x = xl+j-0.5;
            double y = yb+i-0.5;
            RGBA8 g_color(sample(a, x, y));
            out_image.set_pixel(j-1, i-1, g_color[0], g_color[1], g_color[2], 255);
        }
    }
    store_png<uint8_t>(out, out_image);
    a.destroy();
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
            rvg_lua_check<rvg::viewport>(L, 3),
            rvg_lua_optargs(L, 4)));
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