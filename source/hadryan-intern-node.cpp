#include "hadryan-intern-node.h"

namespace hadryan {

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

} // hadryan