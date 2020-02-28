#include "hadryan-builder.h"
#include "blue-noise.h"

namespace rvg {
    namespace driver {
        namespace png {

void accelerated_builder::unpack_args(const std::vector<std::string> &args) {
    acc.samples = blue_1;
    double tx = 0;
    double ty = 0;
    for (auto &arg : args) {
        std::string delimiter = ":";
        std::string command = arg.substr(0, arg.find(delimiter)); 
        std::string value = arg.substr(arg.find(delimiter)+1, arg.length()); 
        if(command == std::string{"-pattern"}) {
            if(value == std::string{"1"}) {
                acc.samples = blue_1;
            } else if(value == std::string{"8"}) {
                acc.samples = blue_8;
            } else if(value == std::string{"16"}) {
                acc.samples = blue_16;
            } else if(value == std::string{"32"}) {
                acc.samples = blue_32;
            } else if(value == std::string{"64"}) {
                acc.samples = blue_64;
            }
        } else if(command == std::string{"-tx"}) {
            tx = std::stof(value);
        } else if(command == std::string{"-ty"}) {
            ty = std::stof(value);
        } else if(command == std::string{"-j"}) {
            acc.threads = std::stoi(value);
        } else if(command == std::string{"-depth"}) {
            tree_node::set_max_depth(std::stoi(value));
        } else if(command == std::string{"-min_seg"}) {
            tree_node::set_min_segments(std::stoi(value));
        } else if(command == std::string{"-debug_x"}) {
            acc.debug = make_R2(std::stof(value), acc.debug[1]);
            acc.debugging = true;
        } else if(command == std::string{"-debug_y"}) {
            acc.debug = make_R2(acc.debug[0], std::stof(value));
            acc.debugging = true;
        }
    }
    push_xf(translation(tx, ty));
}

void accelerated_builder::do_painted_shape(e_winding_rule wr, const shape &s, const paint &p){
    xform post;
    monotonic_builder path_builder;
    path_data::const_ptr path_data = s.as_path_data_ptr(post);
    const xform s_xf = post*top_xf()*s.get_xf();
    path_data->iterate(make_input_path_f_close_contours(
                        make_input_path_f_xform(s_xf,
                        make_input_path_f_downgrade_degenerate(
                        make_input_path_f_monotonize(
                        make_input_path_not_interger(
                        path_builder))))));
    if(path_builder.get().size() > 0) {
        acc.add(new scene_object(path_builder.get(), wr, p.transformed(top_xf())));
    } 
}

}}}