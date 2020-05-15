#ifndef HADRYAN_LEAVE_NODE_H
#define HADRYAN_LEAVE_NODE_H

#include "hadryan-node-object.h"
#include "hadryan-tree-node.h"

using namespace rvg;

namespace hadryan {

class leave_node : public tree_node {
    std::vector<node_object> m_objects;
    int m_n_segments;
public:
    leave_node(const R2 &p0, const R2 &p1);
    const leave_node* get_node_of(const double &x, const double &y) const;
    void add_node_object(const node_object &node_obj);
    const std::vector<node_object>& get_objects() const;
    tree_node* subdivide(int depth = 0);
};

inline void leave_node::add_node_object(const node_object &node_obj) {
    m_objects.push_back(node_obj);
    m_n_segments += node_obj.get_size();
}

inline const std::vector<node_object>& leave_node::get_objects() const {
    return m_objects;
}

} // hadryan

#endif // HADRYAN_LEAVE_NODE_H