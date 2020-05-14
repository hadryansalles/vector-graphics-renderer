#include "hadryan-bouding-box.h"

using namespace rvg;

namespace hadryan {

bouding_box::bouding_box()
    : m_p0(0, 0)
    , m_p1(0, 0)
{}

bouding_box::bouding_box(const R2 &first, const R2 &last) {
    m_p0 = make_R2(std::min(first[0], last[0]), std::min(first[1], last[1]));
    m_p1 = make_R2(std::max(first[0], last[0]), std::max(first[1], last[1]));
}

}