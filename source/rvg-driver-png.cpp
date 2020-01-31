
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
#include "rvg-input-path-f-close-contours.h"

#include "rvg-i-scene-data.h"

#include "rvg-lua-facade.h"

#include "rvg-driver-png.h"

#define EPS 0.0000000001

namespace rvg {
    namespace driver {
        namespace png {

bool almost_zero(const double& x) {
    return (std::abs(x) < EPS);
}

class path_segment;

class bouding_box {
private:
    R2 m_p0;
    R2 m_p1;
public:
    inline bouding_box(std::vector<R2> &points) {
        assert(points.size() > 1);
        R2 first = points[0];
        R2 last = points[points.size()-1];
        m_p0 = make_R2(std::min(first.get_x(), last.get_x()), std::min(first.get_y(), last.get_y()));
        m_p1 = make_R2(std::max(first.get_x(), last.get_x()), std::max(first.get_y(), last.get_y()));
    }
    inline bouding_box(std::vector<path_segment*> &path);
    inline bool hit_left(const double x, const double y) const {
        return y >= m_p0.get_y() && y < m_p1.get_y() && x <= m_p0.get_x();
    }
    inline bool hit_right(const double x, const double y) const {
        return y >= m_p0.get_y() && y < m_p1.get_y() && x > m_p1.get_x();
    }
    inline bool hit_inside(const double x, const double y) const {
        return y >= m_p0.get_y() && y < m_p1.get_y() && x >= m_p0.get_x() && x < m_p1.get_x();
    }
};

class path_segment {
protected:
    std::vector<R2> m_points; // control points
    const bouding_box m_bbox; // segment bouding box
    int m_dir;
public:
    inline path_segment(std::vector<R2> &points)
        : m_bbox(points) 
        , m_dir(1) { 
        for (auto point : points){
            m_points.push_back(point);
        }
        if(m_points[0].get_y() > m_points[m_points.size()-1].get_y()){
            m_dir = -1;
        }
    } 
    inline virtual ~path_segment() {
        m_points.clear();
    }
    inline R2 bezier(const double t) const {
        return make_R2(in_t(0, t), in_t(1, t));
    }
    virtual double in_t(const int i, const double t) const = 0;
    virtual void print() const = 0;
    inline bool intersect(const double x, const double y) const {
        if(!m_bbox.hit_right(x, y)){
            if(m_bbox.hit_left(x, y)){
                return true;
            }
            else if(m_bbox.hit_inside(x, y)){
                R2 bisection_point;
                double step = 0.5;
                double t = 0.5;
                do {
                    step /= 2.0;
                    bisection_point = bezier(t); 
                    if(bisection_point[1] > y){
                        t -= (double)m_dir*step;
                    }
                    else if(bisection_point[1] < y){
                        t += (double)m_dir*step;
                    }
                } while(step > 0.0001);
                return (x - bisection_point[0] < EPS);
            }
        }
        return false;
    }
    inline int get_dir() const {
        return m_dir;
    }
};

bouding_box::bouding_box(std::vector<path_segment*> &path) {
    assert(path.size() > 0);
    m_p0 = path[0]->bezier(0);
    m_p1 = path[0]->bezier(1);
    for(auto seg : path){
        R2 first = seg->bezier(0);
        R2 last =  seg->bezier(1);
        m_p0 = make_R2(std::min(m_p0[0], first[0]), std::min(m_p0[1], first[1]));
        m_p0 = make_R2(std::min(m_p0[0],  last[0]), std::min(m_p0[1],  last[1]));
        m_p1 = make_R2(std::max(m_p1[0], first[0]), std::max(m_p1[1], first[1]));
        m_p1 = make_R2(std::max(m_p1[0],  last[0]), std::max(m_p1[1],  last[1]));
    }
}

class linear : public path_segment {
public:
    inline linear(std::vector<R2> &points)
        : path_segment(points) {
        assert(points.size() == 2);
    } 
    inline void print() const {
        printf("\tlin: (%.2f,%.2f), (%.2f,%.2f).\n", m_points[0][0], m_points[0][1], m_points[1][0], m_points[1][1]);
    }
    inline double in_t(const int i, const double t) const {
        return (1-t)*m_points[0][i] + t*m_points[1][i];
    }
};

class quadratic : public path_segment {
public:
    inline quadratic(std::vector<R2> &points) 
        : path_segment(points){
        assert(points.size() == 3);
    }
    inline virtual double in_t(const int i, const double t) const {
        return m_points[0][i]*(1-t)*(1-t) + m_points[1][i]*2*(t - t*t) + m_points[2][i]*t*t;
    }
    inline virtual void print() const {
        printf("\tquad: (%.2f,%.2f), (%.2f,%.2f), (%.2f,%.2f).\n", m_points[0][0], m_points[0][1], m_points[1][0], m_points[1][1], m_points[2][0], m_points[2][1]);
    }
};

class rational : public quadratic { 
    quadratic den;
public:
    inline rational(std::vector<R2> &points, std::vector<R2> &den_points)
        : quadratic(points)
        , den(den_points) {
    }
    inline void print() const {
        quadratic::print();
        den.print();
    }
    inline double in_t(const int i, const double t) const {
        return quadratic::in_t(i, t)/den.in_t(i, t);
    }
};

class cubic : public path_segment {
public:
    inline cubic(std::vector<R2> &points)
        : path_segment(points){
        assert(points.size() == 4);
    }
    inline void print() const {
        printf("\tquad: (%.2f,%.2f), (%.2f,%.2f), (%.2f,%.2f).\n", m_points[0][0], m_points[0][1], m_points[1][0], m_points[1][1], m_points[2][0], m_points[2][1]);
    }
    inline double in_t(const int i, const double t) const {
        return m_points[0][i]*(1-t)*(1-t)*(1-t) + 
            m_points[1][i]*3.0*(1-t)*(1-t)*t + 
            m_points[2][i]*3.0*t*t*(1-t) + 
            m_points[3][i]*t*t*t;
    }
};

class scene_object {
private:
    std::vector<path_segment*> m_path;
    bouding_box m_bbox;
    e_winding_rule m_wrule;
    RGBA8 m_color;
public:
    inline scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint) 
        : m_bbox(path)
        , m_wrule(wrule)
        , m_color(paint.get_solid_color()) {
        m_path = path;
    };
    inline ~scene_object() {
        m_path.clear();
    }
    inline void destroy() {
        for(auto seg : m_path) {
            delete seg;
            seg = NULL;
        }
    }
    inline bool hit(const double x, const double y) const {
        if(m_bbox.hit_inside(x, y)) { 
            int sum = 0;
            for(auto seg : m_path){
                if(seg->intersect(x, y)) {
                    sum += seg->get_dir();
                }
            }
            if(m_wrule == e_winding_rule::non_zero){
                return (sum != 0);
            }
            else if(m_wrule == e_winding_rule::odd){
                return ((sum % 2)!= 0);
            }
        }
        return false;
    }
    inline RGBA8 get_color() const {
        return m_color;
    }
    inline void print() const {
        printf("obj:\n");
        for(auto seg : m_path){
            seg->print();
        }
    }
};

class accelerated {
public:
    std::vector<scene_object*> objects;
    inline accelerated()
    {}
    inline ~accelerated() {
        objects.clear();
    }
    inline void destroy() { 
        for(auto obj : objects) {
            obj->destroy();
            delete obj;
            obj = NULL;
        }
    }
    inline void add(scene_object* obj){
        objects.push_back(obj);
    }
    inline void print() const {
        for(auto obj : objects) {
            obj->print();
        }
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
        m_path.push_back(new linear(points));
    };
    inline void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2){
        std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2)};
        if(!are_collinear(points)){
            m_path.push_back(new quadratic(points));
        }
        else{
            do_linear_segment(points[0][0], points[0][1], points[2][0], points[2][1]);
        }
    }
    inline void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2){
        std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2)};
        if(are_collinear(points) && w1 == 1){
            do_linear_segment(points[0][0], points[0][1], points[2][0], points[2][1]);
        }
        else{
            std::vector<R2> den{make_R2(1, 1), make_R2(w1, w1), make_R2(1, 1)};
            m_path.push_back(new rational(points, den));    
        }

    };
    inline void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3){
        std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), make_R2(x3, y3)};
        if(!are_collinear(points)){
            m_path.push_back(new cubic(points));
        }
        else{
            do_linear_segment(points[0][0], points[0][1], points[3][0], points[3][1]);
        }
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
    inline static bool are_collinear(std::vector<R2> &points) {
        int size = points.size();
        for(int i = 0; i < size; i++) {
            R2 p1 = points[i];
            for(int j = i+1; j < size; j++) {
                R2 p2 = points[j];
                for(int k = j+1; k < size; k++) { 
                    R2 p3 = points[k];
                    double r1 = (p2[0] - p1[0])*(p3[1] - p1[1]);
                    double r2 = (p3[0] - p1[0])*(p2[1] - p1[1]);
                    if(!almost_zero(r1-r2)){
                        return false;
                    }
                }
            }
        }
        return true;
    }
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
        path_data->iterate(make_input_path_f_close_contours(
                           make_input_path_f_xform(post*top_xf()*s.get_xf(),
                           make_input_path_f_monotonize(path_builder))));
        acc.add(new scene_object(path_builder.get(), wr, p));
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
    RGBA8 s_color = make_rgba8(255, 255, 255, 255);
    for (auto obj : a.objects){
        if(obj->hit(x, y)){
            RGBA8 color = obj->get_color(); 
            s_color = make_rgba8 (
                (int)color[0] + (1.0-(int)color[3]/255.0)*(int)s_color[0],
                (int)color[1] + (1.0-(int)color[3]/255.0)*(int)s_color[1],
                (int)color[2] + (1.0-(int)color[3]/255.0)*(int)s_color[2],
                (int)color[3] + (1.0-(int)color[3]/255.0)*(int)s_color[3]
            );
        }
    }
    return s_color;
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
