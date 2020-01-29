#include "hadryan-path-builder.h"

#define EPS 0.0000000001

namespace rvg {
    namespace driver {
        namespace png {


bool almost_zero(const double& x) {
    return (std::abs(x) < EPS);
}

bouding_box::bouding_box(std::vector<R2> &points){
    assert(points.size() > 1);
    R2 first = points[0];
    R2 last = points[points.size()-1];
    m_p0 = make_R2(std::min(first.get_x(), last.get_x()), std::min(first.get_y(), last.get_y()));
    m_p1 = make_R2(std::max(first.get_x(), last.get_x()), std::max(first.get_y(), last.get_y()));
}

bool bouding_box::hit_left(const double x, const double y) const {
    return y >= m_p0.get_y() && y < m_p1.get_y() && x <= m_p0.get_x();
}

bool bouding_box::hit_right(const double x, const double y) const {
    return y >= m_p0.get_y() && y < m_p1.get_y() && x > m_p1.get_x();
}

bool bouding_box::hit_inside(const double x, const double y) const {
    return y >= m_p0.get_y() && y < m_p1.get_y() && x >= m_p0.get_x() && x < m_p1.get_x();
}

namespace monotonic {

    path_segment::path_segment(std::vector<R2> &points) 
        : m_bbox(points) 
        , m_dir(1) { 
        for (auto point : points){
            m_points.push_back(point);
        }
        if(m_points[0].get_y() > m_points[m_points.size()-1].get_y()){
            m_dir = -1;
        }
    }

    R2 path_segment::p_in_t(const double t) const {
        return make_R2(x_in_t(t), y_in_t(t));
    }

    bool path_segment::intersect(const double x, const double y) const {
        if(!m_bbox.hit_left(x, y)){
            if(m_bbox.hit_right(x, y)){
                return true;
            }
            else{
                double bisection_y;
                double step = 0.5;
                double t = 0.5;
                do {
                    step /= 2.0;
                    bisection_y = y_in_t(t); 
                    if(bisection_y > y){
                        t += (double)m_dir*step;
                    }
                    else if(bisection_y < y){
                        t -= (double)m_dir*step;
                    }
                } while(bisection_y != y || step > 0.01);
                return x-x_in_t(t) < EPS;
            }
        }
        return 0;
    }

    int path_segment::get_dir() const {
        return m_dir;
    }

    linear_segment::linear_segment(std::vector<R2> &points)
        : path_segment(points) {
        assert(points.size() == 2);
    }

    double linear_segment::x_in_t(const double t) const {
        return t*m_points[0].get_x() + t*m_points[1].get_x();
    }

    double linear_segment::y_in_t(const double t) const {
        return t*m_points[0].get_y() + t*m_points[1].get_y();
    }

    quadratic_segment::quadratic_segment(std::vector<R2> &points)
        : path_segment(points){
            assert(points.size() == 3);
    }

    double quadratic_segment::x_in_t(const double t) const {
        return m_points[0].get_x()*(1-t)*(1-t) + m_points[1].get_x()*2*(t - t*t) + m_points[2].get_x()*t*t;
    }

    double quadratic_segment::y_in_t(const double t) const {
        return m_points[0].get_y()*(1-t)*(1-t) + m_points[1].get_y()*2*(t - t*t) + m_points[2].get_y()*t*t;
    }

    rational_quadratic_segment::rational_quadratic_segment(std::vector<R2> &points, std::vector<R2> &den_points)
        : quadratic_segment(points)
        , den(den_points) {
    }

    double rational_quadratic_segment::x_in_t(const double t) const {
        return quadratic_segment::x_in_t(t)/den.x_in_t(t);
    }

    double rational_quadratic_segment::y_in_t(const double t) const {
        return quadratic_segment::y_in_t(t)/den.y_in_t(t);
    }

    cubic_segment::cubic_segment(std::vector<R2> &points) 
        : path_segment(points){
        assert(points.size() == 4);
    }

