#include "hadryan-accelerated.h"

#include "hadryan-tree-node.h"
#include "hadryan-scene-object.h"

namespace hadryan {

void accelerated::destroy() { 
    for(auto &obj : objects) {
        delete obj;
        obj = NULL;
    }
    root->destroy();
    delete root;
}

} // hadryan