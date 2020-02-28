#pragma once

#include "rvg-winding-rule.h"
#include "hadryan-bezier.h"
#include "hadryan-color.h"

namespace rvg {
    namespace driver {
        namespace png {

bool totally_inside(int xmin, int xmax, int ymin, int ymax, const path_segment* seg);
bool totally_inside(R2 bl, R2 tr, const path_segment* seg);
bool hit_v_bound(int cx, int ymin, int ymax, const path_segment* seg);
bool hit_h_bound(int cy, int xmin, int xmax, const path_segment* seg);

class scene_object {
private:
    e_winding_rule m_wrule;
    color_solver* color;
public:
    std::vector<path_segment*> m_path;
    bouding_box m_bbox;
public:
    scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint_in);
    scene_object(const scene_object &rhs) = delete;
    ~scene_object();
    scene_object& operator=(const scene_object &rhs) = delete;
    RGBA8 get_color(const double x, const double y) const;
    bool satisfy_wrule(int winding) const;
};

class node_object {
    // only points to segments inside scene_object
    std::vector<const path_segment*> m_segments;
    std::vector<const path_segment*> m_shortcuts;
public:
    int m_w_increment = 0;
    const scene_object* m_ptr; 
public:
    node_object(const scene_object* ptr);
    void add_segment(const path_segment* segment, bool shortcut = false);
    bool hit(const double x, const double y) const;
    const std::vector<const path_segment*> get_all_segments() const;
    const std::vector<const path_segment*> get_shortcuts() const;
    RGBA8 get_color(const double x, const double y) const;
    int get_increment() const;
    int get_size() const;
    void increment(int inc);    
    
    void debug(const double& x, const double &y) const;
};

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

inline bool totally_inside(int xmin, int xmax, int ymin, int ymax, const path_segment* seg) {
    R2 left(seg->left());
    R2 right(seg->right());
    return (left[0] > xmin && left[0] < xmax && left[1] > ymin && left[1] < ymax) 
        || (right[0] > xmin && right[0] < xmax && right[1] > ymin && right[1] < ymax);
}

inline bool totally_inside(R2 bl, R2 tr, const path_segment* seg) {
    return totally_inside(bl[0], tr[0], bl[1], tr[1], seg);
}

inline bool hit_v_bound(int cx, int ymin, int ymax, const path_segment* seg) {
    R2 left(seg->left());
    R2 right(seg->right());
    R2 top(seg->top());
    R2 bot(seg->bot());
    if(left[0] < cx && right[0] > cx) {
        if(bot[1] > ymin && top[1] < ymax) {
            return true;
        } else if(seg->implicit_value(cx, ymin) * seg->implicit_value(cx, ymax) < 0) {
            return true;
        }
    }
    return false;
}

inline bool hit_h_bound(int cy, int xmin, int xmax, const path_segment* seg) {
    R2 left(seg->left());
    R2 right(seg->right());
    R2 top(seg->top());
    R2 bot(seg->bot());
    if(bot[1] < cy && top[1] > cy) {
        if(left[0] > xmin && right[0] < xmax) {
            return true;
        } else if(seg->implicit_value(xmin, cy) != seg->implicit_value(xmax, cy)) {
            return true;
        }
    }
    return false;
}

// ----- scene object

inline bool scene_object::satisfy_wrule(int winding) const {
    if(m_wrule == e_winding_rule::non_zero){
        return (winding != 0);
    }
    else if(m_wrule == e_winding_rule::odd){
        return ((winding % 2)!= 0);
    }
    return false;
}

inline RGBA8 scene_object::get_color(const double x, const double y) const {
    return color->solve(x, y);
}

// ----- node object

inline void node_object::add_segment(const path_segment* segment, bool shortcut) {
    if(shortcut) {
        m_shortcuts.push_back(segment);
    } else {
        m_segments.push_back(segment);
    }
}

inline void node_object::increment(int inc) {
    m_w_increment += inc;
}

inline RGBA8 node_object::get_color(const double x, const double y) const {
    return m_ptr->get_color(x, y);
}

inline int node_object::get_increment() const {
    return m_w_increment;
}

inline int node_object::get_size() const {
    return m_segments.size() + m_shortcuts.size();
}

inline const std::vector<const path_segment*> node_object::get_all_segments() const {
    std::vector<const path_segment*> all_seg;
    all_seg.insert(all_seg.end(), m_segments.begin(), m_segments.end());
    all_seg.insert(all_seg.end(), m_shortcuts.begin(), m_shortcuts.end());
    return all_seg;
}

inline const std::vector<const path_segment*> node_object::get_shortcuts() const {
    return m_shortcuts;
}

// ----- abstract tree node

inline bool tree_node::intersect(const bouding_box& bbox) const {
    return m_bbox.intersect(bbox);
}

inline bool tree_node::is_in_cell(const double &x, const double &y) const {
    return x >= (double) m_p0[0] && x < (double) m_p1[0] && y >= (double) m_p0[1] && y < (double) m_p1[1];
}

// ----- concrete leave node

inline void leave_node::add_node_object(const node_object &node_obj) {
    m_objects.push_back(node_obj);
    m_n_segments += node_obj.get_size();
}

inline const std::vector<node_object>& leave_node::get_objects() const {
    return m_objects;
}

}}}