    double cubic_segment::x_in_t(const double t) const {
        return m_points[0].get_x()*(1-t)*(1-t)*(1-t) + 
            m_points[1].get_x()*3.0*(1-t)*(1-t)*t + 
            m_points[2].get_x()*3.0*t*t*(1-t) + 
            m_points[3].get_x()*t*t*t;
    }

    double cubic_segment::y_in_t(const double t) const {
        return m_points[0].get_y()*(1-t)*(1-t)*(1-t) + 
            m_points[1].get_y()*3.0*(1-t)*(1-t)*t + 
            m_points[2].get_y()*3.0*t*t*(1-t) + 
            m_points[3].get_y()*t*t*t;
    }

} // end monotonic

namespace raw {

    path_segment::path_segment() 
        : m_w(1.0)
    {}

    path_segment::~path_segment() {
        m_points.clear();
    }

    void path_segment::apply(const xform& xf) {
        for(auto& point : m_points) {
            point = make_R2(xf[0][0]*point.get_x() + xf[0][1]*point.get_y() + m_w*xf[0][2],
                            xf[1][0]*point.get_x() + xf[1][1]*point.get_y() + m_w*xf[1][2]);
        }
    }

    R2 path_segment::blossom(double a, double b) const {
        assert(m_points.size() == 3);
        double x = (1-a)*((1-b)*m_points[0].get_x() + b*m_points[1].get_x()) + a*((1-b)*m_points[1].get_x() + b*m_points[2].get_x());
        double y = (1-a)*((1-b)*m_points[0].get_y() + b*m_points[1].get_y()) + a*((1-b)*m_points[1].get_y() + b*m_points[2].get_y());
        return make_R2(x, y);   
    }

    R2 path_segment::blossom(double a, double b, double c) const {
        assert(m_points.size() == 4);
        double x = (((1-a)*m_points[0].get_x() +
                    (a)*m_points[1].get_x())*(1-b) +
                    ((1-a)*m_points[1].get_x() +
                    (a)*m_points[2].get_x())*(b)) * (1-c) + 
                (((1-a)*m_points[1].get_x() +
                    (a)*m_points[2].get_x())*(1-b) +
                    ((1-a)*m_points[2].get_x() +
                    (a)*m_points[3].get_x())*(b)) * (c);
        double y = (((1-a)*m_points[0].get_y() +
                    (a)*m_points[1].get_y())*(1-b) +
                    ((1-a)*m_points[1].get_y() +
                    (a)*m_points[2].get_y())*(b)) * (1-c) + 
                (((1-a)*m_points[1].get_y() +
                    (a)*m_points[2].get_y())*(1-b) +
                    ((1-a)*m_points[2].get_y() +
                    (a)*m_points[3].get_y())*(b)) * (c);
        return make_R2(x, y);
    }

    std::vector<double> path_segment::roots(double A, double B) {
        // double A = a - 2*b + c;
        // double B = a - b;
        std::vector<double> r_roots;
        if(!almost_zero(A)){
            r_roots.push_back(B/A);
        }
        return r_roots;
    }

    std::vector<double> path_segment::roots(double A, double B, double C) {
        // double A = 3*d - 9*c + 9*b - 3*a;
        // double B = 6*c - 12*b + 6*a;
        // double C = 3*b - 3*a;
        // double A = -2*a*w + 2*a + 2*w*c - 2*c;
        // double B = 4*a*w - 2*a - 4*b + 2*c;
        // double C = -2*a*w + 2*b;
        std::vector<double> r_roots;
        double delta = B*B - 4*A*C;
        if(!almost_zero(A)){
            if(delta > 0) {
                double sqrt = std::sqrt(delta);
                r_roots.push_back((-B+sqrt)/(2*A));
                r_roots.push_back((-B-sqrt)/(2*A));
            }
            else if(almost_zero(delta)) {
                r_roots.push_back(-B/(2*A));
            }
        }   
        else{
            r_roots.push_back(-C/B);
        }
        return r_roots;
    } 

