#ifndef RVG_WINDING_RULE_H
#define RVG_WINDING_RULE_H

#include <cstdint>

namespace rvg {

enum class e_winding_rule: uint8_t {
    non_zero,  // non-zero winding number is in
    odd,       // odd winding number is in
    zero,      // zero winding number is in (non-standard)
    even,      // even winding number is in (non-standard)
};

} // namespace rvg

#endif
