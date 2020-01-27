#ifndef RVG_STROKE_DASHES_H
#define RVG_STROKE_DASHES_H

#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include "rvg-ptr.h"

namespace rvg {

class stroke_dashes final:
    private std::vector<float>,
    public boost::intrusive_ref_counter<stroke_dashes> {

    using vector_base = std::vector<float>;

public:
    using ptr = boost::intrusive_ptr<stroke_dashes>;
    using const_ptr = boost::intrusive_ptr<const stroke_dashes>;

    stroke_dashes(void) = default;

    stroke_dashes(const std::initializer_list<float> &list):
        vector_base(list) {
        this->adjust();
    }

    stroke_dashes(const stroke_dashes &other) = default;

    stroke_dashes(stroke_dashes &&other) = default;

    stroke_dashes(const std::vector<float> &other):
        vector_base(other) {
        this->adjust();
    }

    stroke_dashes(std::vector<float> &&other):
        vector_base(std::move(other)) {
        this->adjust();
    }

    stroke_dashes &operator=(const stroke_dashes &other) = default;
    stroke_dashes &operator=(stroke_dashes &&other) = default;

    void adjust(void) {
        // If an odd number of dashes were given, repeat them
        if (size() % 2 == 1) {
            auto old_count = this->size();
            this->reserve(2*old_count);
            std::copy_n(this->begin(), old_count, std::back_inserter(*this));
        }
        // Make sure all dashes have non-negative length
        float s = 0.f;
        for (float &d: *this) {
            d = std::fabs(d);
            s += d;
        }
        // If dashes sum to zero, ignore
        if (s == 0.f) this->clear();
    }

    using vector_base::back;
    using vector_base::front;
    using vector_base::size;
    using vector_base::empty;
    using vector_base::begin;
    using vector_base::end;
    using vector_base::capacity;
    using vector_base::reserve;
    using vector_base::resize;
    using vector_base::push_back;
    using vector_base::operator[];
    using vector_base::value_type;
};

} // namespace rvg

#endif
