#ifndef HADRYAN_INTERN_NODE_H
#define HADRYAN_INTERN_NODE_H

#include "hadryan-tree-node.h"

using namespace rvg;

namespace hadryan {

class intern_node : public tree_node {
    tree_node* m_tr;
    tree_node* m_tl;
    tree_node* m_bl;
    tree_node* m_br;
public:
    intern_node(const R2 &p0, const R2 &p1, tree_node* tr,
            tree_node* tl, tree_node* bl, tree_node* br);
    void destroy();
    const leave_node* get_node_of(const double &x, const double &y) const;
};

} // hadryan

#endif // HADRYAN_INTERN_NODE_H