    // remove almost 0 or almost 1 and almost equals
    std::vector<double> path_segment::organized_roots(std::vector<double> &x_roots, std::vector<double> &y_roots) {
        std::vector<double> roots;
        roots.insert(roots.end(), x_roots.begin(), x_roots.end());
        roots.insert(roots.end(), y_roots.begin(), y_roots.end());
        roots.push_back(0.f);
        roots.push_back(1.f);
        roots.erase(std::unique(roots.begin(), roots.end()), roots.end());
        std::sort(roots.begin(), roots.end());
        return roots;
        // REMOVE LESS GREATER 0 1
    }

    linear_segment::linear_segment(R2 p1, R2 p2)
        : path_segment() {
        m_points.push_back(p1);
        m_points.push_back(p2);
    }

    std::vector<R2> linear_segment::reparametrize(double t0, double t1) const {
        std::vector<R2> r_points {(1.0-t0)*m_points[0] + t0*m_points[1], 
                                  (1.0-t1)*m_points[0] + t1*m_points[1]};
        return r_points;
    }

    std::vector<double> linear_segment::d_roots() const {
        std::vector<double> r_roots;
        return r_roots;
    }

    quadratic_segment::quadratic_segment(R2 p1, R2 p2, R2 p3)
        : path_segment() {
        m_points.push_back(p1);
        m_points.push_back(p2);
        m_points.push_back(p3);
    }

    std::vector<R2> quadratic_segment::reparametrize(double t0, double t1) const {
        std::vector<R2> r_points;
        r_points.push_back(blossom(t0, t0));
        r_points.push_back(blossom(t0, t1));
        r_points.push_back(blossom(t1, t1));
        return r_points;
    }

    std::vector<double> quadratic_segment::d_roots() const {
        // A = a - 2*b + c;
        // B = a - b;
        R2 A = m_points[0] - 2*m_points[1] + m_points[2];
        R2 B = m_points[0] - m_points[1];
        std::vector<double> x_roots = roots(A.get_x(), B.get_x());
        std::vector<double> y_roots = roots(A.get_y(), B.get_y());
        return organized_roots(x_roots, y_roots);
    }

    rational_quadratic_segment::rational_quadratic_segment(R2 p1, R2 p2, R2 p3, double w) 
        : quadratic_segment(p1, p2, p3)
        , den(make_R2(1.0, 1.0), make_R2(w, w), make_R2(1.0, 1.0)) {
        m_w = w;
    }

    std::vector<R2> rational_quadratic_segment::reparametrize(double t0, double t1) const {
        std::vector<R2> pp = quadratic_segment::reparametrize(t0, t1);
        std::vector<R2> pw = den.reparametrize(t0, t1);
        std::vector<R2> r_points;
        r_points.push_back(make_R2(pp[0].get_x()/pw[0].get_x(), pp[0].get_y()/pw[0].get_y()));
        r_points.push_back(make_R2(pp[1].get_x()/std::sqrt(pw[0].get_x()*pw[2].get_x()), 
                                pp[1].get_y()/std::sqrt(pw[0].get_y()*pw[2].get_y())));
        r_points.push_back(make_R2(pp[2].get_x()/pw[2].get_x(), pp[2].get_y()/pw[2].get_y()));
        return r_points;
    } 

    std::vector<R2> rational_quadratic_segment::reparametrize_den(double t0, double t1) const {
        std::vector<R2> w = den.reparametrize(t0, t1);
        w[1] = make_R2(w[1].get_x()/std::sqrt(w[0].get_x()*w[2].get_x()),
                       w[1].get_y()/std::sqrt(w[0].get_y()*w[2].get_y()));
        w[0] = make_R2(1.0, 1.0);
        w[2] = make_R2(1.0, 1.0);
        return w;
    }

    std::vector<double> rational_quadratic_segment::d_roots() const {
        // A = -2*a*w + 2*a + 2*w*c - 2*c;
        // B = 4*a*w - 2*a - 4*b + 2*c;
        // C = -2*a*w + 2*b;
        R2 A = 2.0*(m_points[0] - m_points[2])*(1 - m_w); 
        R2 B = 2.0*(m_points[0]*(2.0*m_w - 1) + m_points[2] - 2.0*m_points[1]);
        R2 C = 2.0*(m_points[1] - m_points[0]*m_w);
        std::vector<double> x_roots = roots(A.get_x(), B.get_x(), C.get_x());
        std::vector<double> y_roots = roots(A.get_y(), B.get_y(), C.get_y());
        return organized_roots(x_roots, y_roots);
    }

