
#include <string>
#include <sstream>
#include <cmath>
#include <memory>
#include <assert.h>

#include <lua.h>
#include "rvg-lua.h"
#include "rvg-pngio.h"
#include "rvg-image.h"
#include "rvg-shape.h"
#include "rvg-paint.h"
#include "rvg-rgba.h"
#include "rvg-winding-rule.h"
#include "rvg-xform-affinity.h"
#include "rvg-i-input-path.h"
#include "rvg-i-scene-data.h"
#include "rvg-stroke-style.h"

#include "rvg-input-path-f-xform.h"
#include "rvg-rgba.h"

#include "rvg-lua-facade.h"

#include "rvg-driver-png.h"

#define EPS 0.000000001

namespace rvg {
    namespace driver {
        namespace png {

R2 transform_point(const R2 point, const xform& xf){
    return make_R2(xf[0][0]*point.get_x() + xf[0][1]*point.get_y() + xf[0][2],
                   xf[1][0]*point.get_x() + xf[1][1]*point.get_y() + xf[1][2]);
}

R2 transform_point(const R2 point, double w, const xform& xf){
    return make_R2(xf[0][0]*point.get_x() + xf[0][1]*point.get_y() + w*xf[0][2],
                   xf[1][0]*point.get_x() + xf[1][1]*point.get_y() + w*xf[1][2]);
}

float det(const xform& xf){
    return xf[0][0]*xf[1][1] - xf[0][1]*xf[1][0];
} 

int sign(float x){
    if(x > 0){
        return 1;
    }
    if(x < 0){
        return -1;
    }
    else{
        return 0;
    }
}

class path_segment;

class bouding_box {
private:
    R2 p0;
    R2 p1;

public:
    inline bouding_box()
    {};
    inline ~bouding_box()
    {};
    inline void set(R2 p0, R2 p1){
        double x_min = std::min(p0.get_x(), p1.get_x());
        double y_min = std::min(p0.get_y(), p1.get_y());
        double x_max = std::max(p0.get_x(), p1.get_x());
        double y_max = std::max(p0.get_y(), p1.get_y());
        this->p0 = make_R2(x_min, y_min);
        this->p1 = make_R2(x_max, y_max);
    };
    inline void set(std::vector<R2> points){
        assert(points.size() > 0);
        float x_min = points[0].get_x();
        float y_min = points[0].get_y();
        float x_max = points[0].get_x();
        float y_max = points[0].get_y();
        for(auto point : points){
            x_min = std::min(point.get_x(), x_min);
            y_min = std::min(point.get_y(), y_min);
            x_max = std::max(point.get_x(), x_max);
            y_max = std::max(point.get_y(), y_max);
        }   
        p0 = make_R2(x_min, y_min);
        p1 = make_R2(x_max, y_max);
    };
    
    void set(std::vector<path_segment*> segments);

