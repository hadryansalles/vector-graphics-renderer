#include <iostream>

#include "rvg-svg-path-token.h"

namespace rvg {

std::ostream &operator<<(std::ostream &out, const svg_path_token &token) {
    out << "svg_path_token{";
    switch (token.type) {
        case svg_path_token::e_type::command:
            out << "command, '" << static_cast<char>(token.value.i) << '\'';
            break;
        case svg_path_token::e_type::number:
            out << "number, " << token.value.f;
            break;
        case svg_path_token::e_type::underflow: out << "underflow, " << token.value.i;
            break;
        case svg_path_token::e_type::overflow: out << "overflow, " << token.value.i;
            break;
        case svg_path_token::e_type::error: out << "error, " << token.value.i;
            break;
        case svg_path_token::e_type::end: out << "end";
            break;
    }
    out << '}';
    return out;
}

} // namespace rvg
