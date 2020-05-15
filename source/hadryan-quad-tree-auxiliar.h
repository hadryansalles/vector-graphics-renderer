#ifndef HADRYAN_QUAD_TREE_AUXILIAR_H
#define HADRYAN_QUAD_TREE_AUXILIAR_H

#include "hadryan-path-segment.h"

namespace hadryan {

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

} // hadryan

#endif // HADRYAN_QUAD_TREE_AUXILIAR_H