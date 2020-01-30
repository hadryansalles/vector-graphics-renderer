#ifndef RVG_DRIVER_PNG_H
#define RVG_DRIVER_PNG_H

#include <iosfwd>
#include <string>
#include <vector>


#include "rvg-image.h"
#include "rvg-viewport.h"
#include "rvg-window.h"
#include "rvg-scene.h"

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
    linear_segment(std::vector<R2> points); 
    double x_in_t(const double t) const;
    double y_in_t(const double t) const;
};

class quadratic_segment : public path_segment {
public:
    quadratic_segment(std::vector<R2> &points);  
    virtual double x_in_t(const double t) const;
    virtual double y_in_t(const double t) const;
};

class rational_quadratic_segment : public quadratic_segment {
private: 
    quadratic_segment den;
public:
    rational_quadratic_segment(std::vector<R2> &points, std::vector<R2> &den_points); 
    double x_in_t(const double t) const;
    double y_in_t(const double t) const;
};

class cubic_segment : public path_segment {
    cubic_segment(std::vector<R2> &points);
    double x_in_t(const double t) const;
    double y_in_t(const double t) const;
};

class accelerated;

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v);

void render(const accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args =
        std::vector<std::string>());

} } } // namespace rvg::driver::png

#endif
