#ifndef HADRYAN_CUBIC_PATH_SEGMENT_H
#define HADRYAN_CUBIC_PATH_SEGMENT_H

#include <vector>

#include "hadryan-linear-path-segment.h"

using namespace rvg;

namespace hadryan {

class cubic : public path_segment {
    double A;
    double B;
    double C;
    double D;
    double E;
    double F;
    double G;
    double H;
    double I;
    double m_der;
    std::vector<linear> m_tri; 
public:
    cubic(const R2 &p0, const R2 &p1, const R2 &p2, const R2 &p3);
    int triangle_hits(double x, double y) const;
    bool hit_me(double x, double y) const;
    bool implicit_hit(double x, double y) const;
};

inline bool cubic::hit_me(double x, double y) const {
    return (m_der*(y*(A + y*(y*(B) + C)) + x*(D + y*(E + y*F) + x*(G + y*H + x*I)))) <= 0;
}

} // hadryan

#endif // HADRYAN_CUBIC_PATH_SEGMENT_H 