#include "hadryan-accelerated-builder.h"

#include "rvg-input-path-f-close-contours.h"
#include "rvg-input-path-f-xform.h"
#include "rvg-input-path-f-downgrade-degenerate.h"
#include "rvg-input-path-f-monotonize.h"

#include "hadryan-monotonic-path-builder.h"
#include "hadryan-input-path-not-interger.h"
#include "hadryan-tree-node.h"
#include "hadryan-scene-object.h"
#include "hadryan-blue-noise.h"

using namespace rvg;

namespace hadryan {

void accelerated_builder::unpack_args(const std::vector<std::string> &args) {
    acc.samples = blue_noise::get_1();
    double tx = 0;
    double ty = 0;
    for (auto &arg : args) {
        std::string delimiter = ":";
        std::string command = arg.substr(0, arg.find(delimiter)); 
        std::string value = arg.substr(arg.find(delimiter)+1, arg.length()); 
        if(command == std::string{"-pattern"}) {
            if(value == std::string{"1"}) {
                acc.samples = blue_noise::get_1();
            } else if(value == std::string{"8"}) {
                acc.samples = blue_noise::get_8();
            } else if(value == std::string{"16"}) {
                acc.samples = blue_noise::get_16();
            } else if(value == std::string{"32"}) {
                acc.samples = blue_noise::get_32();
            } else if(value == std::string{"64"}) {
                acc.samples = blue_noise::get_64();
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

} // hadryan