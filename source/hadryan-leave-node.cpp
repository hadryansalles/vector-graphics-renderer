#include "hadryan-leave-node.h"

#include "hadryan-intern-node.h"
#include "hadryan-quad-tree-auxiliar.h"

using namespace rvg;

namespace hadryan {

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

} // hadryan