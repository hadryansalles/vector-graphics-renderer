#include "rvg-svg-path-token.h"

namespace rvg {

    namespace svg_path_commands {

        svg_path_token T{svg_path_token::e_type::command, rvgi{'T'}};
        svg_path_token t{svg_path_token::e_type::command, rvgi{'t'}};
        svg_path_token R{svg_path_token::e_type::command, rvgi{'R'}};
        svg_path_token r{svg_path_token::e_type::command, rvgi{'r'}};
        svg_path_token A{svg_path_token::e_type::command, rvgi{'A'}};
        svg_path_token a{svg_path_token::e_type::command, rvgi{'a'}};
        svg_path_token C{svg_path_token::e_type::command, rvgi{'C'}};
        svg_path_token c{svg_path_token::e_type::command, rvgi{'c'}};
        svg_path_token H{svg_path_token::e_type::command, rvgi{'H'}};
        svg_path_token h{svg_path_token::e_type::command, rvgi{'h'}};
        svg_path_token L{svg_path_token::e_type::command, rvgi{'L'}};
        svg_path_token l{svg_path_token::e_type::command, rvgi{'l'}};
        svg_path_token M{svg_path_token::e_type::command, rvgi{'M'}};
        svg_path_token m{svg_path_token::e_type::command, rvgi{'m'}};
        svg_path_token Q{svg_path_token::e_type::command, rvgi{'Q'}};
        svg_path_token q{svg_path_token::e_type::command, rvgi{'q'}};
        svg_path_token S{svg_path_token::e_type::command, rvgi{'S'}};
        svg_path_token s{svg_path_token::e_type::command, rvgi{'s'}};
        svg_path_token V{svg_path_token::e_type::command, rvgi{'V'}};
        svg_path_token v{svg_path_token::e_type::command, rvgi{'v'}};
        svg_path_token Z{svg_path_token::e_type::command, rvgi{'Z'}};

    }

} // namespace rvg::svg_path_tokens
