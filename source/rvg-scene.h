#ifndef RVG_SCENE_H
#define RVG_SCENE_H

#include "rvg-i-xformable.h"
#include "rvg-scene-data.h"

namespace rvg {

// This is simply an input scene that can be xformed
class scene: public i_xformable<scene> {

    scene_data::const_ptr m_scene_ptr;

public:

    scene(scene_data::const_ptr scene_ptr):
        m_scene_ptr(scene_ptr) { ; }

    const scene_data &get_scene_data(void) const {
        return *m_scene_ptr;
    }

    scene_data::const_ptr get_scene_data_ptr(void) const {
        return m_scene_ptr;
    }

};

}

#endif
