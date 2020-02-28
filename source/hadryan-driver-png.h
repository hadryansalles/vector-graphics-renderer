#ifndef RVG_DRIVER_PNG_H
#define RVG_DRIVER_PNG_H

#include <iosfwd>
#include <string>
#include <vector>

#include "rvg-image.h"
#include "rvg-viewport.h"
#include "rvg-window.h"
#include "rvg-scene.h"

#include "hadryan-shortcut-tree.h"

namespace rvg {
    namespace driver {
        namespace png {

class accelerated {
public:
    std::vector<scene_object*> objects;
    tree_node* root = nullptr;
    std::vector<R2> samples;
    int threads;
    R2 debug;
    bool debugging;
public:
    accelerated();
    void destroy();
    void add(scene_object* obj);
    void invert();
};

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args =
        std::vector<std::string>());

// ----- accelerated

inline accelerated::accelerated()
    : samples{make_R2(0, 0)}
    , threads(1)
    , debug(0, 0)
    , debugging(false)
{}

inline void accelerated::add(scene_object* obj){
    objects.push_back(obj);
}

inline void accelerated::invert() {
    std::reverse(objects.begin(), objects.end());
}

} } } // namespace rvg::driver::png

#endif
