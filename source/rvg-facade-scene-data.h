#ifndef RVG_FACADE_SCENE_DATA_H
#define RVG_FACADE_SCENE_DATA_H

#include <initializer_list>
#include <vector>

#include "rvg-scene-data.h"
#include "rvg-facade-painted-compound.h"

namespace rvg_facade {

rvg::scene_data make_scene_data(const painted_compound &p);
rvg::scene_data make_scene_data(const std::initializer_list<painted_compound> &p);
rvg::scene_data make_scene_data(const std::vector<painted_compound> &p);

} // namespace rvg::facade

#endif
