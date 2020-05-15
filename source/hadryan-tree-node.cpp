#include "hadryan-tree-node.h"

using namespace rvg;

namespace hadryan {

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

} // hadryan