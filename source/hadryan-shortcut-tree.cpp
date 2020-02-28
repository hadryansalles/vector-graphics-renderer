#include "hadryan-shortcut-tree.h"

namespace rvg {
    namespace driver {
        namespace png {

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

void node_object::debug(const double& x, const double &y) const {
    printf("\tstart inc: %d\n", m_w_increment);  
    if(true) { 
        int sum = m_w_increment;
        for(auto &seg : m_segments){
            if(seg->intersect(x, y)) {
                printf("\t\tintersect normal-seg (%.2f,%.2f) (%.2f,%.2f)\n", 
                    seg->first()[0], seg->first()[1], seg->last()[0], seg->last()[1]);
                sum += seg->get_dir();
            }
        }
        for(auto &sh : m_shortcuts){
            if(sh->intersect(x, y)) {
                printf("\t\tintersect shortcut-seg (%.2f,%.2f) (%.2f,%.2f)\n", 
                    sh->first()[0], sh->first()[1], sh->last()[0], sh->last()[1]);
                sum += sh->get_dir();
            }
            if(sh->intersect_shortcut(x, y)) {
                printf("\t\tintersect shortcut (%.2f,%.2f) (%.2f,%.2f)\n", 
                    sh->first()[0], sh->first()[1], sh->last()[0], sh->last()[1]);
                sum += sh->get_sh_dir();
            }
        }
        printf("\tfinish path with winding: %d\n", sum);
    }
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

}}}
