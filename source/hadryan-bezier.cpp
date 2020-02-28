#include "hadryan-bezier.h"

namespace rvg {
    namespace driver {
        namespace png {

// ----- bouding box

bouding_box::bouding_box()
    : m_p0(0, 0)
    , m_p1(0, 0)
{}
bouding_box::bouding_box(const R2 &first, const R2 &last) {
    m_p0 = make_R2(std::min(first[0], last[0]), std::min(first[1], last[1]));
    m_p1 = make_R2(std::max(first[0], last[0]), std::max(first[1], last[1]));
}

// ----- abstract path segment

path_segment::path_segment(const R2 &p0, const R2 &p1)
    : m_pi(p0)
    , m_pf(p1)
    , m_right((m_pf[0] > m_pi[0]) ? m_pf : m_pi)
    , m_dir((m_pi[1] > m_pf[1]) ? -1 : 1) 
    , m_sh_dir(-1*m_dir)  
    , m_bbox(m_pi, m_pf) { 
    if((m_right == m_pf && m_dir == 1)
        ||(m_right == m_pi && m_dir == -1)) {
        m_sh_dir *= -1;
    }
} 

int path_segment::implicit_value(double x, double y) const {
    if(m_bbox.hit_inside(x, y)) {
        return implicit_hit(x, y) ? 1 : -1;
    } else if((m_right == m_pi && m_dir == -1) || (m_right == m_pf && m_dir == 1)) {
        // 0 em baixo esquerda
        // 0 em cima direita
        if((m_bbox.hit_left(x,y) && m_bbox.hit_down(x,y)) || (m_bbox.hit_right(x,y) && m_bbox.hit_up(x,y))) {
            return 0;
        } else if(m_bbox.hit_up(x,y) || m_bbox.hit_left(x,y)) {
            return 1;
        } else if(m_bbox.hit_down(x,y) || m_bbox.hit_right(x,y)) {
            return -1;
        }
    } else if((m_right == m_pi && m_dir == 1) || (m_right == m_pf && m_dir == -1)) {
        // 0 em cima esquerda
        // 0 em baixo direita
        if((m_bbox.hit_left(x,y) && m_bbox.hit_up(x,y)) || (m_bbox.hit_right(x,y) && (m_bbox.hit_down(x,y)))) {
            return 0;
        } else if(m_bbox.hit_up(x,y) || m_bbox.hit_right(x,y)) {
            return -1;
        } else if(m_bbox.hit_down(x,y) || m_bbox.hit_left(x,y)) {
            return 1;
        }
    }
    return 0;
}

// ----- linear concrete path segment

linear::linear(const R2 &p0, const R2 &p1)
    : path_segment(p0, p1)
    , m_d(p1-p0) 
{}

// ----- quadratic concrete path segment

quadratic::quadratic(const R2 &p0, const R2 &p1, const R2& p2, double w) 
    : path_segment(p0, p2)
    , m_p1(p1-(p0*w))
    , m_p2(p2-p0)
    , m_diag(make_R2(0, 0), m_p2)
    , m_cvx(m_diag.implicit_hit(m_p1[0], m_p1[1]))
    , m_A(4.0*m_p1[0]*m_p1[0]-4.0*w*m_p1[0]*m_p2[0]+m_p2[0]*m_p2[0])
    , m_B(4.0*m_p1[0]*m_p2[0]*m_p1[1]-4.0*m_p1[0]*m_p1[0]*m_p2[1])
    , m_C(-4.0*m_p2[0]*m_p1[1]*m_p1[1]+4.0*m_p1[0]*m_p1[1]*m_p2[1])
    , m_D(-8.0*m_p1[0]*m_p1[1]+4.0*w*m_p2[0]*m_p1[1]+4.0*w*m_p1[0]*m_p2[1]-2.0*m_p2[0]*m_p2[1])
    , m_E(4.0*m_p1[1]*m_p1[1]-4.0*w*m_p1[1]*m_p2[1]+m_p2[1]*m_p2[1]) 
    , m_der((2*m_p2[1]*(-m_p2[0]*m_p1[1]+m_p1[0]*m_p2[1]))) 
{}

// ----- cubic concrete path segment

cubic::cubic(const R2 &p0, const R2 &p1, const R2 &p2, const R2 &p3)
    : path_segment(p0, p3) {
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

}}}