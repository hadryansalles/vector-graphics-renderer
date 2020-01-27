#include <chrono>

#include "rvg-chronos.h"

namespace rvg {

chronos::
chronos(void) {
    reset();
}

void
chronos::
reset(void) {
    m_reset = time();
}

double
chronos::
elapsed(void) {
    return time() - m_reset;
}

double
chronos::time(void) {
    return static_cast<double>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count())*1.e-6;
}

} // namespace rvg
