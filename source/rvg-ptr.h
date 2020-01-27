#ifndef RVG_PTR
#define RVG_PTR

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace rvg {

template <typename T, typename ...ARGS>
boost::intrusive_ptr<T> make_intrusive(ARGS&&... args) {
    return boost::intrusive_ptr<T>(new T(std::forward<ARGS>(args)...));
}

template <typename T, typename ...ARGS>
boost::intrusive_ptr<T> make_dashes_intrusive(ARGS&&... args) {
    fprintf(stderr, "initializing dashes\n");
    return boost::intrusive_ptr<T>(new T(std::forward<ARGS>(args)...));
}

template <typename T, typename ...ARGS>
boost::intrusive_ptr<T> make_style_intrusive(ARGS&&... args) {
    fprintf(stderr, "initializing style\n");
    return boost::intrusive_ptr<T>(new T(std::forward<ARGS>(args)...));
}

} // namespace rvg

#endif
