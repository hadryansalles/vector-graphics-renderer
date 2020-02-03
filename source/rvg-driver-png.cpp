
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

#include "rvg-winding-rule.h"
#include "rvg-i-input-path.h"
#include "rvg-i-monotonic-parameters.h"

#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-monotonize.h"
#include "rvg-input-path-f-close-contours.h"
#include "rvg-input-path-f-downgrade-degenerate.h"

#include "rvg-i-scene-data.h"

#include "rvg-lua-facade.h"

#include "rvg-driver-png.h"

#define EPS 0.0000000000001

namespace rvg {
    namespace driver {
        namespace png {


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
    inline virtual bool implicit_hit(const double x, const double y) const {
        (void) x;
        (void) y;
        return false;
    }
    inline bool intersect(const double x, const double y) const {
        if(!m_bbox.hit_right(x, y)){
            if(m_bbox.hit_left(x, y) || (implicit_hit(x, y) && m_bbox.hit_inside(x, y))){
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
                } while(step > 0.000001);
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
    if(path.size() > 0) {
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
}

class linear : public path_segment {
private:
    R2 m_d;
public:
    inline linear(std::vector<R2> &points)
        : path_segment(points)
        , m_d(0, 0) {
        assert(points.size() == 2);
        m_d = points[1] - points[0];
    } 
    inline void print() const {
        printf("\tlin: (%.2f,%.2f), (%.2f,%.2f).\n", m_points[0][0], m_points[0][1], m_points[1][0], m_points[1][1]);
    }
    inline double in_t(const int i, const double t) const {
        return (1-t)*m_points[0][i] + t*m_points[1][i];
    }
    inline bool implicit_hit(const double x, const double y) const {
        return (m_d[1]*((x - m_points[0][0])*m_d[1] - (y - m_points[0][1])*m_d[0]) <= 0);
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

class color_solver {
protected:
    paint m_paint;
public:
    color_solver(const paint& pat)
        : m_paint(pat)
    {}
    virtual ~color_solver()
    {}
    virtual RGBA8 solve(double x, double y) const {
        (void) x;
        (void) y;
        RGBA8 color = m_paint.get_solid_color();
        return make_rgba8(
            color[0], color[1], color[2], color[3]*m_paint.get_opacity()
        );
    }
};

class color_gradient_solver : public color_solver {
protected:
    color_ramp m_ramp;
    const xform m_inv_xf;
    double spread(double t) const {
        double rt = t;
        if(t < 0 && t > 1) {
            switch(m_ramp.get_spread()){
                case e_spread::clamp:
                    rt = std::max(0.0, std::min(1.0, t));
                    break;
                case e_spread::wrap:
                    rt = t - std::floor(t);
                    break;
                case e_spread::mirror:
                    rt =  t - std::floor(t);
                    if((int)rt%2 == 0){
                        rt = 1 - rt;
                    }
                    break;
                case e_spread::transparent:
                    rt = -1;
                    break;
                default:
                    rt = -1;
                    break;
            }
        }
        return rt;
    }
    virtual RGBA8 wrap(double t) const {
        RGBA8 color(0, 0, 0, 0);
        std::vector<color_stop> stops = m_ramp.get_color_stops();
        assert(stops.size() > 0);
        if(t <= stops[0].get_offset()){
            color = stops[0].get_color();
        }
        else if(t >= stops[stops.size()-1].get_offset()){
            color = stops[stops.size()-1].get_color();
        }
        else{
            assert(stops.size() > 1);
            for(int i = 0, j = 1; j < stops.size(); i++, j++){
                if(stops[j].get_offset() >= t){
                    double amp = stops[j].get_offset() - stops[i].get_offset();
                    t -= stops[i].get_offset();
                    t /= amp;
                    RGBA8 c1 = stops[i].get_color();
                    RGBA8 c2 = stops[j].get_color();
                    return make_rgba8(
                        c1[0]*(1-t) + c2[0]*t,
                        c1[1]*(1-t) + c2[1]*t,
                        c1[2]*(1-t) + c2[2]*t,
                        c1[3]*(1-t) + c2[3]*t 
                    );
                    break;
                }
            }
        }
        return color;
    }
    virtual double convert(R2 p) const = 0;
public:
    color_gradient_solver(const paint &pat, const color_ramp &ramp) 
        : color_solver(pat)
        , m_ramp(ramp)
        , m_inv_xf(m_paint.get_xf().inverse()) {
    }
    virtual ~color_gradient_solver()
    {}
    virtual RGBA8 solve(double x, double y) const {
        RGBA8 color(0, 0, 0, 0);
        RP2 pp(m_inv_xf.apply(make_RP2(x, y)));
        R2 p(pp[0]/pp[2], pp[1]/pp[2]);
        double t = spread(convert(p));
        if(t != -1) {
            color = wrap(t);
        }
        return make_rgba8(color[0], color[1], color[2], color[3]*m_paint.get_opacity());
    }
};

class linear_gradient_solver : public color_gradient_solver {
private:
    const linear_gradient_data m_data;
    const R2 m_p1;
    const R2 m_p2_p1;
    double convert(R2 p) const {
        return dot((p-m_p1), (m_p2_p1))/dot((m_p2_p1), (m_p2_p1));
    }
public:
    linear_gradient_solver(const paint& pat)
        : color_gradient_solver(pat, pat.get_linear_gradient_data().get_color_ramp())
        , m_data(m_paint.get_linear_gradient_data())
        , m_p1(m_data.get_x1(), m_data.get_y1())
        , m_p2_p1(m_data.get_x2()-m_data.get_x1(), m_data.get_y2()-m_data.get_y1()) 
    {}
};


class radial_gradient_solver : public color_gradient_solver {
private:
    const radial_gradient_data m_data;
    xform  m_xf;
    R2     m_c;
    R2     m_f;
    double m_r;
    double m_mod_f;
    double m_B;
    double m_C;
    double convert(R2 p_in) const {
        R2 p(m_xf.apply(p_in));
        double A = p[0]*p[0] + p[1]*p[1];
        double B = p[0]*m_B;
        double C = m_C;
        double det = B*B - A*C;
        assert(det >= 0);
        det = std::sqrt(det);
        //assert(std::abs(-B + det) > EPS);
        return A/(-B + det);
    }
       
public:
    radial_gradient_solver(const paint& pat)
        : color_gradient_solver(pat, pat.get_radial_gradient_data().get_color_ramp())
        , m_data(pat.get_radial_gradient_data()) 
        , m_c(m_data.get_cx(), m_data.get_cy()) 
        , m_f(m_data.get_fx(), m_data.get_fy())
        , m_r(m_data.get_r()) {
        m_xf = identity().translated(-m_c[0], -m_c[1]).scaled(1/m_r);
        m_f = R2(m_xf.apply(m_f));
        m_mod_f = std::sqrt(m_f[0]*m_f[0] + m_f[1]*m_f[1]);
        if(m_mod_f > 1.0f - EPS){
            m_mod_f = 1.0f - EPS;
        }
        if(m_mod_f > EPS){
            m_xf = m_xf.transformed(make_affinity(-m_f[0]/m_mod_f, -m_f[1]/m_mod_f, m_mod_f,  -m_f[1]/m_mod_f, m_f[0]/m_mod_f, 0));   
        }
        m_B = -m_mod_f;
        m_C = m_mod_f*m_mod_f - 1;
    }   
};  

class scene_object {
private:
    std::vector<path_segment*> m_path;
    bouding_box m_bbox;
    e_winding_rule m_wrule;
    color_solver* color;
public:
    inline scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint_in) 
        : m_bbox(path)
        , m_wrule(wrule) {
        m_path = path;
        if(paint_in.is_solid_color()) {
            color = new color_solver(paint_in);
        } else if(paint_in.is_linear_gradient()) {
            color = new linear_gradient_solver(paint_in);
        } else if(paint_in.is_radial_gradient()) {
            color = new radial_gradient_solver(paint_in);
        } else {
            RGBA8 s_transparent(0, 0, 0, 0);
            unorm8 s_opacity(0);
            paint s_paint(s_transparent, s_opacity);
            color = new color_solver(s_paint);
        }
    }
    inline ~scene_object() {
        m_path.clear();
    }
    inline void destroy() {
        for(auto seg : m_path) {
            delete seg;
            seg = NULL;
        }
        delete color;
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
    inline RGBA8 get_color(const double x, const double y) const {
        return color->solve(x, y);
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
        m_path.push_back(new quadratic(points));
    }
    inline void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2){
        std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2)};
        std::vector<R2> den{make_R2(1, 1), make_R2(w1, w1), make_R2(1, 1)};
        m_path.push_back(new rational(points, den));    
    };
    inline void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3){
        std::vector<R2> points{make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), make_R2(x3, y3)};
        m_path.push_back(new cubic(points));
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
    accelerated acc;
    std::vector<xform> m_xf_stack;
    
    inline void push_xf(const xform &xf){
        m_xf_stack.push_back(top_xf() * xf);
    }
    inline void pop_xf(){
        if (m_xf_stack.size() > 0) {
            m_xf_stack.pop_back();
        }
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
        p.transformed(top_xf());
        path_data->iterate(make_input_path_f_close_contours(
                           make_input_path_f_xform(s_xf,
                           make_input_path_f_downgrade_degenerate(
                           make_input_path_f_monotonize(
                           path_builder)))));
        acc.add(new scene_object(path_builder.get(), wr, p));
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
    RGBA8 c = make_rgba8(0, 0, 0, 0);
    for(auto obj_it = a.objects.rbegin(); obj_it != a.objects.rend(); ++obj_it) {
        auto obj = (*obj_it);
        if(obj->hit(x, y)){
            c = over(pre_multiply(c), pre_multiply(obj->get_color(x, y)));
        }
    }
    return over(c, make_rgba8(255, 255, 255, 255));
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
