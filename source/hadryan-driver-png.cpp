#include "hadryan-driver-png.h"

#include "rvg-image.h"
#include "rvg-pngio.h"
#include "rvg-lua.h"
#include "rvg-lua-facade.h"

#include "hadryan-accelerated-builder.h"
#include "hadryan-tree-node.h"
#include "hadryan-leave-node.h"
#include "hadryan-quad-tree-auxiliar.h"

using namespace rvg;

namespace hadryan {

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
        if(first_leave->intersect(obj->get_bbox())){
            for(auto &seg : obj->get_path()) {
                bool hit_br_tr = hit_v_bound(xr, yb, yt, seg);
                bool hit_bl_br = hit_h_bound(yb, xl, xr, seg);
                bool hit_bl_tl = hit_v_bound(xl, yb, yt, seg);
                bool hit_tl_tr = hit_h_bound(yt, xl, xr, seg);
                bool total_inside = totally_inside(xl, xr, yb, yt, seg);
                bool inside = (total_inside || hit_br_tr || hit_bl_br || hit_bl_tl || hit_tl_tr);
                if(inside) {
                    node_obj.add_segment(seg, hit_br_tr);
                }
                bool hit_br(seg->intersect(xr, yb));
                if(hit_br) {
                    node_obj.increment(seg->get_dir());
                }
            }
            if(node_obj.get_size() || node_obj.get_increment() != 0) {
                first_leave->add_node_object(node_obj);
            }
        }
    }
    #pragma omp parallel num_threads(acc.threads)
    {
        #pragma omp single
        {
            acc.root = first_leave->subdivide();
        }
    }
    if(first_leave != acc.root) {
        delete first_leave;
    }
    return std::move(acc);
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

} // hadryan

// Lua version of the accelerate function.
// Since there is no acceleration, we simply
// and return the input scene unmodified.
static int luaaccelerate(lua_State *L) {
    rvg_lua_push<hadryan::accelerated>(L,
        hadryan::accelerate(
            rvg_lua_check<rvg::scene>(L, 1),
            rvg_lua_check<rvg::window>(L, 2),
            rvg_lua_check<rvg::viewport>(L, 3),
            rvg_lua_optargs(L, 4)));
    return 1;
}

// Lua version of render function
static int luarender(lua_State *L) {
    auto a = rvg_lua_check<hadryan::accelerated>(L, 1);
    auto w = rvg_lua_check<rvg::window>(L, 2);
    auto v = rvg_lua_check<rvg::viewport>(L, 3);
    auto o = rvg_lua_optargs(L, 5);
    hadryan::render(a, w, v, rvg_lua_check_file(L, 4), o);
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
int luaopen_driver_hadryan_salles(lua_State *L) {
    rvg_lua_init(L);
    if (!rvg_lua_typeexists<hadryan::accelerated>(L, -1)) {
        rvg_lua_createtype<hadryan::accelerated>(L,
            "png accelerated", -1);
    }
    rvg_lua_facade_new_driver(L, modpngpng);
    return 1;
}
