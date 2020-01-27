#ifndef RVG_HARFBUZZ_FEATURES_H
#define RVG_HARFBUZZ_FEATURES_H

#include <vector>
#include <cstring>
#include <cstdio>

#include <hb.h>

#include "rvg-ptr.h"

namespace rvg {

class harfbuzz_features:
    private std::vector<hb_feature_t>,
    public boost::intrusive_ref_counter<harfbuzz_features> {

    using vector_base = std::vector<hb_feature_t>;

public:

    using ptr = boost::intrusive_ptr<harfbuzz_features>;
    using const_ptr = boost::intrusive_ptr<const harfbuzz_features>;

	explicit harfbuzz_features(const char *s) {
		const char *n = nullptr;
		while (s && *s) {
			n = strchr(s, ',');
			hb_feature_t f;
			if (hb_feature_from_string(s, static_cast<int>(n? n-s: -1), &f))
				push_back(f);
			if (!n) break;
			s = n+1;
		}
	}

    harfbuzz_features(void) = default;
    harfbuzz_features(const std::initializer_list<hb_feature_t> &list):
        vector_base(list) { ; }
    harfbuzz_features(const harfbuzz_features &other) = default;
    harfbuzz_features(harfbuzz_features &&other) = default;
    harfbuzz_features(const std::vector<hb_feature_t> &other):
		vector_base(other) { ; }
    harfbuzz_features(std::vector<hb_feature_t> &&other):
        vector_base(std::move(other)) { ; }
    harfbuzz_features &operator=(const harfbuzz_features &other) = default;
    harfbuzz_features &operator=(harfbuzz_features &&other) = default;

    using vector_base::size;
    using vector_base::empty;
    using vector_base::begin;
    using vector_base::end;
    using vector_base::capacity;
    using vector_base::reserve;
    using vector_base::resize;
    using vector_base::operator[];
};

}

#endif
