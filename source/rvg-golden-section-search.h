#ifndef RVG_GOLDEN_SECTION_SEARCH_H
#define RVG_GOLDEN_SECTION_SEARCH_H

#include <limits>

namespace rvg {

template <typename F, typename T>
float golden_section_search(F f, T a, T b, T tol,
    int niter = std::numeric_limits<T>::digits) {
    constexpr T gr = T(1.618033988749895);
    T c = b - (b-a)/gr;
    T d = a + (b-a)/gr;
    int iter = 0;
    while (std::fabs(c-d) > tol && iter < niter) {
        if (f(c) < f(d)) {
            b = d;
        } else {
            a = c;
        }
        c = b - (b-a)/gr;
        d = a + (b-a)/gr;
        iter++;
    }
    return T(0.5)*(b+a);
}

} // namespace rvg

#endif
