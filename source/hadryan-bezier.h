#pragma once

#include "rvg-point.h"
#include <vector>

#define BEZIER_EPS 0.00001

namespace rvg {
    namespace driver {
        namespace png {

class bouding_box {
private:
    R2 m_p0;
    R2 m_p1;
public: 
    bouding_box();
    bouding_box(const R2 &first, const R2 &last);
    bool hit_up(const double x, const double y) const;
    bool hit_down(const double x, const double y) const;
    bool hit_left(const double x, const double y) const;
    bool hit_right(const double x, const double y) const;
    bool hit_inside(const double x, const double y) const;
    bool intersect(const bouding_box &rhs) const;
};

class path_segment {
protected:
    const R2 m_pi;
    const R2 m_pf;
    const R2 m_right;

    int m_dir;
    int m_sh_dir;
public:
    const bouding_box m_bbox;
public:
    path_segment(const R2 &p0, const R2 &p1);
    virtual ~path_segment() = default;
    
    virtual bool implicit_hit(double x, double y) const = 0;
    int implicit_value(double x, double y) const;
    
    bool intersect(const double x, const double y) const;
    bool intersect_shortcut(const double x, const double y) const;
    
    int get_dir() const;
    int get_sh_dir() const;
    
    R2 first() const;
    R2 last()  const;
    R2 left()  const;
    R2 right() const;
    R2 top()   const;
    R2 bot()   const;
};

class linear : public path_segment {
private:
    const R2 m_d;
public:
    linear(const R2 &p0, const R2 &p1);
    bool implicit_hit(double x, double y) const;
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
    quadratic(const R2 &p0, const R2 &p1, const R2& p2, double w = 1.0);
    bool implicit_hit(double x, double y) const;
    bool hit_me(double x, double y) const;
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
    cubic(const R2 &p0, const R2 &p1, const R2 &p2, const R2 &p3);
    int triangle_hits(double x, double y) const;
    bool hit_me(double x, double y) const;
    bool implicit_hit(double x, double y) const;
};

// ----- bouding box

inline bool bouding_box::hit_up(const double x, const double y) const {
    (void) x;
    return y >= m_p1.get_y();
}

inline bool bouding_box::hit_down(const double x, const double y) const {
    (void) x;
    return y < m_p0.get_y();
}

inline bool bouding_box::hit_left(const double x, const double y) const {
    (void) y;
    return x <= m_p0.get_x();
}

inline bool bouding_box::hit_right(const double x, const double y) const {
    (void) y;
    return x > m_p1.get_x();
}

inline bool bouding_box::hit_inside(const double x, const double y) const {
    return y >= m_p0.get_y() && y < m_p1.get_y() && x >= m_p0.get_x() && x < m_p1.get_x();
}

inline bool bouding_box::intersect(const bouding_box &rhs) const {
    return m_p1[0] > rhs.m_p0[0] &&
           m_p1[1] > rhs.m_p0[1] &&
           rhs.m_p1[0] > m_p0[0] &&
           rhs.m_p1[1] > m_p0[1];
}

// ----- abstract path segment

inline bool path_segment::intersect(const double x, const double y) const {
    return !(m_bbox.hit_up(x,  y) || m_bbox.hit_right(x, y) || m_bbox.hit_down(x, y)) 
          &&(m_bbox.hit_left(x,y) || implicit_hit(x, y));
}

inline bool path_segment::intersect_shortcut(const double x, const double y) const {
    return (x < m_right[0] && y >= m_right[1]);
}

inline int path_segment::get_dir() const {
    return m_dir;
}

inline int path_segment::get_sh_dir() const  {
    return m_sh_dir;
}

inline R2 path_segment::first() const {
    return m_pi;
}

inline R2 path_segment::last() const {
    return m_pf;
}

inline R2 path_segment::left() const {
    return (m_pi[0] < m_pf[0]) ? m_pi : m_pf;
}

inline R2 path_segment::right() const {
    return (m_pi[0] >= m_pf[0]) ? m_pi : m_pf;
}

inline R2 path_segment::top() const {
    return (m_pi[1] >= m_pf[1]) ? m_pi : m_pf;
}

inline R2 path_segment::bot() const { 
    return (m_pi[1] < m_pf[1]) ? m_pi : m_pf;
}

// ----- linear concrete path segment

inline bool linear::implicit_hit(double x, double y) const {
    return (m_d[1]*((x - m_pi[0])*m_d[1] - (y - m_pi[1])*m_d[0]) <= 0);
}

// ----- quadratic concrete path segment

inline bool quadratic::implicit_hit(double x, double y) const {
    x -= m_pi[0];
    y -= m_pi[1];
    bool diag_hit = m_diag.implicit_hit(x, y);
    return(m_cvx && (diag_hit && hit_me(x, y)))
       ||(!m_cvx && (diag_hit || hit_me(x, y)));
}

inline bool quadratic::hit_me(double x, double y) const {
    return m_der*((y*(y*m_A + m_B) + x*(m_C + y*m_D + x*m_E))) <= 0;
}

// ----- cubic concrete path segment

inline bool cubic::hit_me(double x, double y) const {
    return (m_der*(y*(A + y*(y*(B) + C)) + x*(D + y*(E + y*F) + x*(G + y*H + x*I)))) <= 0;
}

inline bool cubic::implicit_hit(double x, double y) const {
    x -= m_pi[0];
    y -= m_pi[1];
    int hits = triangle_hits(x, y);
    return (hits == 2 || 
           (hits == 1 && hit_me(x, y)));
}

}}}