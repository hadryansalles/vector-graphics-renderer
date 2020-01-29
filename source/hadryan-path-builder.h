#include <vector>
#include <assert.h>

#include "rvg-i-input-path.h"
#include "rvg-xform.h"

namespace rvg {
    namespace driver {
        namespace png {

class bouding_box {
    private:
        R2 m_p0;
        R2 m_p1;
    public:
        bouding_box(std::vector<R2> &points);
        bool hit_left(const double x, const double y) const;
        bool hit_right(const double x, const double y) const;
        bool hit_inside(const double x, const double y) const;
};

namespace monotonic {

    class path_segment {
    protected:
        std::vector<R2> m_points; // control points
        const bouding_box m_bbox; // segment bouding box
        int m_dir;
    public:
        path_segment(std::vector<R2> &points);
        virtual ~path_segment();

        R2 p_in_t(const double t) const;
        virtual double x_in_t(const double t) const = 0;
        virtual double y_in_t(const double t) const = 0;
        bool intersect(const double x, const double y) const;
        int get_dir() const;
    };

    class linear_segment : public path_segment {
    public:
        linear_segment(std::vector<R2> &points); 
        double x_in_t(const double t) const;
        double y_in_t(const double t) const;
    };

    class quadratic_segment : public path_segment {
    public:
        quadratic_segment(std::vector<R2> &points);  // POSSIBLE BUG
        virtual double x_in_t(const double t) const;
        virtual double y_in_t(const double t) const;
    };

    class rational_quadratic_segment : public quadratic_segment {
    private: 
        quadratic_segment den;
    public:
        rational_quadratic_segment(std::vector<R2> &points, std::vector<R2> &den_points); // ASSERT size = 2
        double x_in_t(const double t) const;
        double y_in_t(const double t) const;
    };

    class cubic_segment : public path_segment {
        cubic_segment(std::vector<R2> &points);
        double x_in_t(const double t) const;
        double y_in_t(const double t) const;
    };

} // end monotonic

namespace raw {

    class path_segment {
    protected:
        std::vector<R2> m_points;
        double m_w;

        R2 blossom(double a, double b) const;
        R2 blossom(double a, double b, double c) const;

        static std::vector<double> roots(double A, double B);
        static std::vector<double> roots(double A, double B, double C);
        static std::vector<double> organized_roots(std::vector<double> &x_roots, std::vector<double> &y_roots);
    
    public:
        path_segment();
        virtual ~path_segment();

        void apply(const xform& xf);
        virtual std::vector<R2> reparametrize(double t0, double t1) const = 0;
        virtual std::vector<double> d_roots() const = 0;
    };

    class linear_segment : public path_segment {
    public:
        linear_segment(R2 p1, R2 p2);
        std::vector<R2> reparametrize(double t0, double t1) const;
        std::vector<double> d_roots() const;
    };

    class quadratic_segment : public path_segment {
    public:
        quadratic_segment(R2 p1, R2 p2, R2 p3);
        std::vector<R2> reparametrize(double t0, double t1) const;
        std::vector<double> d_roots() const;
    };

    class rational_quadratic_segment : public quadratic_segment {
    private:
        quadratic_segment den;
    public:
        rational_quadratic_segment(R2 p1, R2 p2, R2 p3, double w);
        std::vector<R2> reparametrize(double t0, double t1) const;
        std::vector<R2> reparametrize_den(double t0, double t1) const;
        std::vector<double> d_roots() const;
    };

    class cubic_segment : public path_segment {
    public:
        cubic_segment(R2 p1, R2 p2, R2 p3, R2 p4);
        std::vector<R2> reparametrize(double t0, double t1) const;
        std::vector<double> d_roots() const;
    };

} // end raw

//  Iterates through path_data getting its parameters and creates a representation 
//  with only monotonic segments
class path_builder final : public i_input_path<path_builder>{
    private:
        friend i_input_path<path_builder>;
        std::vector<monotonic::path_segment*> m_path;
        const xform m_xf;
        R2 m_last_move;
        
    public:
        path_builder(const xform& xf);
        ~path_builder();

        std::vector<monotonic::path_segment*> get_path() const;

        void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1);
        void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2);
        void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2);
        void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3);
        void do_begin_contour(rvgf x0, rvgf y0);
        void do_end_open_contour(rvgf x0, rvgf y0);
        void do_end_closed_contour(rvgf x0, rvgf y0);
};

}}}
