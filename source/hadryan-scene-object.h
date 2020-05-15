#ifndef HADRYAN_SCENE_OBJECT_H
#define HADRYAN_SCENE_OBJECT_H

#include <vector>
#include <memory>

#include "rvg-paint.h"

#include "hadryan-path-segment.h"
#include "hadryan-color-solver.h"
#include "rvg-winding-rule.h"

using namespace rvg;

namespace hadryan {

class scene_object {
private:
    e_winding_rule m_wrule;
    std::unique_ptr<color_solver> m_color;
    std::vector<path_segment*> m_path;
    bouding_box m_bbox;

    scene_object(const scene_object &rhs) = delete;
    scene_object& operator=(const scene_object &rhs) = delete;
public:

public:
    scene_object(std::vector<path_segment*> &path, const e_winding_rule &wrule, const paint &paint_in);
    ~scene_object();
    RGBA8 get_color(const double x, const double y) const;
    bool satisfy_wrule(int winding) const;

    const auto& get_path() const {return m_path;}
    const bouding_box& get_bbox() const {return m_bbox;}
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
    return m_color->solve(x, y);
}

} // hadryan

#endif // HADRYAN_SCENE_OBJECT_H