    inline bool hit(const double x, const double y) const {
        return y >= p0.get_y() && y < p1.get_y() && x >= p0.get_x() && x < p1.get_x();
    };
    inline bool hit_in(const double x, const double y) const {
        return y >= p0.get_y() && y < p1.get_y() && x <= p0.get_x(); 
    };
    inline bool hit_out(const double x, const double y) const {
        return (p1.get_x() < x || p0.get_y() > y || p1.get_y() <= y);
    };
};

class path_segment {
protected:
    std::vector<R2> points;
    bouding_box bbox;
public:
    inline path_segment(){};
    inline virtual ~path_segment(){points.clear();};
    inline virtual void apply_xf(const xform& xf) {
        // printf("transformou segmento.\n");
        for(unsigned int i = 0; i < points.size(); i++){
            points[i] = transform_point(points[i], xf);
        }
        bbox.set(points);
    };
    virtual double x_in_t(const double t) const = 0;
    virtual double y_in_t(const double t) const = 0;
    virtual bool specific_hit(const double x, const double y) const {
        assert(points.size() > 1);
        int a = 1;
        if(points[points.size()-1].get_y() > points[0].get_y()){
            a = -1;
        }
        double _y;
        double d = 0.5;
        double t = 0.5;
        do {
            d /= 2.0;
            _y = y_in_t(t); 
            if(_y > y){
                t += (double)a*d;
            }
            else if(_y < y){
                t -= (double)a*d;
            }
        } while(d > 0.001);
        return x-x_in_t(t) < EPS;
    };
    inline virtual int hit(const double x, const double y) const {
        assert(points.size() > 1);
        if(!bbox.hit_out(x, y)){
            R2 pi = points[0];
            R2 pf = points[points.size()-1];
            int a = 1;
            if(std::min(pi.get_y(), pf.get_y()) == pf.get_y()){
                a = -1;
            }
            if(bbox.hit_in(x, y) || specific_hit(x,y)){
                return a;
            }
        }
        return 0;
    };
    inline void print() const {
        for(unsigned int i = 0; i < points.size(); i++){
            printf("(%.3f, %.3f) ", points[i].get_x(), points[i].get_y());
        }
        printf("\n");
    };
    inline R2 get_point(unsigned int indice){
        assert(indice < points.size());
        return points[indice];
    }
    inline R2 get_max() const {
        assert(points.size() > 0);
        return points[points.size()-1];
    }
    inline R2 get_min() const {
        assert(points.size() > 0);
        return points[0];
    }
};


void bouding_box::set(std::vector<path_segment*> segments){
    assert(segments.size() > 0);
    float x_min = segments[0]->get_min().get_x();
    float y_min = segments[0]->get_min().get_y();
    float x_max = segments[0]->get_max().get_x();
    float y_max = segments[0]->get_max().get_y();
    for(unsigned int i = 0; i < segments.size(); i++){
        x_min = std::min(segments[i]->get_min().get_x(), x_min);
        y_min = std::min(segments[i]->get_min().get_y(), y_min);
        x_max = std::max(segments[i]->get_max().get_x(), x_max);
        y_max = std::max(segments[i]->get_max().get_y(), y_max);
    }
    p0 = make_R2(x_min, y_min);
    p1 = make_R2(x_max, y_max);
}


class linear_p_segment : public path_segment {
public:
    inline linear_p_segment() : path_segment() 
    {};
    inline linear_p_segment(const R2 p1, const R2 p2) : path_segment() {
        points.push_back(p1);
        points.push_back(p2);
        bbox.set(points);
    };
    inline void set(const R2 p1, const R2 p2){
        points.clear();
        points.push_back(p1);
        points.push_back(p2);
        bbox.set(points);
    };
    inline bool specific_hit(const double x, const double y) const{
        assert(points.size() > 1);
        R2 p1 = points[0];
        R2 p2 = points[points.size()-1];
        if(std::min(p1.get_y(), p2.get_y()) <= y && std::max(p1.get_y(), p2.get_y()) > y){
            float dy = p2.get_y() - p1.get_y();
            float dx = p2.get_x() - p1.get_x();
            return dy*((x - p1.get_x())*dy - (y - p1.get_y())*dx) <= 0;
        }
        return false;
    };
    virtual double y_in_t(const double t) const{
        (void) t;
        return 0;
    }
    virtual double x_in_t(const double t) const{
        (void) t;
        return 0;
    }
};

class quadratic_p_segment : public path_segment {
public:
    inline quadratic_p_segment(){
    };
    inline quadratic_p_segment(const R2 p1, const R2 p2, const R2 p3){
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        bbox.set(points);
    };
    inline void set(const R2 p1, const R2 p2, const R2 p3){
        points.clear();
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        bbox.set(points);
    };
    inline static double derivative_root(double a, double b, double c){
        double d = a - 2*b + c;
        if(!(std::abs(d) > EPS)){
            return 0;
        }
        double n = (a-b)/d;
        if(n < EPS || (n-1) > EPS) {
            return 0;
        }
        else{
            return n;
        }
    };
    inline static double val_in_quad_bezie(double t, double p0, double p1, double p2){
        return p0*(1-t)*(1-t) + p1*2*(t - t*t) + p2*t*t;
    };
    inline double x_in_t(double t)const{
        assert(points.size() > 1);
        return points[0].get_x()*(1-t)*(1-t) + points[1].get_x()*2*(t - t*t) + points[2].get_x()*t*t;
    };
    inline double y_in_t(double t)const{
        assert(points.size() > 1);
        return points[0].get_y()*(1-t)*(1-t) + points[1].get_y()*2*(t - t*t) + points[2].get_y()*t*t;
    };
    inline R2 r2_in_t(double t)const{
        return make_R2(x_in_t(t), y_in_t(t));
    };
    inline R2 blossom(double s, double r)const{
        assert(points.size() > 1);
        double x = (1-s)*((1-r)*points[0].get_x() + r*points[1].get_x()) + s*((1-r)*points[1].get_x() + r*points[2].get_x());
        double y = (1-s)*((1-r)*points[0].get_y() + r*points[1].get_y()) + s*((1-r)*points[1].get_y() + r*points[2].get_y());
        return make_R2(x, y);
    };
    inline quadratic_p_segment* reparametrize(double t0, double t1)const{
        R2 pP1 = r2_in_t(t0);
        R2 pP2 = blossom(t0, t1);
        R2 pP3 = r2_in_t(t1);
        return new quadratic_p_segment(pP1, pP2, pP3);
    };
    inline bool is_linear() const {
        assert(points.size() == 3);
        R2 p0 = points[0];
        R2 p1 = points[1];
        R2 p2 = points[2];
        return std::abs((p1.get_x()-p0.get_x())*(p2.get_y()-p0.get_y())-(p2.get_x()-p0.get_x())*(p1.get_y()-p0.get_y())) < EPS;
    };
};

class cubic_p_segment : public path_segment {
public:
    inline cubic_p_segment(){
    };
    inline cubic_p_segment(const R2 p1, const R2 p2, const R2 p3, const R2 p4){
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        points.push_back(p4);
        bbox.set(points);
    };
    inline static std::tuple<int, double, double> derivative_root(double a, double b, double c, double d){
        // printf("achando raizes para: %.2f, %.2f, %.2f, %.2f\n", a, b, c, d);
        int roots_number = 0;
        double root1 = 0, root2 = 0;
        double A = 3*d - 9*c + 9*b - 3*a;
        double B = 6*c - 12*b + 6*a;
        double C = 3*b - 3*a;
        double delt = B*B - 4*A*C;
        // printf("pegou raiz com delt %.2f e A:%.2f B:%.2f C:%.2f.\n", delt, A, B, C);
        if(std::abs(A) > EPS){
            if(delt > 0){
                roots_number = 2;
                double sqrt = std::sqrt(delt);
                root1 = (-B+sqrt)/(2*A);
                root2 = (-B-sqrt)/(2*A);
            }
            else if(delt == 0){
                roots_number = 1;
                root1 = -B/(2*A);
                root2 = root1;
            }
            else{
                roots_number = 0;
            }
        }
        else{
            roots_number = 1;
            root1 = -C/B;
            root2 = root1;
        }
        return std::make_tuple(roots_number, std::min(root1, root2), std::max(root1, root2));
    };
    inline double x_in_t(double t) const {
        assert(points.size() > 3);
        return points[0].get_x()*(1-t)*(1-t)*(1-t) + 
               points[1].get_x()*3.0*(1-t)*(1-t)*t + 
               points[2].get_x()*3.0*t*t*(1-t) + 
               points[3].get_x()*t*t*t;
    };
    inline double y_in_t(double t) const {
        assert(points.size() > 3);
        return points[0].get_y()*(1-t)*(1-t)*(1-t) + 
               points[1].get_y()*3.0*(1-t)*(1-t)*t + 
               points[2].get_y()*3.0*t*t*(1-t) + 
               points[3].get_y()*t*t*t;
    };
    inline R2 r2_in_t(double t) const {
        return make_R2(x_in_t(t), y_in_t(t));
    };
    inline R2 blossom(double a, double b, double c) const {
        assert(points.size()>3);
        double x = (((1-a)*points[0].get_x() +
                       (a)*points[1].get_x())*(1-b) +
                    ((1-a)*points[1].get_x() +
                       (a)*points[2].get_x())*(b)) * (1-c) + 
                   (((1-a)*points[1].get_x() +
                       (a)*points[2].get_x())*(1-b) +
                    ((1-a)*points[2].get_x() +
                       (a)*points[3].get_x())*(b)) * (c);
        double y = (((1-a)*points[0].get_y() +
                       (a)*points[1].get_y())*(1-b) +
                    ((1-a)*points[1].get_y() +
                       (a)*points[2].get_y())*(b)) * (1-c) + 
                   (((1-a)*points[1].get_y() +
                       (a)*points[2].get_y())*(1-b) +
                    ((1-a)*points[2].get_y() +
                       (a)*points[3].get_y())*(b)) * (c);
        return make_R2(x, y);
    };
    inline cubic_p_segment* reparametrize(double t0, double t1) const {
        R2 pP1 = r2_in_t(t0);
        R2 pP2 = blossom(t0, t0, t1);
        R2 pP3 = blossom(t0, t1, t1);
        R2 pP4 = r2_in_t(t1);  
        // printf("reparametrizou para (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f).\n",
        // pP1.get_x(), pP1.get_y(), pP2.get_x(), pP2.get_y(), pP3.get_x(), pP3.get_y(), pP4.get_x(), pP4.get_y());
        return new cubic_p_segment(pP1, pP2, pP3, pP4);
    };
    inline bool is_linear() const {
        assert(points.size() == 4);
        R2 p0 = points[0];
        R2 p1 = points[1];
        R2 p2 = points[2];
        R2 p3 = points[3];
        return std::abs((p1.get_x()-p0.get_x())*(p2.get_y()-p0.get_y())-(p2.get_x()-p0.get_x())*(p1.get_y()-p0.get_y())) < EPS
            && std::abs((p1.get_x()-p0.get_x())*(p3.get_y()-p0.get_y())-(p3.get_x()-p0.get_x())*(p1.get_y()-p0.get_y())) < EPS
            && std::abs((p1.get_x()-p2.get_x())*(p3.get_y()-p2.get_y())-(p3.get_x()-p2.get_x())*(p1.get_y()-p2.get_y())) < EPS;
    }
};

class rational_p_segment : public path_segment {
private:
    double m_w;
public:
    inline rational_p_segment(){
    };
    inline rational_p_segment(const R2 p1, const R2 p2, const R2 p3, double w){
        // printf("criando racional (%.2f, %.2f) (%.2f, %.2f) (%.2f, %.2f) %.2f.\n",
            // p1.get_x(), p1.get_y(), p2.get_x(), p2.get_y(), p3.get_x(), p3.get_y(), w);
        points.push_back(p1);
        points.push_back(p2);
        points.push_back(p3);
        bbox.set(points);
        m_w = w;
    };
    inline static std::tuple<int, double, double> derivative_root(double a, double b, double c, double w){
        // printf("achando raizes racionais para: %.2f, %.2f, %.2f, %.2f\n", a, b, c, w);
        int roots_number = 0;
        double root1 = 0, root2 = 0;
        double A = -2*a*w + 2*a + 2*w*c - 2*c;
        double B = 4*a*w - 2*a - 4*b + 2*c;
        double C = -2*a*w + 2*b;
        double delt = B*B - 4*A*C;
        // printf("pegou raiz com delt %.2f e A:%.2f B:%.2f C:%.2f.\n", delt, A, B, C);
        if(std::abs(A) > EPS){
            if(delt > 0){
                roots_number = 2;
                double sqrt = std::sqrt(delt);
                root1 = (-B+sqrt)/(2*A);
                root2 = (-B-sqrt)/(2*A);
            }
            else if(delt == 0){
                roots_number = 1;
                root1 = -B/(2*A);
                root2 = root1;
            }
            else{
                roots_number = 0;
            }
        }
        else{
            roots_number = 1;
            root1 = -C/B;
            root2 = root1;
        }
        return std::make_tuple(roots_number, std::min(root1, root2), std::max(root1, root2));
    };
    inline double x_in_t(double t) const {
        quadratic_p_segment a(points[0], points[1], points[2]);
        quadratic_p_segment b(make_R2(1,1), make_R2(m_w, m_w), make_R2(1, 1));
        return a.x_in_t(t)/b.x_in_t(t);
    };
    inline double y_in_t(double t) const {
    quadratic_p_segment a(points[0], points[1], points[2]);
        quadratic_p_segment b(make_R2(1,1), make_R2(m_w, m_w), make_R2(1, 1));
        return a.y_in_t(t)/b.y_in_t(t);
    };
    inline R2 r2_in_t(double t) const {
        return make_R2(x_in_t(t), y_in_t(t));
    };
    inline rational_p_segment* reparametrize(double t0, double t1) const {
        assert(points.size() == 3);
        quadratic_p_segment* temp = new quadratic_p_segment(points[0], points[1], points[2]);
        quadratic_p_segment* temp_repar = temp->reparametrize(t0, t1);
        double w0 = quadratic_p_segment::val_in_quad_bezie(t0, 1.0, m_w, 1.0);
        
        double w1 = (1.0-t0)*((1.0-t1) + t1*m_w) + t0*((1.0-t1)*m_w + t1);

        double w2 = quadratic_p_segment::val_in_quad_bezie(t1, 1.0, m_w, 1.0);
        // printf("reparametrizou com w0:%.2f, w1:%.2f, w2:%.2f.\n", w0, w1, w2);
        R2 rp1 = temp_repar->get_point(0);
        R2 rp2 = temp_repar->get_point(1);
        R2 rp3 = temp_repar->get_point(2);
        R2 p1 = make_R2(rp1.get_x()/w0, rp1.get_y()/w0);
        R2 p2 = make_R2(rp2.get_x()/sqrt(w0*w2), rp2.get_y()/sqrt(w0*w2));
        R2 p3 = make_R2(rp3.get_x()/w2, rp3.get_y()/w2);
        auto retu = new rational_p_segment(p1, p2, p3, w1/sqrt(w0*w2));
        delete temp;
        delete temp_repar;
        return retu;
    };
    inline bool is_linear() const {
        assert(points.size() == 3);
        R2 p0 = points[0];
        R2 p1 = points[1];
        R2 p2 = points[2];
        return std::abs((p1.get_x()-p0.get_x())*(p2.get_y()-p0.get_y())-(p2.get_x()-p0.get_x())*(p1.get_y()-p0.get_y())) < EPS
            && m_w == 1;
    }
    inline void apply_xf(const xform& xf){
        // printf("transformou racional.\n");
        for(unsigned int i = 0; i < points.size(); i++){
            points[i] = transform_point(points[i], xf);
        }
        bbox.set(points);
    }
};

class scene_object {
private:
    RGBA8 color;
    e_winding_rule w_rule;
    std::vector<path_segment*> segments;
    bouding_box bbox;
public:
    inline scene_object(std::vector<path_segment*> path, const paint &c, e_winding_rule w_r)
    :   w_rule(w_r) {
        segments = path;
        if(c.is_solid_color()){
            color = c.get_solid_color();
        }
        else{
            printf("INVALID PAINT TYPE");
            color = RGBA8(0, 0, 0, 0);
        }
    };
    inline ~scene_object()
    {};
    inline const RGBA8 get_color() const{
        return color;    
    };
    inline bool hit(const double x, const double y) const{
        if(bbox.hit(x, y)){
            int sum = 0;
            for(unsigned int i = 0; i < segments.size(); i++){
                sum += segments[i]->hit(x, y);
            }
            if(w_rule == e_winding_rule::non_zero){
                return (sum != 0);
            }
            else if(w_rule == e_winding_rule::odd){
                return ((sum % 2)!= 0);
            }
        }
        return false;
    };
    inline void print_info() const{
        for(auto seg : segments){
            seg->print();
        }
    };
    inline void update_bbox(){
        bbox.set(segments);
    }
};

class accelerated {
public:
    std::vector<scene_object*> objects;
    inline accelerated()
    {};
    inline ~accelerated()
    {};
    inline void add_object(std::vector<path_segment*> path, const paint &paint, e_winding_rule w_rule){
        scene_object* obj = new scene_object(path, paint, w_rule);
        objects.push_back(obj);
    };
    inline void destroy_objects(){
        scene_object* obj;
        for(unsigned int i = 0; i < objects.size(); i++){
            obj = objects[i];
            objects[i] = NULL;
            delete obj;    
        }
    };
    inline void update_bbox(){
        for(unsigned int i = 0; i < objects.size(); i++){
            objects[i]->update_bbox();
        }
    }
    inline void print_objects() const{
        for (auto obj : objects){
            obj->print_info();
        }   
    };
};

class path_builder final: public i_input_path<path_builder>{
private:
    friend i_input_path<path_builder>;
    const xform& xf;
    std::vector<path_segment*> &path;
    R2 last_move; 
    inline void do_linear_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1){
        // printf("criando segmento linear.\n");
        linear_p_segment* seg = new linear_p_segment(make_R2(x0, y0), make_R2(x1, y1));
        seg->apply_xf(xf);
        path.push_back(seg);
    };
    inline void do_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1,rvgf x2, rvgf y2){
        auto* complete = new quadratic_p_segment(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2));
        if(!complete->is_linear()){
            complete->apply_xf(xf);
            quadratic_p_segment* segment = NULL;
            double tx = quadratic_p_segment::derivative_root(x0, x1, x2);
            double ty = quadratic_p_segment::derivative_root(y0, y1, y2);
            double t0 = std::min(tx, ty);
            double t1 = std::max(tx, ty);
            if(t0 > EPS){
                segment = complete->reparametrize(0, t0);
                path.push_back(segment);
            }
            if(t1 > EPS){
                if(std::abs(t0-t1) > EPS){
                    segment = complete->reparametrize(t0, t1);
                    path.push_back(segment);
                }
                segment = complete->reparametrize(t1, 1);
                path.push_back(segment);
            }
            else{
                path.push_back(complete);
            }
        }
        else{
            // printf("fez linear a partir da quadratica: (%.2f, %.2f) (%.2f, %.2f).\n", x0, y0, x2, y2);
            do_linear_segment(x0, y0, x2, y2);
        }
    }
    inline void do_rational_quadratic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf w1, rvgf x2, rvgf y2){
        // printf("começou racional (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f), (%.2f).\n",
                                // x0, y0, x1, y1, x2, y2, w1);
        auto* complete = new rational_p_segment(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), w1);
        if(!complete->is_linear()) {
            complete->apply_xf(xf);
            // printf("x:");
            auto roots_x = rational_p_segment::derivative_root(x0, x1, x2, w1);
            // printf("y:"); 
            auto roots_y = rational_p_segment::derivative_root(y0, y1, y2, w1);
            std::vector<double> roots_t;
            switch(std::get<0>(roots_x)){
                case 1:
                    // printf("raiz x: %.2f.\n", std::get<1>(roots_x));
                    roots_t.push_back(std::get<1>(roots_x));
                    break;
                case 2:
                    // printf("raizes x: %.2f, %.2f.\n", std::get<1>(roots_x), std::get<2>(roots_x));
                    roots_t.push_back(std::get<1>(roots_x));
                    roots_t.push_back(std::get<2>(roots_x));
                    break;
                default:
                    break;
            }
            switch(std::get<0>(roots_y)){
                case 1:
                    // printf("raiz y: %.2f.\n", std::get<1>(roots_y));
                    roots_t.push_back(std::get<1>(roots_y));
                    break;
                case 2:
                    // printf("raizes y: %.2f, %.2f.\n", std::get<1>(roots_y), std::get<2>(roots_y));
                    roots_t.push_back(std::get<1>(roots_y));
                    roots_t.push_back(std::get<2>(roots_y));
                    break;
                default:
                    break;
            }
            // printf("pontos: ");
            for(auto it = roots_t.begin(); it != roots_t.end(); ++it){
                // printf("%.2f\t", (*it));
            }
            // printf("\n");
            for(int i = roots_t.size()-1; i >= 0; i--){
                if(roots_t[i] >= 1.0f || roots_t[i] <= 0.0f){
                    roots_t.erase(roots_t.begin() + i);
                }
            }
            roots_t.push_back(0.f);
            roots_t.push_back(1.f);
            roots_t.erase(std::unique(roots_t.begin(), roots_t.end()), roots_t.end());
            std::sort(roots_t.begin(), roots_t.end());
            // printf("pontos filtrados: ");
            for(auto it = roots_t.begin(); it != roots_t.end(); ++it){
                // printf("%.2f\t", (*it));
            }
            // printf("\n");
            for(unsigned int i = 0; i < roots_t.size()-1; i++){
                int j = (i+1)%(roots_t.size());
                // printf("criou segmento %.2f %.2f.\n", roots_t[i], roots_t[j]);
                auto repar = complete->reparametrize(roots_t[i], roots_t[j]);
                path.push_back(repar);
            }
            delete complete;
        }
        else{
            // printf("fez linear a partir da racional: (%.2f, %.2f) (%.2f, %.2f).\n", x0, y0, x2, y2);
            do_linear_segment(x0, y0, x2, y2);
        }
    };
    inline void do_cubic_segment(rvgf x0, rvgf y0, rvgf x1, rvgf y1, rvgf x2, rvgf y2, rvgf x3, rvgf y3){
        // printf("começou cubica (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f).\n",
                                //x0, y0, x1, y1, x2, y2, x3, y3);
        auto* complete = new cubic_p_segment(make_R2(x0, y0), make_R2(x1, y1), make_R2(x2, y2), make_R2(x3, y3));
        if(!complete->is_linear()) {
            complete->apply_xf(xf);
            // printf("x:");
            auto roots_x = cubic_p_segment::derivative_root(x0, x1, x2, x3);
            // printf("y:"); 
            auto roots_y = cubic_p_segment::derivative_root(y0, y1, y2, y3);
            std::vector<double> roots_t;
            switch(std::get<0>(roots_x)){
                case 1:
                    // printf("raiz x: %.2f.\n", std::get<1>(roots_x));
                    roots_t.push_back(std::get<1>(roots_x));
                    break;
                case 2:
                    // printf("raizes x: %.2f, %.2f.\n", std::get<1>(roots_x), std::get<2>(roots_x));
                    roots_t.push_back(std::get<1>(roots_x));
                    roots_t.push_back(std::get<2>(roots_x));
                    break;
                default:
                    break;
            }
            switch(std::get<0>(roots_y)){
                case 1:
                    // printf("raiz y: %.2f.\n", std::get<1>(roots_y));
                    roots_t.push_back(std::get<1>(roots_y));
                    break;
                case 2:
                    // printf("raizes y: %.2f, %.2f.\n", std::get<1>(roots_y), std::get<2>(roots_y));
                    roots_t.push_back(std::get<1>(roots_y));
                    roots_t.push_back(std::get<2>(roots_y));
                    break;
                default:
                    break;
            }
            // printf("pontos: ");
            for(auto it = roots_t.begin(); it != roots_t.end(); ++it){
                // printf("%.2f\t", (*it));
            }
            // printf("\n");
            for(int i = roots_t.size()-1; i >= 0; i--){
                if(roots_t[i] >= 1.0f || roots_t[i] <= 0.0f){
                    roots_t.erase(roots_t.begin() + i);
                }
            }
            roots_t.push_back(0.f);
            roots_t.push_back(1.f);
            roots_t.erase(std::unique(roots_t.begin(), roots_t.end()), roots_t.end());
            std::sort(roots_t.begin(), roots_t.end());
            // printf("pontos filtrados: ");
            for(auto it = roots_t.begin(); it != roots_t.end(); ++it){
                // printf("%.2f\t", (*it));
            }
            // printf("\n");
            for(unsigned int i = 0; i < roots_t.size()-1; i++){
                int j = (i+1)%(roots_t.size());
                // printf("criou segmento %.2f %.2f.\n", roots_t[i], roots_t[j]);
                path.push_back(complete->reparametrize(roots_t[i], roots_t[j]));
            }
        }
        else{
            // printf("fez linear a partir da cubica: (%.2f, %.2f) (%.2f, %.2f).\n", x0, y0, x3, y3);
            do_linear_segment(x0, y0, x3, y3);
        }
    };
    inline void do_begin_contour(rvgf x0, rvgf y0){
        last_move = make_R2(x0, y0);
    };
    inline void do_end_open_contour(rvgf x0, rvgf y0){
        do_linear_segment(x0, y0, last_move.get_x(), last_move.get_y());
    };
    inline void do_end_closed_contour(rvgf x0, rvgf y0){
        (void) x0;
        (void) y0;
    };
