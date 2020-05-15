#include "hadryan-node-object.h"

using namespace rvg;

namespace hadryan {

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

} // hadryan