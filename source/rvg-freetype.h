#ifndef RVG_FREETYPE_H
#define RVG_FREETYPE_H

#include <memory>
#include <type_traits>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "rvg-freetype-typeface.h"

namespace rvg {

class freetype final {

    using library_ptr =
        std::shared_ptr<typename std::remove_pointer<FT_Library>::type>;

    library_ptr m_library;

public:

    freetype(void);

    freetype(library_ptr library);

    int get_memory_num_faces(const uint8_t *base, size_t size);

    int get_num_faces(const char *filename);

    freetype_typeface::ptr new_face(const char *filename, int face_index = 0);

    freetype_typeface::ptr new_memory_face(const uint8_t *base, size_t size,
        int face_index = 0);
};

}

#endif
