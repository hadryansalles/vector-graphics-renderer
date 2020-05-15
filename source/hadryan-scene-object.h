#ifndef HADRYAN_SCENE_OBJECT_H
#define HADRYAN_SCENE_OBJECT_H

#include <vector>

#include "rvg-paint.h"

#include "hadryan-path-segment.h"
#include "hadryan-color-solver.h"
#include "rvg-winding-rule.h"

using namespace rvg;

namespace hadryan {

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

} // hadryan

#endif // HADRYAN_SCENE_OBJECT_H