#ifndef HADRYAN_LINEAR_PATH_SEGMENT_H
#define HADRYAN_LINEAR_PATH_SEGMENT_H

#include "hadryan-path-segment.h"

using namespace rvg;

namespace hadryan {

class linear : public path_segment {
public:
    linear(const R2 &p0, const R2 &p1);    
    bool implicit_hit(double x, double y) const;
    
private:
    const R2 m_d;
};

} // hadryan

#endif // HADRYAN_LINEAR_PATH_SEGMENT_H
