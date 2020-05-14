#include "hadryan-cubic-path-segment.h"

using namespace rvg;

namespace hadryan {

cubic::cubic(const R2 &p0, const R2 &p1, const R2 &p2, const R2 &p3)
    : path_segment(p0, p3) {
    constexpr double BEZIER_EPS = 0.0001f;
    double x1, x2, x3;
    double y1, y2, y3;
    x1 = (p1 - p0).get_x();
    y1 = (p1 - p0).get_y();
    x2 = (p2 - p0).get_x();
    y2 = (p2 - p0).get_y();
    x3 = (p3 - p0).get_x();
    y3 = (p3 - p0).get_y();
    A = -27.0*x1*x3*x3*y1*y1 + 81.0*x1*x2*x3*y1*y2 - 81.0*x1*x1*x3*y2*y2 - 
            81.0*x1*x2*x2*y1*y3 + 54.0*x1*x1*x3*y1*y3 + 81.0*x1*x1*x2*y2*y3 - 
            27.0*x1*x1*x1*y3*y3;
    B = (-27.0*x1*x1*x1 + 81.0*x1*x1*x2 - 81.0*x1*x2*x2 + 27.0*x2*x2*x2 - 27.0*x1*x1*x3 + 
            54.0*x1*x2*x3 - 27.0*x2*x2*x3 - 9.0*x1*x3*x3 + 9.0*x2*x3*x3 - 
            x3*x3*x3);
    C = 81.0*x1*x2*x2*y1 - 54.0*x1*x1*x3*y1 - 81.0*x1*x2*x3*y1 + 
        54.0*x1*x3*x3*y1 - 9.0*x2*x3*x3*y1 - 81.0*x1*x1*x2*y2 + 
        162.0*x1*x1*x3*y2 - 81.0*x1*x2*x3*y2 + 27.0*x2*x2*x3*y2 - 
        18*x1*x3*x3*y2 + 54.0*x1*x1*x1*y3 - 81.0*x1*x1*x2*y3 + 81.0*x1*x2*x2*y3 - 
        27.0*x2*x2*x2*y3 - 54.0*x1*x1*x3*y3 + 27.0*x1*x2*x3*y3;
    D = 27.0*x3*x3*y1*y1*y1 - 81.0*x2*x3*y1*y1*y2 + 81.0*x1*x3*y1*y2*y2 + 
        81.0*x2*x2*y1*y1*y3 - 54.0*x1*x3*y1*y1*y3 - 81.0*x1*x2*y1*y2*y3 + 
        27.0*x1*x1*y1*y3*y3;
    E = -81.0*x2*x2*y1*y1 + 108.0*x1*x3*y1*y1 + 81.0*x2*x3*y1*y1 - 
        54.0*x3*x3*y1*y1 - 243*x1*x3*y1*y2 + 81.0*x2*x3*y1*y2 + 
        27.0*x3*x3*y1*y2 + 81.0*x1*x1*y2*y2 + 81.0*x1*x3*y2*y2 - 54.0*x2*x3*y2*y2 - 
        108*x1*x1*y1*y3 + 243*x1*x2*y1*y3 - 81.0*x2*x2*y1*y3 - 
        9.0*x2*x3*y1*y3 - 81.0*x1*x1*y2*y3 - 81.0*x1*x2*y2*y3 + 
        54.0*x2*x2*y2*y3 + 9.0*x1*x3*y2*y3 + 54.0*x1*x1*y3*y3 - 27.0*x1*x2*y3*y3;
    F = 81.0*x1*x1*y1 - 162.0*x1*x2*y1 + 81.0*x2*x2*y1 + 54.0*x1*x3*y1 - 
        54.0*x2*x3*y1 + 9.0*x3*x3*y1 - 81.0*x1*x1*y2 + 162.0*x1*x2*y2 - 
        81.0*x2*x2*y2 - 54.0*x1*x3*y2 + 54.0*x2*x3*y2 - 9.0*x3*x3*y2 + 
        27.0*x1*x1*y3 - 54.0*x1*x2*y3 + 27.0*x2*x2*y3 + 18*x1*x3*y3 - 
        18*x2*x3*y3 + 3.0*x3*x3*y3;
    G = -54.0*x3*y1*y1*y1 + 81.0*x2*y1*y1*y2 + 81.0*x3*y1*y1*y2 - 81.0*x1*y1*y2*y2 - 
        81.0*x3*y1*y2*y2 + 27.0*x3*y2*y2*y2 + 54.0*x1*y1*y1*y3 - 162.0*x2*y1*y1*y3 + 
        54.0*x3*y1*y1*y3 + 81.0*x1*y1*y2*y3 + 81.0*x2*y1*y2*y3 - 
        27.0*x3*y1*y2*y3 - 27.0*x2*y2*y2*y3 - 54.0*x1*y1*y3*y3 + 
        18*x2*y1*y3*y3 + 9.0*x1*y2*y3*y3;
    H = -81.0*x1*y1*y1 + 81.0*x2*y1*y1 - 27.0*x3*y1*y1 + 162.0*x1*y1*y2 - 
        162.0*x2*y1*y2 + 54.0*x3*y1*y2 - 81.0*x1*y2*y2 + 81.0*x2*y2*y2 - 
        27.0*x3*y2*y2 - 54.0*x1*y1*y3 + 54.0*x2*y1*y3 - 18.0*x3*y1*y3 + 
        54.0*x1*y2*y3 - 54.0*x2*y2*y3 + 18.0*x3*y2*y3 - 9.0*x1*y3*y3 + 
        9.0*x2*y3*y3 - 3.0*x3*y3*y3;
    I = 27.0*y1*y1*y1 - 81.0*y1*y1*y2 + 81.0*y1*y2*y2 - 27.0*y2*y2*y2 + 27.0*y1*y1*y3 - 
        54.0*y1*y2*y3 + 27.0*y2*y2*y3 + 9.0*y1*y3*y3 - 9.0*y2*y3*y3 + y3*y3*y3;
    m_der = ((y1 - y2 - y3)*(-x3*x3*(4.0*y1*y1 - 2.0*y1*y2 + y2*y2) + 
                x1*x1*(9.0*y2*y2 - 6.0*y2*y3 - 4.0*y3*y3) + 
                x2*x2*(9.0*y1*y1 - 12*y1*y3 - y3*y3) + 
                2*x1*x3*(-y2*(6.0*y2 + y3) + y1*(3.0*y2 + 4.0*y3)) - 
                2*x2*(x3*(3.0*y1*y1 - y2*y3 + y1*(-6.0*y2 + y3)) + 
                x1*(y1*(9.0*y2 - 3.0*y3) - y3*(6.0*y2 + y3))))
    );
    R2 v0(0.0, 0.0);
    R2 v1;
    R2 v2(x3, y3);
    if(std::abs(x1*x1) < BEZIER_EPS && std::abs(y1*y1) < BEZIER_EPS) {
        v1 = make_R2(x2, y2);
    }
    else if(std::abs(x3-x2)*std::abs(x3-x2) < BEZIER_EPS && std::abs(y3-y2)*std::abs(y3-y2) < BEZIER_EPS) {
        
        v1 = make_R2(x1, y1);
    }
    else if(std::abs(x2-x1)*std::abs(x2-x1) < BEZIER_EPS && std::abs(y2-y1)*std::abs(y2-y1) < BEZIER_EPS){
        v1 = make_R2(x2, y2);
    }
    else {
        v1 = make_R2(-x1*(x2*y3 - x3*y2)/(x1*y2 - x1*y3 - x2*y1 + x3*y1), 
                        -y1*(x2*y3 - x3*y2)/(x1*y2 - x1*y3 - x2*y1 + x3*y1));
    }
    m_tri.push_back(linear(v0, v1));
    m_tri.push_back(linear(v1, v2));
    m_tri.push_back(linear(v2, v0));
}

int cubic::triangle_hits(double x, double y) const {
    int sum = 0;
    for(auto &seg : m_tri) {
        if(seg.intersect(x, y)) {
            sum++;
        }
    }
    return sum;
}

bool cubic::implicit_hit(double x, double y) const {
    x -= m_pi[0];
    y -= m_pi[1];
    int hits = triangle_hits(x, y);
    return (hits == 2 || 
           (hits == 1 && hit_me(x, y)));
}

} // hadryan