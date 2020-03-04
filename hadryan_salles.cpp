
#include <string>
#include <sstream>
#include <cmath>
#include <memory>
#include <assert.h>
#include <lua.h>

#include "rvg-lua.h"
#include "rvg-pngio.h"
#include "rvg-shape.h"
#include "rvg-color-ramp.h"
#include "rvg-spread.h"
#include "rvg-rgba.h"
#include "rvg-xform.h"

#include "omp.h"

#include "hadryan_salles.h"

#include "rvg-lua-facade.h"

namespace rvg {
    namespace driver {
        namespace png {
// --------------------------------------------------- HADRYAN_BEZIER_CPP

// ----- bouding box

bouding_box::bouding_box()
    : m_p0(0, 0)
    , m_p1(0, 0)
{}
bouding_box::bouding_box(const R2 &first, const R2 &last) {
    m_p0 = make_R2(std::min(first[0], last[0]), std::min(first[1], last[1]));
    m_p1 = make_R2(std::max(first[0], last[0]), std::max(first[1], last[1]));
}

// ----- abstract path segment

path_segment::path_segment(const R2 &p0, const R2 &p1)
    : m_pi(p0)
    , m_pf(p1)
    , m_right((m_pf[0] > m_pi[0]) ? m_pf : m_pi)
    , m_dir((m_pi[1] > m_pf[1]) ? -1 : 1) 
    , m_sh_dir(-1*m_dir)  
    , m_bbox(m_pi, m_pf) { 
    if((m_right == m_pf && m_dir == 1)
        ||(m_right == m_pi && m_dir == -1)) {
        m_sh_dir *= -1;
    }
} 

int path_segment::implicit_value(double x, double y) const {
    if(m_bbox.hit_inside(x, y)) {
        return implicit_hit(x, y) ? 1 : -1;
    } else if((m_right == m_pi && m_dir == -1) || (m_right == m_pf && m_dir == 1)) {
        // 0 em baixo esquerda
        // 0 em cima direita
        if((m_bbox.hit_left(x,y) && m_bbox.hit_down(x,y)) || (m_bbox.hit_right(x,y) && m_bbox.hit_up(x,y))) {
            return 0;
        } else if(m_bbox.hit_up(x,y) || m_bbox.hit_left(x,y)) {
            return 1;
        } else if(m_bbox.hit_down(x,y) || m_bbox.hit_right(x,y)) {
            return -1;
        }
    } else if((m_right == m_pi && m_dir == 1) || (m_right == m_pf && m_dir == -1)) {
        // 0 em cima esquerda
        // 0 em baixo direita
        if((m_bbox.hit_left(x,y) && m_bbox.hit_up(x,y)) || (m_bbox.hit_right(x,y) && (m_bbox.hit_down(x,y)))) {
            return 0;
        } else if(m_bbox.hit_up(x,y) || m_bbox.hit_right(x,y)) {
            return -1;
        } else if(m_bbox.hit_down(x,y) || m_bbox.hit_left(x,y)) {
            return 1;
        }
    }
    return 0;
}

// ----- linear concrete path segment

linear::linear(const R2 &p0, const R2 &p1)
    : path_segment(p0, p1)
    , m_d(p1-p0) 
{}

// ----- quadratic concrete path segment

quadratic::quadratic(const R2 &p0, const R2 &p1, const R2& p2, double w) 
    : path_segment(p0, p2)
    , m_p1(p1-(p0*w))
    , m_p2(p2-p0)
    , m_diag(make_R2(0, 0), m_p2)
    , m_cvx(m_diag.implicit_hit(m_p1[0], m_p1[1]))
    , m_A(4.0*m_p1[0]*m_p1[0]-4.0*w*m_p1[0]*m_p2[0]+m_p2[0]*m_p2[0])
    , m_B(4.0*m_p1[0]*m_p2[0]*m_p1[1]-4.0*m_p1[0]*m_p1[0]*m_p2[1])
    , m_C(-4.0*m_p2[0]*m_p1[1]*m_p1[1]+4.0*m_p1[0]*m_p1[1]*m_p2[1])
    , m_D(-8.0*m_p1[0]*m_p1[1]+4.0*w*m_p2[0]*m_p1[1]+4.0*w*m_p1[0]*m_p2[1]-2.0*m_p2[0]*m_p2[1])
    , m_E(4.0*m_p1[1]*m_p1[1]-4.0*w*m_p1[1]*m_p2[1]+m_p2[1]*m_p2[1]) 
    , m_der((2*m_p2[1]*(-m_p2[0]*m_p1[1]+m_p1[0]*m_p2[1]))) 
{}

// ----- cubic concrete path segment

cubic::cubic(const R2 &p0, const R2 &p1, const R2 &p2, const R2 &p3)
    : path_segment(p0, p3) {
    double x1, x2, x3;
    double y1, y2, y3;
    x1 = (p1 - p0).get_x();
    y1 = (p1 - p0).get_y();
    x2 = (p2 - p0).get_x();
    y2 = (p2 - p0).get_y();
    x3 = (p3 - p0).get_x();
    y3 = (p3 - p0).get_y();
    A = -27.0*x1*x3*x3*y1*y1 + 81.0*x1*x2*x3*y1*y2 - 81.0*x1*x1*x3*y2*y2 - 
            81.0*x1*x2*x2*y1*y3 + 54.0*x1*x1*x3*y1*y3 + 81.0*x1*x1*x2*y2*y3 - 
            27.0*x1*x1*x1*y3*y3;
    B = (-27.0*x1*x1*x1 + 81.0*x1*x1*x2 - 81.0*x1*x2*x2 + 27.0*x2*x2*x2 - 27.0*x1*x1*x3 + 
            54.0*x1*x2*x3 - 27.0*x2*x2*x3 - 9.0*x1*x3*x3 + 9.0*x2*x3*x3 - 
            x3*x3*x3);
    C = 81.0*x1*x2*x2*y1 - 54.0*x1*x1*x3*y1 - 81.0*x1*x2*x3*y1 + 
        54.0*x1*x3*x3*y1 - 9.0*x2*x3*x3*y1 - 81.0*x1*x1*x2*y2 + 
        162.0*x1*x1*x3*y2 - 81.0*x1*x2*x3*y2 + 27.0*x2*x2*x3*y2 - 
        18*x1*x3*x3*y2 + 54.0*x1*x1*x1*y3 - 81.0*x1*x1*x2*y3 + 81.0*x1*x2*x2*y3 - 
        27.0*x2*x2*x2*y3 - 54.0*x1*x1*x3*y3 + 27.0*x1*x2*x3*y3;
    D = 27.0*x3*x3*y1*y1*y1 - 81.0*x2*x3*y1*y1*y2 + 81.0*x1*x3*y1*y2*y2 + 
        81.0*x2*x2*y1*y1*y3 - 54.0*x1*x3*y1*y1*y3 - 81.0*x1*x2*y1*y2*y3 + 
        27.0*x1*x1*y1*y3*y3;
    E = -81.0*x2*x2*y1*y1 + 108.0*x1*x3*y1*y1 + 81.0*x2*x3*y1*y1 - 
        54.0*x3*x3*y1*y1 - 243*x1*x3*y1*y2 + 81.0*x2*x3*y1*y2 + 
        27.0*x3*x3*y1*y2 + 81.0*x1*x1*y2*y2 + 81.0*x1*x3*y2*y2 - 54.0*x2*x3*y2*y2 - 
        108*x1*x1*y1*y3 + 243*x1*x2*y1*y3 - 81.0*x2*x2*y1*y3 - 
        9.0*x2*x3*y1*y3 - 81.0*x1*x1*y2*y3 - 81.0*x1*x2*y2*y3 + 
        54.0*x2*x2*y2*y3 + 9.0*x1*x3*y2*y3 + 54.0*x1*x1*y3*y3 - 27.0*x1*x2*y3*y3;
    F = 81.0*x1*x1*y1 - 162.0*x1*x2*y1 + 81.0*x2*x2*y1 + 54.0*x1*x3*y1 - 
        54.0*x2*x3*y1 + 9.0*x3*x3*y1 - 81.0*x1*x1*y2 + 162.0*x1*x2*y2 - 
        81.0*x2*x2*y2 - 54.0*x1*x3*y2 + 54.0*x2*x3*y2 - 9.0*x3*x3*y2 + 
        27.0*x1*x1*y3 - 54.0*x1*x2*y3 + 27.0*x2*x2*y3 + 18*x1*x3*y3 - 
        18*x2*x3*y3 + 3.0*x3*x3*y3;
    G = -54.0*x3*y1*y1*y1 + 81.0*x2*y1*y1*y2 + 81.0*x3*y1*y1*y2 - 81.0*x1*y1*y2*y2 - 
        81.0*x3*y1*y2*y2 + 27.0*x3*y2*y2*y2 + 54.0*x1*y1*y1*y3 - 162.0*x2*y1*y1*y3 + 
        54.0*x3*y1*y1*y3 + 81.0*x1*y1*y2*y3 + 81.0*x2*y1*y2*y3 - 
        27.0*x3*y1*y2*y3 - 27.0*x2*y2*y2*y3 - 54.0*x1*y1*y3*y3 + 
        18*x2*y1*y3*y3 + 9.0*x1*y2*y3*y3;
    H = -81.0*x1*y1*y1 + 81.0*x2*y1*y1 - 27.0*x3*y1*y1 + 162.0*x1*y1*y2 - 
        162.0*x2*y1*y2 + 54.0*x3*y1*y2 - 81.0*x1*y2*y2 + 81.0*x2*y2*y2 - 
        27.0*x3*y2*y2 - 54.0*x1*y1*y3 + 54.0*x2*y1*y3 - 18.0*x3*y1*y3 + 
        54.0*x1*y2*y3 - 54.0*x2*y2*y3 + 18.0*x3*y2*y3 - 9.0*x1*y3*y3 + 
        9.0*x2*y3*y3 - 3.0*x3*y3*y3;
    I = 27.0*y1*y1*y1 - 81.0*y1*y1*y2 + 81.0*y1*y2*y2 - 27.0*y2*y2*y2 + 27.0*y1*y1*y3 - 
        54.0*y1*y2*y3 + 27.0*y2*y2*y3 + 9.0*y1*y3*y3 - 9.0*y2*y3*y3 + y3*y3*y3;
    m_der = ((y1 - y2 - y3)*(-x3*x3*(4.0*y1*y1 - 2.0*y1*y2 + y2*y2) + 
                x1*x1*(9.0*y2*y2 - 6.0*y2*y3 - 4.0*y3*y3) + 
                x2*x2*(9.0*y1*y1 - 12*y1*y3 - y3*y3) + 
                2*x1*x3*(-y2*(6.0*y2 + y3) + y1*(3.0*y2 + 4.0*y3)) - 
                2*x2*(x3*(3.0*y1*y1 - y2*y3 + y1*(-6.0*y2 + y3)) + 
                x1*(y1*(9.0*y2 - 3.0*y3) - y3*(6.0*y2 + y3))))
    );
    R2 v0(0.0, 0.0);
    R2 v1;
    R2 v2(x3, y3);
    if(std::abs(x1*x1) < BEZIER_EPS && std::abs(y1*y1) < BEZIER_EPS) {
        v1 = make_R2(x2, y2);
    }
    else if(std::abs(x3-x2)*std::abs(x3-x2) < BEZIER_EPS && std::abs(y3-y2)*std::abs(y3-y2) < BEZIER_EPS) {
        
        v1 = make_R2(x1, y1);
    }
    else if(std::abs(x2-x1)*std::abs(x2-x1) < BEZIER_EPS && std::abs(y2-y1)*std::abs(y2-y1) < BEZIER_EPS){
        v1 = make_R2(x2, y2);
    }
    else {
        v1 = make_R2(-x1*(x2*y3 - x3*y2)/(x1*y2 - x1*y3 - x2*y1 + x3*y1), 
                        -y1*(x2*y3 - x3*y2)/(x1*y2 - x1*y3 - x2*y1 + x3*y1));
    }
    m_tri.push_back(linear(v0, v1));
    m_tri.push_back(linear(v1, v2));
    m_tri.push_back(linear(v2, v0));
}

int cubic::triangle_hits(double x, double y) const {
    int sum = 0;
    for(auto &seg : m_tri) {
        if(seg.intersect(x, y)) {
            sum++;
        }
    }
    return sum;
}

// --------------------------------------------------- HADRYAN_COLOR_CPP

// ----- abstract color solver

color_solver::color_solver(const paint& pat)
    : m_paint(pat)
    , m_inv_xf(m_paint.get_xf().inverse())
{}

double color_solver::spread(e_spread spread, double t) const {
    double rt = t;
    if(t < 0 || t > 1) {
        switch(spread){
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
            
RGBA8 color_solver::solve(double x, double y) const {
    (void) x;
    (void) y;
    RGBA8 color = m_paint.get_solid_color();
    return make_rgba8(
        color[0], color[1], color[2], color[3]*m_paint.get_opacity()
    );
}

// ----- abstract gradient solver

color_gradient_solver::color_gradient_solver(const paint &pat, const color_ramp &ramp) 
    : color_solver(pat)
    , m_ramp(ramp)
    , m_stops(m_ramp.get_color_stops())
    , m_stops_size(m_stops.size())
{}

RGBA8 color_gradient_solver::wrap(double t) const {
    RGBA8 color(0, 0, 0, 0);
    if(m_stops_size > 0) {
        if(t <= m_stops[0].get_offset()){
            color = m_stops[0].get_color();
        }
        else if(t >= m_stops[m_stops_size-1].get_offset()){
            color = m_stops[m_stops_size-1].get_color();
        }
        else if(m_stops_size > 1) {
            for(unsigned int i = 0, j = 1; j < m_stops_size; i++, j++){
                if(m_stops[j].get_offset() >= t){
                    double amp = m_stops[j].get_offset() - m_stops[i].get_offset();
                    t -= m_stops[i].get_offset();
                    t /= amp;
                    RGBA8 c1 = m_stops[i].get_color();
                    RGBA8 c2 = m_stops[j].get_color();
                    return make_rgba8(
                        c1[0]*(1-t) + c2[0]*t,
                        c1[1]*(1-t) + c2[1]*t,
                        c1[2]*(1-t) + c2[2]*t,
                        c1[3]*(1-t) + c2[3]*t 
                    );
                }
            }
        }
    }
    return color;
}

RGBA8 color_gradient_solver::solve(double x, double y) const {
    RGBA8 color(0, 0, 0, 0);
    R2 p(m_inv_xf.apply(make_R2(x, y)));
    double t = spread(m_ramp.get_spread(), convert(p));
    if(t != -1) {
        color = wrap(t);
    }
    return make_rgba8(color[0], color[1], color[2], color[3]*m_paint.get_opacity());
}

// ----- linear gradient solver

linear_gradient_solver::linear_gradient_solver(const paint& pat)
    : color_gradient_solver(pat, pat.get_linear_gradient_data().get_color_ramp())
    , m_data(m_paint.get_linear_gradient_data())
    , m_p1(m_data.get_x1(), m_data.get_y1())
    , m_p2_p1(m_data.get_x2()-m_data.get_x1(), m_data.get_y2()-m_data.get_y1()) 
    , m_dot_p2_p1(dot(m_p2_p1, m_p2_p1))
{}

double linear_gradient_solver::convert(R2 p) const {
    return dot((p-m_p1), (m_p2_p1))/m_dot_p2_p1;
}

// ----- radial gradient solver

radial_gradient_solver::radial_gradient_solver(const paint& pat)
    : color_gradient_solver(pat, pat.get_radial_gradient_data().get_color_ramp())
    , m_data(pat.get_radial_gradient_data()) {
    m_xf = identity().translated(-m_data.get_cx(), -m_data.get_cy()).scaled(1/m_data.get_r());
    R2 f = R2(m_xf.apply(make_R2(m_data.get_fx(), m_data.get_fy())));
    double mod_f = std::sqrt(f[0]*f[0] + f[1]*f[1]);
    if(mod_f > 1.0f) { // if focus transformed is in circle boudary
        mod_f = 1.0f - 0.00001;
    }
    if(mod_f > 0.00001) { // if focus transformed isn't on origin
        m_xf = m_xf.rotated(-f[0]/mod_f, f[1]/mod_f).translated(mod_f, 0);
    }
    m_B = -mod_f;
    m_C = mod_f*mod_f - 1;
}   

double radial_gradient_solver::convert(R2 p_in) const {
    R2 p(m_xf.apply(p_in));
    double A = p[0]*p[0] + p[1]*p[1];
    double B = p[0]*m_B;
    double det = B*B - A*m_C;
    assert(det >= 0);
    det = std::sqrt(det);
    assert(std::abs(-B + det) > 0);
    return A/(-B + det);
}

// ----- texture color solver

texture_solver::texture_solver(const paint &pat)
    : color_solver(pat)
    , m_image_ptr(pat.get_texture_data().get_image_ptr())
    , m_spread(pat.get_texture_data().get_spread())
    , m_w(m_image_ptr->get_width())
    , m_h(m_image_ptr->get_height())
{}

RGBA8 texture_solver::solve(double x, double y) const {
    RGBA8 color(0, 0, 0, 0);
    R2 p(m_inv_xf.apply(make_R2(x, y)));
    double s_x = spread(m_spread, p[0]);
    double s_y = spread(m_spread, p[1]);
    if(s_x != -1 && s_y != -1) {
        s_x *= m_w;
        s_y *= m_h;
        int r = 255*m_image_ptr->get_unorm(s_x, s_y, 0);
        int g = 255*m_image_ptr->get_unorm(s_x, s_y, 1);
        int b = 255*m_image_ptr->get_unorm(s_x, s_y, 2);
        color = make_rgba8(r, g, b, m_paint.get_opacity());
    }
    return color;
}

// --------------------------------------------------- HADRYAN_SHORTCUT_TREE_CPP

// ----- scene object

scene_object::scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint_in) 
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

scene_object::~scene_object() {
    for(auto &seg : m_path) {
        delete seg;
        seg = NULL;
    }
    delete color;
    m_path.clear();
}

// ----- node object

node_object::node_object(const scene_object* ptr)
    : m_ptr(ptr) {
}

bool node_object::hit(const double x, const double y) const {
    bool in_path = m_ptr->m_bbox.hit_inside(x, y);
    if(in_path) { 
        int sum = m_w_increment;
        for(auto &seg : m_segments){
            if(seg->intersect(x, y)) {
                sum += seg->get_dir();
            }
        }
        for(auto &sh : m_shortcuts){
            if(sh->intersect(x, y)){
                sum += sh->get_dir();
            } 
            if(sh->intersect_shortcut(x, y)) {
                sum += sh->get_sh_dir();
            }
        }
        return m_ptr->satisfy_wrule(sum);
    }
    return false;
}

// ----- abstract tree node

int tree_node::max_depth = 2;
int tree_node::min_segments = 1;

tree_node::tree_node(const R2 &p0, const R2 &p1) 
    : m_w((int)p1[0]-(int)p0[0])
    , m_h((int)p1[1]-(int)p0[1])
    , m_bbox(make_R2((int)p0[0], (int)p0[1]), make_R2((int)p1[0], (int)p1[1]))
    , m_pc(make_R2((int)p0[0], (int)p0[1])+(make_R2((int)(m_w/2), (int)(m_h/2)))) 
    , m_p0(make_R2((int)p0[0], (int)p0[1]))
    , m_p1(make_R2((int)p1[0], (int)p1[1])) {
}

void tree_node::set_max_depth(int max_) {
    max_depth = max_;
}

void tree_node::set_min_segments(int min_) {
    min_segments = min_;
} 

// ----- concrete intern node

intern_node::intern_node(const R2 &p0, const R2 &p1, tree_node* tr,
            tree_node* tl, tree_node* bl, tree_node* br)
    : tree_node(p0, p1)
    , m_tr(tr)
    , m_tl(tl)
    , m_bl(bl)
    , m_br(br) 
{}

void intern_node::destroy() {
    m_tr->destroy();
    m_tl->destroy();
    m_bl->destroy();
    m_br->destroy();
    delete m_tr;
    delete m_tl;
    delete m_bl;
    delete m_br;
}

const leave_node* intern_node::get_node_of(const double &x, const double &y) const {
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

// ----- concrete leave node 

leave_node::leave_node(const R2 &p0, const R2 &p1)
    : tree_node(p0, p1)
    , m_n_segments(0)
{}

const leave_node* leave_node::get_node_of(const double &x, const double &y) const {
    (void) x;
    (void) y;
    return this;
}

tree_node* leave_node::subdivide(int depth) {
    if(depth >= max_depth || m_n_segments < min_segments) {
        return this;
    }
    auto tr = new leave_node(m_pc, m_p1);
    auto tl = new leave_node(make_R2(m_p0[0],m_pc[1]), make_R2(m_pc[0],m_p1[1]));
    auto bl = new leave_node(m_p0, m_pc);
    auto br = new leave_node(make_R2(m_pc[0],m_p0[1]), make_R2(m_p1[0],m_pc[1]));
    for(auto &nobj : m_objects) {
        node_object tr_obj(nobj.m_ptr);
        node_object tl_obj(nobj.m_ptr);
        node_object bl_obj(nobj.m_ptr);
        node_object br_obj(nobj.m_ptr);
        tr_obj.m_w_increment = nobj.m_w_increment;
        tl_obj.m_w_increment = nobj.m_w_increment;
        bl_obj.m_w_increment = nobj.m_w_increment;
        br_obj.m_w_increment = nobj.m_w_increment;
        auto all_seg(nobj.get_all_segments());
        for(auto &seg : all_seg) {
            bool hit_tr_righ = hit_v_bound(m_p1[0], m_pc[1], m_p1[1], seg);
            bool hit_br_righ = hit_v_bound(m_p1[0], m_p0[1], m_pc[1], seg);
            bool hit_br_down = hit_h_bound(m_p0[1], m_pc[0], m_p1[0], seg);
            bool hit_bl_down = hit_h_bound(m_p0[1], m_p0[0], m_pc[0], seg);
            bool hit_bl_left = hit_v_bound(m_p0[0], m_p0[1], m_pc[1], seg);
            bool hit_tl_left = hit_v_bound(m_p0[0], m_pc[1], m_p1[1], seg);
            bool hit_tl_up   = hit_h_bound(m_p1[1], m_p0[0], m_pc[0], seg);
            bool hit_tr_up   = hit_h_bound(m_p1[1], m_pc[0], m_p1[0], seg);
            bool hit_tl_tr   = hit_v_bound(m_pc[0], m_pc[1], m_p1[1], seg);
            bool hit_bl_tl   = hit_h_bound(m_pc[1], m_p0[0], m_pc[0], seg);
            bool hit_bl_br   = hit_v_bound(m_pc[0], m_p0[1], m_pc[1], seg);
            bool hit_br_tr   = hit_h_bound(m_pc[1], m_pc[0], m_p1[0], seg);
            bool hit_c_inf   = seg->intersect(m_pc[0], m_pc[1]);
            bool hit_cr_inf  = seg->intersect(m_p1[0], m_pc[1]);
            bool hit_dc_inf  = seg->intersect(m_pc[0], m_p0[1]);
            bool hit_br_inf  = seg->intersect(m_p1[0], m_p0[1]);
            if(totally_inside(tr->m_p0, tr->m_p1, seg) || hit_tr_righ || hit_tr_up || hit_tl_tr || hit_br_tr) {
                tr_obj.add_segment(seg, hit_tr_righ);
            }
            if(totally_inside(tl->m_p0, tl->m_p1, seg) || hit_tl_left || hit_tl_tr || hit_tl_up || hit_bl_tl) {
                tl_obj.add_segment(seg, hit_tl_tr);
            }
            if(totally_inside(bl->m_p0, bl->m_p1, seg) || hit_bl_br || hit_bl_down || hit_bl_left || hit_bl_tl) {
                bl_obj.add_segment(seg, hit_bl_br);
            }
            if(totally_inside(br->m_p0, br->m_p1, seg) || hit_br_down || hit_br_righ || hit_br_tr || hit_bl_br) {
                br_obj.add_segment(seg, hit_br_righ);
            } 
            if(hit_c_inf) {
                tl_obj.increment(seg->get_dir());
            }
            if(hit_cr_inf) {
                tr_obj.increment(seg->get_dir());
            }
            if(hit_dc_inf) {
                bl_obj.increment(seg->get_dir());
            }
            if(hit_br_inf) {
                br_obj.increment(seg->get_dir());
            }
        }
        for(auto &shortcut : nobj.get_shortcuts()) {
            bool hit_c_inf   = shortcut->intersect_shortcut(m_pc[0], m_pc[1]);
            bool hit_cr_inf  = shortcut->intersect_shortcut(m_p1[0], m_pc[1]);
            bool hit_dc_inf  = shortcut->intersect_shortcut(m_pc[0], m_p0[1]);
            bool hit_br_inf  = shortcut->intersect_shortcut(m_p1[0], m_p0[1]);
            if(hit_c_inf) {
                tl_obj.increment(shortcut->get_sh_dir());
            }
            if(hit_cr_inf) {
                tr_obj.increment(shortcut->get_sh_dir());
            }
            if(hit_dc_inf) {
                bl_obj.increment(shortcut->get_sh_dir());
            }
            if(hit_br_inf) {
                br_obj.increment(shortcut->get_sh_dir());
            }
        }
        if(tr_obj.get_size() || tr_obj.get_increment() != 0) {
            tr->add_node_object(tr_obj);
        }
        if(tl_obj.get_size() || tl_obj.get_increment() != 0) {
            tl->add_node_object(tl_obj);
        }
        if(bl_obj.get_size() || bl_obj.get_increment() != 0) {
            bl->add_node_object(bl_obj);
        }
        if(br_obj.get_size() || br_obj.get_increment() != 0) {
            br->add_node_object(br_obj);
        }
    }
    depth++;
    tree_node* ntr = nullptr; 
    tree_node* ntl = nullptr;
    tree_node* nbl = nullptr; 
    tree_node* nbr = nullptr;
    #pragma omp task shared(ntr)
    {
        ntr = tr->subdivide(depth);
    }
    #pragma omp task shared(ntl)
    {
        ntl = tl->subdivide(depth);
    }
    #pragma omp task shared(nbl)
    {
        nbl = bl->subdivide(depth);
    }
    #pragma omp task shared(nbr)
    {
        nbr = br->subdivide(depth);
    }
    #pragma omp taskwait 
    {
        if(ntr != tr) delete tr;
        if(ntl != tl) delete tl;
        if(nbl != bl) delete bl;
        if(nbr != br) delete br;
        return new intern_node(m_p0, m_p1, ntr, ntl, nbl, nbr);
    }
}

// --------------------------------------------------- HADRYAN_DRIVER_PNG_CPP

void accelerated::destroy() { 
    for(auto &obj : objects) {
        delete obj;
        obj = NULL;
    }
    root->destroy();
    delete root;
}

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
    return acc;
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

// --------------------------------------------------- HADRYAN_BUILDER_CPP

void accelerated_builder::unpack_args(const std::vector<std::string> &args) {
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
        }
    }
    push_xf(translation(tx, ty));
}

void accelerated_builder::do_painted_shape(e_winding_rule wr, const shape &s, const paint &p){
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
int luaopen_driver_hadryan_salles(lua_State *L) {
    rvg_lua_init(L);
    if (!rvg_lua_typeexists<rvg::driver::png::accelerated>(L, -1)) {
        rvg_lua_createtype<rvg::driver::png::accelerated>(L,
            "png accelerated", -1);
    }
    rvg_lua_facade_new_driver(L, modpngpng);
    return 1;
}