public:
    inline path_builder(const xform& xf_, std::vector<path_segment*> &path_) 
        : xf(xf_)
        , path(path_)
        , last_move(make_R2(0,0)) {
        std::cout<<"";
    };
    inline ~path_builder()
    {};
    inline std::vector<path_segment*> get_path() const {
        return path;
    };
};

class acc_builder final: public i_scene_data<acc_builder> {
private:
    accelerated acc;
    std::vector<xform> m_xf_stack;
    friend i_scene_data<acc_builder>;
    
    inline void push_xf(const xform &xf){
        m_xf_stack.push_back(top_xf() * xf);
    };
    inline void pop_xf(){
        if (m_xf_stack.size() > 0) {
            m_xf_stack.pop_back();
        }
    };
    inline const xform &top_xf() const{
        static xform id;
        if (m_xf_stack.empty()) return id;
        else return m_xf_stack.back();
    };
    inline void do_painted_shape(e_winding_rule wr, const shape &s, const paint &p){
        std::vector<path_segment*> path;
        xform post;
        path_data::const_ptr path_data;
        if(s.is_path()){
            path_data = s.get_path_data_ptr();
        }
        else if(s.is_triangle()){
            path_data = s.get_triangle_data().as_path_data_ptr(post);
        }
        else if(s.is_polygon()){
            path_data = s.get_polygon_data().as_path_data_ptr(post);
        }
        else if(s.is_circle()){
            path_data = s.get_circle_data().as_path_data_ptr(post);
        }
        else if(s.is_stroke()){
            path_data = s.as_path_data_ptr();
        }
        path_builder _path_builder(post*top_xf()*s.get_xf(), path);
        path_data->iterate(_path_builder);
        acc.add_object(path, p, wr);
    };
    inline void do_begin_transform(uint16_t depth, const xform &xf){
        (void) depth;
        push_xf(xf);
    };
    inline void do_end_transform(uint16_t depth, const xform &xf){
        (void) depth;
        (void) xf;
        pop_xf(); 
    };
    inline void do_tensor_product_patch(const patch<16,4> &tpp){(void) tpp;};
    inline void do_coons_patch(const patch<12,4> &cp){(void) cp;};
    inline void do_gouraud_triangle(const patch<3,3> &gt){(void) gt;};
    inline void do_stencil_shape(e_winding_rule wr, const shape &s){(void) wr;(void) s;};
    inline void do_begin_clip(uint16_t depth){(void) depth;};
    inline void do_activate_clip(uint16_t depth){(void) depth;};
    inline void do_end_clip(uint16_t depth){(void) depth;};
    inline void do_begin_fade(uint16_t depth, unorm8 opacity){(void) depth;(void) opacity;};
    inline void do_end_fade(uint16_t depth, unorm8 opacity){(void) depth;(void) opacity;};
    inline void do_begin_blur(uint16_t depth, float radius){(void) depth;(void) radius;};
    inline void do_end_blur(uint16_t depth, float radius){(void) depth;(void) radius;};

public:
    inline acc_builder(const xform &screen_xf){
        push_xf(screen_xf);
    };
    inline void update_bbox(){
        acc.update_bbox();
    }
    inline accelerated get_acc() const{
        return acc;
    };
};

