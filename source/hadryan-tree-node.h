#ifndef HADRYAN_TREE_NODE_H
#define HADRYAN_TREE_NODE_H

#include "hadryan-bouding-box.h"

using namespace rvg;

namespace hadryan {

class leave_node;

class tree_node {
protected:
    static int max_depth;
    static int min_segments;
    const double m_w;
    const double m_h;
    const bouding_box m_bbox;
    const R2 m_pc;
    const R2 m_p0;
    const R2 m_p1;
public:
    tree_node(const R2 &p0, const R2 &p1);
    virtual ~tree_node() = default;
    bool intersect(const bouding_box& bbox) const;
    virtual void destroy() {}
    bool is_in_cell(const double &x, const double &y) const;
    static void set_max_depth(int max_);
    static void set_min_segments(int min_);  
    virtual const leave_node* get_node_of(const double &x, const double &y) const = 0;
};

inline bool tree_node::intersect(const bouding_box& bbox) const {
    return m_bbox.intersect(bbox);
}

inline bool tree_node::is_in_cell(const double &x, const double &y) const {
    return x >= (double) m_p0[0] && x < (double) m_p1[0] && y >= (double) m_p0[1] && y < (double) m_p1[1];
}

} // hadryan

#endif // HADRYAN_TREE_NODE_H