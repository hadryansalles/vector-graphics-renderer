
#include "rvg-point.h"
#include <vector>

namespace rvg {
namespace driver {
namespace png {

class bouding_box {
private:
    R2 m_p0;
    R2 m_p1;
public: 
    inline bouding_box()
        : m_p0(0, 0)
        , m_p1(0, 0)
    {}
    inline bouding_box(const R2 &first, const R2 &last) {
        m_p0 = make_R2(std::min(first[0], last[0]), std::min(first[1], last[1]));
        m_p1 = make_R2(std::max(first[0], last[0]), std::max(first[1], last[1]));
    }
    inline bool hit_up(const double x, const double y) const {
        (void) x;
        return y >= m_p1.get_y();
    }
    inline bool hit_down(const double x, const double y) const {
        (void) x;
        return y < m_p0.get_y();
    }
    inline bool hit_left(const double x, const double y) const {
        (void) y;
        return x <= m_p0.get_x();
    }
    inline bool hit_right(const double x, const double y) const {
        (void) y;
        return x > m_p1.get_x();
    }
    inline bool hit_inside(const double x, const double y) const {
        return y >= m_p0.get_y() && y < m_p1.get_y() && x >= m_p0.get_x() && x < m_p1.get_x();
    }
    inline bool intersect(const bouding_box &rhs) const {
        return m_p1[0] > rhs.m_p0[0] &&
               m_p1[1] > rhs.m_p0[1] &&
               rhs.m_p1[0] > m_p0[0] &&
               rhs.m_p1[1] > m_p0[1];
    }
};

class path_segment {
protected:
    const R2 m_pi;
    const R2 m_pf;
    const R2 m_right;
    int m_dir;
    int m_sh_dir;
public:
    const bouding_box m_bbox; // segment bouding box
    path_segment(const R2 &p0, const R2 &p1)
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
    inline virtual ~path_segment() {
    }
    virtual bool implicit_hit(double x, double y) const = 0;
    inline int implicit_value(double x, double y) const {
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
        assert(true);
        return 0;
    }
    inline bool intersect(const double x, const double y) const {
        return !(m_bbox.hit_up(x, y) || m_bbox.hit_right(x, y) || m_bbox.hit_down(x, y)) 
              &&(m_bbox.hit_left(x,  y) || implicit_hit(x, y));
    }
    inline bool intersect_shortcut(const double x, const double y) const {
        return (x < m_right[0] && y >= m_right[1]);
    }
    inline int get_dir() const {
        return m_dir;
    }
    inline int get_sh_dir() const  {
        return m_sh_dir;
    }
    R2 first() const {
        return m_pi;
    }
    R2 last() const {
        return m_pf;
    }
    R2 left() const {
        return (m_pi[0] < m_pf[0]) ? m_pi : m_pf;
    }
    R2 right() const {
        return (m_pi[0] >= m_pf[0]) ? m_pi : m_pf;
    }
    R2 top() const {
        return (m_pi[1] >= m_pf[1]) ? m_pi : m_pf;
    }
    R2 bot() const { 
        return (m_pi[1] < m_pf[1]) ? m_pi : m_pf;
    }
};

class linear : public path_segment {
private:
    const R2 m_d;
public:
    inline linear(const R2 &p0, const R2 &p1)
        : path_segment(p0, p1)
        , m_d(p1-p0) 
    {}
    inline bool implicit_hit(double x, double y) const {
        return (m_d[1]*((x - m_pi[0])*m_d[1] - (y - m_pi[1])*m_d[0]) <= 0);
    }
};

class quadratic : public path_segment {
protected:
    const R2 m_p1;
    const R2 m_p2;
    const linear m_diag; 
    const bool m_cvx;
    const double m_A;
    const double m_B;
    const double m_C;
    const double m_D;
    const double m_E;
    const double m_der;
public:
    quadratic(const R2 &p0, const R2 &p1, const R2& p2, double w = 1.0) 
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
    inline bool implicit_hit(double x, double y) const {
        x -= m_pi[0];
        y -= m_pi[1];
        bool diag_hit = m_diag.implicit_hit(x, y);
        return(m_cvx && (diag_hit && hit_me(x, y)))
           ||(!m_cvx && (diag_hit || hit_me(x, y)));
    }
    inline bool hit_me(double x, double y) const {
        return m_der*((y*(y*m_A + m_B) + x*(m_C + y*m_D + x*m_E))) <= 0;
    }
};

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
    inline cubic(const R2 &p0, const R2 &p1, const R2 &p2, const R2 &p3)
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
        if(std::abs(x1*x1) < EPS && std::abs(y1*y1) < EPS) {
            v1 = make_R2(x2, y2);
        }
        else if(std::abs(x3-x2)*std::abs(x3-x2) < EPS && std::abs(y3-y2)*std::abs(y3-y2) < EPS) {
            
            v1 = make_R2(x1, y1);
        }
        else if(std::abs(x2-x1)*std::abs(x2-x1) < EPS && std::abs(y2-y1)*std::abs(y2-y1) < EPS){
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
    inline int triangle_hits(double x, double y) const {
        int sum = 0;
        for(auto &seg : m_tri) {
            if(seg.intersect(x, y)) {
                sum++;
            }
        }
        return sum;
    }
    inline bool hit_me(double x, double y) const {
        return (m_der*(y*(A + y*(y*(B) + C)) + x*(D + y*(E + y*F) + x*(G + y*H + x*I)))) <= 0;
    }
    inline bool implicit_hit(double x, double y) const {
        x -= m_pi[0];
        y -= m_pi[1];
        int hits = triangle_hits(x, y);
        return (hits == 2 || 
               (hits == 1 && hit_me(x, y)));
    }
};

}}}