const accelerated accelerate(const scene &c, const window &w,
    const viewport &v) {
    acc_builder builder(make_windowviewport(w, v) * c.get_xf());
    c.get_scene_data().iterate(builder);
    builder.update_bbox();
    return builder.get_acc();
}

RGBA8 sample(const accelerated& a, float x, float y){
    for (auto obj = a.objects.rbegin(); obj != a.objects.rend(); ++obj){
        if((*obj)->hit(x,y)){
            return (*obj)->get_color();
        }
    }
    return RGBA8(255, 255, 255, 255);
}

void render(accelerated &a, const window &w, const viewport &v,
    FILE *out, const std::vector<std::string> &args) {
    (void) w;
    (void) args;
    int xl, yb, xr, yt;
    std::tie(xl, yb) = v.bl();
    std::tie(xr, yt) = v.tr();
    int width = xr - xl;
    int height = yt - yb;

    image<uint8_t, 4> out_image;
    out_image.resize(width, height);

    RGBA8 s_color;
    for (int i = 1; i <= height; i++) {
        float y = yb+i-1.+.5f;
        for (int j = 1; j <= width; j++) {
            printf("\r%.2f%%", 100*(double)(i*width+j)/(height*width));
            float x = xl+j-1+.5f;
            s_color  = sample(a, x, y);
            out_image.set_pixel(j-1, i-1, s_color[0], s_color[1], s_color[2], s_color[3]);
        }
    }
    printf("\n");
    //a.print_objects();
    a.destroy_objects();
    store_png<uint8_t>(out, out_image);
}

} } } // namespace rvg::driver::png

