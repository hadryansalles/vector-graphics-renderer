#ifndef HADRYAN_NODE_OBJECT_H
#define HADRYAN_NODE_OBJECT_H

#include <vector>

#include "rvg-rgba.h"

#include "hadryan-path-segment.h"
#include "hadryan-scene-object.h"

using namespace rvg;

namespace hadryan {

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
};


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

} // hadryan

#endif // HADRYAN_NODE_OBJECT_H