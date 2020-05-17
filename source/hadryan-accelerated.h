#ifndef HADRYAN_ACCELERATED_H
#define HADRYAN_ACCELERATED_H

#include <vector>

#include "rvg-point.h"

using namespace rvg;

namespace hadryan {

class scene_object;
class tree_node;

class accelerated {
public:
    std::vector<scene_object*> objects;
    tree_node* root = nullptr;
    std::vector<R2> samples;
    int threads;
public:
    accelerated();
    void destroy();
    void add(scene_object* obj);
    void invert();
};

inline accelerated::accelerated()
    : samples{make_R2(0, 0)}
    , threads(1)
{}

inline void accelerated::add(scene_object* obj){
    objects.push_back(obj);
}

inline void accelerated::invert() {
    std::reverse(objects.begin(), objects.end());
}

} // hadryan

#endif // HADRYAN_ACCELERATED_H