// Lua version of the accelerate function.
// Since there is no acceleration, we simply
// and return the input scene unmodified.
static int luaaccelerate(lua_State *L) {
    rvg_lua_push<rvg::driver::png::accelerated>(L,
        rvg::driver::png::accelerate(
            rvg_lua_check<rvg::scene>(L, 1),
            rvg_lua_check<rvg::window>(L, 2),
            rvg_lua_check<rvg::viewport>(L, 3)));
    return 1;
}

// Lua version of render function
static int luarender(lua_State *L) {
    auto a = rvg_lua_check<rvg::driver::png::accelerated>(L, 1);
    auto w = rvg_lua_check<rvg::window>(L, 2);
    auto v = rvg_lua_check<rvg::viewport>(L, 3);
    auto o = rvg_lua_optargs(L, 5);
    rvg::driver::png::render(a, w, v, rvg_lua_check_file(L, 4), o);
    return 0;
}

// List of Lua functions exported into driver table
static const luaL_Reg modpngpng[] = {
    {"render", luarender },
    {"accelerate", luaaccelerate },
    {NULL, NULL}
};

// Lua function invoked to be invoked by require"driver.png"
extern "C"
#ifndef _WIN32
__attribute__((visibility("default")))
#else
__declspec(dllexport)
#endif
int luaopen_salles(lua_State *L) {
    rvg_lua_init(L);
    if (!rvg_lua_typeexists<rvg::driver::png::accelerated>(L, -1)) {
        rvg_lua_createtype<rvg::driver::png::accelerated>(L,
            "png accelerated", -1);
    }
    rvg_lua_facade_new_driver(L, modpngpng);
    return 1;
}
