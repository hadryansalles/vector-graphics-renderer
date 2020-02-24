
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

#include "rvg-winding-rule.h"
#include "rvg-i-input-path.h"
#include "rvg-i-monotonic-parameters.h"

#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-monotonize.h"
#include "rvg-input-path-f-close-contours.h"
#include "rvg-input-path-f-downgrade-degenerate.h"

#include "blue-noise.h"
#include "hadryan-color.cpp"
#include "hadryan-path.cpp"

#include "rvg-i-scene-data.h"
#include "rvg-lua-facade.h"

#include "rvg-driver-png.h"

#define EPS 0.00001

namespace rvg {
    namespace driver {
        namespace png {

class scene_object {
private:
    e_winding_rule m_wrule;
    color_solver* color;
public:
    std::vector<path_segment*> m_path;
    bouding_box m_bbox;
public:
    inline scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint_in) 
        : m_wrule(wrule) {
        m_path = path;
        R2 bb0 = path[0]->first();
        R2 bb1 = path[0]->last();
        for(auto &seg : path){
            R2 f = seg->first();
            R2 l = seg->last();
            bb0 = make_R2(std::min(bb0[0], f[0]), std::min(bb0[1], f[1]));
            bb0 = make_R2(std::min(bb0[0], l[0]), std::min(bb0[1], l[1]));
            bb1 = make_R2(std::max(bb1[0], f[0]), std::max(bb1[1], f[1]));
            bb1 = make_R2(std::max(bb1[0], l[0]), std::max(bb1[1], l[1]));
        }
        m_bbox = bouding_box(bb0, bb1);
        if(paint_in.is_solid_color()) {
            color = new color_solver(paint_in);
        } else if(paint_in.is_linear_gradient()) {
            color = new linear_gradient_solver(paint_in);
        } else if(paint_in.is_radial_gradient()) {
            color = new radial_gradient_solver(paint_in);
        } else if(paint_in.is_texture()) { 
            color = new texture_solver(paint_in);
        } else {
            RGBA8 s_transparent(0, 0, 0, 0);
            unorm8 s_opacity(0);
            paint s_paint(s_transparent, s_opacity);
            color = new color_solver(s_paint);
        }
    }
    scene_object(const scene_object &rhs) = delete;
    scene_object& operator=(const scene_object &rhs) = delete;
    inline ~scene_object() {
        for(auto &seg : m_path) {
            delete seg;
            seg = NULL;
        }
        delete color;
        m_path.clear();
    }
    inline bool satisfy_wrule(int winding) const {
        if(m_wrule == e_winding_rule::non_zero){
            return (winding != 0);
        }
        else if(m_wrule == e_winding_rule::odd){
            return ((winding % 2)!= 0);
        }
        return false;
    }
    inline RGBA8 get_color(const double x, const double y) const {
        return color->solve(x, y);
    }
};

class node_object {
    // only points to segments inside scene_object
    std::vector<const path_segment*> m_segments;
    std::vector<const path_segment*> m_shortcuts;
    int m_w_increment = 0;
public:
    const scene_object* m_ptr; 
    inline node_object(const scene_object* ptr)
        : m_ptr(ptr) {
        for(auto *seg : ptr->m_path) {
            m_segments.push_back(seg);  
        }
    }
    inline void add_segment(const path_segment* segment, bool shortcut = false) {
        if(shortcut) {
            m_shortcuts.push_back(segment);
        } else {
            m_segments.push_back(segment);
        }
    }
    inline bool hit(const double x, const double y) const {
        if(m_ptr->m_bbox.hit_inside(x, y)) { 
            int sum = 0;
            for(auto &seg : m_segments){
                if(seg->intersect(x, y)) {
                    sum += seg->get_dir();
                }
            }
            for(auto &sh : m_shortcuts){
                if(sh->intersect_shortcut(x, y)) {
                    sum += sh->get_dir();
                }
            }
            return m_ptr->satisfy_wrule(sum);
        }
        return false;
    }
    inline RGBA8 get_color(const double x, const double y) const {
        return m_ptr->get_color(x, y);
    }
};

class leave_node;
class tree_node {
protected:
    static int depth;
    static int max_depth;
    const double m_w;
    const double m_h;
    const bouding_box m_bbox;
    const R2 m_pc;
    const R2 m_p0;
    const R2 m_p1;
public:
    tree_node(const R2 &p0, const R2 &p1) 
        : m_w(p1[0]-p0[0])
        , m_h(p1[1]-p0[1])
        , m_bbox(p0, p1)
        , m_pc(p0+(make_R2(m_w, m_h)/2.0)) 
        , m_p0(p0)
        , m_p1(p1) {
        assert(p0[0] <= p1[0] && p0[1] < p1[1]);
    }
    virtual ~tree_node()
    {}
    bool intersect(const scene_object* obj) const{
        return m_bbox.intersect(obj->m_bbox);
    }
    virtual const leave_node* get_node_of(const double &x, const double &y) const = 0;
    virtual void destroy() 
    {}
    static void set_max_depth(int max_) {
        max_depth = max_;
    } 
};
int tree_node::depth = 0;
int tree_node::max_depth = 2;

class intern_node : public tree_node {
    tree_node* m_tr;
    tree_node* m_tl;
    tree_node* m_bl;
    tree_node* m_br;
public:
    intern_node(const R2 &p0, const R2 &p1, tree_node* tr,
                tree_node* tl, tree_node* bl, tree_node* br)
        : tree_node(p0, p1)
        , m_tr(tr)
        , m_tl(tl)
        , m_bl(bl)
        , m_br(br) 
    {}
    virtual void destroy() {
        m_tr->destroy();
        m_tl->destroy();
        m_bl->destroy();
        m_br->destroy();
        delete m_tr;
        delete m_tl;
        delete m_bl;
        delete m_br;
    }
    const leave_node* get_node_of(const double &x, const double &y) const {
        if(!(x >= m_p0[0] && x < m_p1[0]
          && y >= m_p0[1] && y < m_p1[1])) {
            return nullptr;
        }
        if(x >= m_p0[0] && x < m_pc[0]) { // left side
            if(y >= m_p0[1] && y < m_pc[1]) { // bottom side
                return m_bl->get_node_of(x, y); 
            } else { // top side
                return m_tl->get_node_of(x, y);
            }
        } else { // right side
            if(y >= m_p0[1] && y < m_pc[1]) { // bottom side
                return m_br->get_node_of(x, y);
            } else { // top side
                return m_tr->get_node_of(x, y);
            }
        }
    }
};

class leave_node : public tree_node {
    std::vector<node_object> m_objects;
public:
    leave_node(const R2 &p0, const R2 &p1)
        : tree_node(p0, p1)
    {}
    const leave_node* get_node_of(const double &x, const double &y) const {
        (void) x;
        (void) y;
        return this;
    }
    void add_node_object(const node_object &node_obj) {
        m_objects.push_back(node_obj);
    }
    const std::vector<node_object>& get_objects() const {
        return m_objects;
    }
    tree_node* subdivide() {
        if(depth >= max_depth) {
            return this;
        }
        auto tr = new leave_node(m_pc, m_p1);
        auto tl = new leave_node(make_R2(m_p0[0],m_pc[1]), make_R2(m_pc[0],m_p1[1]));
        auto bl = new leave_node(m_p0, m_pc);
        auto br = new leave_node(make_R2(m_pc[0],m_p0[1]), make_R2(m_p1[0],m_pc[1]));
        for(auto &nobj : m_objects) {
            if(tr->intersect(nobj.m_ptr)) {
                tr->add_node_object(nobj);
            }
            if(tl->intersect(nobj.m_ptr)) {
                tl->add_node_object(nobj);
            }
            if(bl->intersect(nobj.m_ptr)) {
                bl->add_node_object(nobj);
            }
            if(br->intersect(nobj.m_ptr)) {
                br->add_node_object(nobj);
            }
        }
        depth++;
        auto ntr = tr->subdivide(); 
        auto ntl = tl->subdivide();
        auto nbl = bl->subdivide(); 
        auto nbr = br->subdivide();
        if(ntr != tr) delete tr;
        if(ntl != tl) delete tl;
        if(nbl != bl) delete bl;
        if(nbr != br) delete br;
        return new intern_node(m_p0, m_p1, ntr, ntl, nbl, nbr);
    }
};

class accelerated {
public:
    std::vector<scene_object*> objects;
    tree_node* root = nullptr;
    std::vector<R2> samples;
    int threads;
    inline accelerated()
        : samples{make_R2(0, 0)}
        , threads(1)
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
                           path_builder)))));
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
            }
        }
        push_xf(translation(tx, ty));
    }
};

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v, const std::vector<std::string> &args) {
    accelerated acc;
    accelerated_builder builder(acc, args, make_windowviewport(w, v) * c.get_xf());
    c.get_scene_data().iterate(builder);
    acc.invert();
    int xl, yb, xr, yt;
    std::tie(xl, yb) = v.bl();
    std::tie(xr, yt) = v.tr();
    leave_node* first_leave = new leave_node(make_R2(xl, yb), make_R2(xr, yt));
    for(auto &obj : acc.objects) {
        // insert in a node if collides with cell
        if(first_leave->intersect(obj)){
            node_object node_obj(obj);
            first_leave->add_node_object(node_obj);
        }
    }
    acc.root = first_leave->subdivide();
    if(first_leave != acc.root) {
        delete first_leave;
    }
    return acc;
}

RGBA8 sample(const accelerated& a, float x, float y){
    RGBA8 c = make_rgba8(0, 0, 0, 0);
    if(a.root != nullptr) {
        auto nod = a.root->get_node_of(x, y);
        for(auto &nobj : nod->get_objects()) {
            if(nobj.hit(x, y)) {
                c = over(c, pre_multiply(nobj.get_color(x, y)));
                if((int) c[3] == 255) {
                    return c;
                }
            }
        }
    }
    return over(c, make_rgba8(255, 255, 255, 255));
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
            std::vector<int> color{0, 0, 0, 255};
            for(auto &sp : a.samples) {
                double y = yb+i-0.5+sp[0];
                double x = xl+j-0.5+sp[1];
                RGBA8 sp_color(remove_gamma(sample(a, x, y)));
                color[0] += (int)sp_color[0];
                color[1] += (int)sp_color[1];
                color[2] += (int)sp_color[2];
            }
            color[0] /= a.samples.size();
            color[1] /= a.samples.size();
            color[2] /= a.samples.size();
            RGBA8 g_color = add_gamma(make_rgba8(color[0], color[1], color[2], color[3]));
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