    cubic_segment::cubic_segment(R2 p1, R2 p2, R2 p3, R2 p4) 
        : path_segment() {
        m_points.push_back(p1);
        m_points.push_back(p2);
        m_points.push_back(p3);
        m_points.push_back(p4);
    }

    std::vector<R2> cubic_segment::reparametrize(double t0, double t1) const {
        std::vector<R2> r_points;
        r_points.push_back(blossom(t0, t0, t0));
        r_points.push_back(blossom(t0, t0, t1));
        r_points.push_back(blossom(t0, t1, t1));
        r_points.push_back(blossom(t1, t1, t1));
        return r_points;
    }

    std::vector<double> cubic_segment::d_roots() const {
        // A = 3*d - 9*c + 9*b - 3*a;
        // B = 6*c - 12*b + 6*a;
        // C = 3*b - 3*a;
        R2 A = 3.0*m_points[3] - 9.0*m_points[2] + 9.0*m_points[1] - 3.0*m_points[0];
        R2 B = 6.0*m_points[2] - 12.0*m_points[1] + 6.0*m_points[0];
        R2 C = 3.0*m_points[1] - 3.0*m_points[0];
        std::vector<double> x_roots = roots(A.get_x(), B.get_x(), C.get_x());
        std::vector<double> y_roots = roots(A.get_y(), B.get_y(), C.get_y());
        return organized_roots(x_roots, y_roots);
    }

} // end raw

path_builder::path_builder(const xform& xf) 
    : m_xf(xf) 
    , m_last_move(make_R2(0, 0))
{}

path_builder::~path_builder() {
    m_path.clear();
}

void path_builder::do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1) {
    raw::linear_segment lin(make_R2(x0, y0), make_R2(x1, y1));
    lin.apply(m_xf);
    std::vector<R2> control_points = lin.reparametrize(0, 1);
    m_path.push_back(new monotonic::linear_segment(control_points));
}

void path_builder::do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2) {
    raw::quadratic_segment quad(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2));
    quad.apply(m_xf);
    std::vector<double> roots = quad.d_roots();
    for(unsigned int i = 0; i < roots.size()-1; i++) {
        int j = (i+1)&roots.size();
        m_path.push_back(new monotonic::quadratic_segment(quad.reparametrize[roots[i], roots[j]]));
    }
}

void path_builder::do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2) {
    raw::rational_quadratic_segment rat(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), w1);
    rat.apply(m_xf);
    std::vector<double> roots = rat.d_roots();
    for(unsigned int i = 0; i < roots.size()-1; i++) {
        int j = (i+1)&roots.size();
        m_path.push_back(new monotonic::rational_quadratic_segment(rat.reparametrize[roots[i], roots[j]],
                                                                   rat.reparametrize_den[roots[i], roots[j]]));
    }
}

void path_builder::do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3) {
    raw::cubic_segment cubic(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), make_R2(x3, y3));
    cubic.apply(m_xf);
    std::vector<double> roots = cubic.d_roots();
    for(unsigned int i = 0; i < roots.size()-1; i++) {
        int j = (i+1)&roots.size();
        m_path.push_back(new monotonic::cubic_segment(cubic.reparametrize[roots[i], roots[j]]));
    }
}

void path_builder::do_begin_contour(rvgf x0, rvgf y0) {
    m_last_move = make_R2(x0, y0);
}

void path_builder::do_end_open_contour(rvgf x0, rvgf y0) {
    do_linear_segment(x0, y0, m_last_move.get_x(), m_last_move.get_y());
}

void path_builder::do_end_closed_contour(rvgf x0, rvgf y0) {
    (void) x0;
    (void) y0;
}

}}}

// criar raw
// aplicar xf
// achar raizes
// reparametrizar
// conseguir paths monotonos
