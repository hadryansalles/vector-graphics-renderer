#ifndef RVG_PNGIO_H
#define RVG_PNGIO_H

#include <string>

#include "rvg-image.h"

namespace rvg {

    int describe_png(FILE *file_in, int *width, int *height,
        int *channels, int *bit_depth);

    int describe_png(const std::string &memory_in, int *width, int *height,
        int *channels, int *bit_depth);

    // load png image from file
    template <typename T, size_t N>
    int load_png(FILE *file_in, image<T,N> *out,
        image_attributes *attrs = nullptr);

    i_image::ptr load_png(FILE *file_in, int wanted_channels = 0,
        image_attributes *attrs = nullptr);

    // load png image from memory
    template <typename T, size_t N>
    int load_png(const std::string &memory_in, image<T,N> *out,
        image_attributes *attrs = nullptr);

    i_image::ptr load_png(const std::string &memory_in, int wanted_channels = 0,
        image_attributes *attrs = nullptr);

    // store png image to file
    template <typename U, typename T, size_t N>
    int store_png(FILE *file_out, const image<T,N> &in,
        const image_attributes &attrs = image_attributes());

    template <typename U>
    int store_png(FILE *file_out, i_image::const_ptr in,
        const image_attributes &attrs = image_attributes());

    // store png to memory
    template <typename U, typename T, size_t N>
    int store_png(std::string *memory_out, const image<T,N> &in,
        const image_attributes &attrs = image_attributes());

    template <typename U>
    int store_png(std::string *memory_out, i_image::const_ptr in,
        const image_attributes &attrs = image_attributes());

    // delcare explicit instantiations for all image types
    extern template int load_png(FILE *file_in, image<uint8_t, 1> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<uint8_t, 2> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<uint8_t, 3> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<uint8_t, 4> *out,
        image_attributes *attrs);

    extern template int load_png(FILE *file_in, image<uint16_t, 1> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<uint16_t, 2> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<uint16_t, 3> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<uint16_t, 4> *out,
        image_attributes *attrs);

    extern template int load_png(FILE *file_in, image<float, 1> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<float, 2> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<float, 3> *out,
        image_attributes *attrs);
    extern template int load_png(FILE *file_in, image<float, 4> *out,
        image_attributes *attrs);

    extern template int load_png(const std::string &memory_in,
        image<uint8_t, 1> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<uint8_t, 2> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<uint8_t, 3> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<uint8_t, 4> *out, image_attributes *attrs);

    extern template int load_png(const std::string &memory_in,
        image<uint16_t, 1> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<uint16_t, 2> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<uint16_t, 3> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<uint16_t, 4> *out, image_attributes *attrs);

    extern template int load_png(const std::string &memory_in,
        image<float, 1> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<float, 2> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<float, 3> *out, image_attributes *attrs);
    extern template int load_png(const std::string &memory_in,
        image<float, 4> *out, image_attributes *attrs);

    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint8_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint8_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint8_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint8_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint16_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint16_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint16_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<uint16_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint8_t>(FILE *file_out,
        const image<float, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<float, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<float, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        const image<float, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint8_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint8_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint8_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint8_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint16_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint16_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint16_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<uint16_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint16_t>(FILE *file_out,
        const image<float, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<float, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<float, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        const image<float, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint8_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint8_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint8_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint8_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint16_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint16_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint16_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<uint16_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<float, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<float, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<float, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint8_t>(std::string *memory_out,
        const image<float, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint8_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint8_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint8_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint8_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint16_t, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint16_t, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint16_t, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<uint16_t, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<float, 1> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<float, 2> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<float, 3> &in, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        const image<float, 4> &in, const image_attributes &attrs);

    extern template int store_png<uint8_t>(std::string *memory_out,
        i_image::const_ptr in_ptr, const image_attributes &attrs);
    extern template int store_png<uint16_t>(std::string *memory_out,
        i_image::const_ptr in_ptr, const image_attributes &attrs);
    extern template int store_png<uint8_t>(FILE *file_out,
        i_image::const_ptr in_ptr, const image_attributes &attrs);
    extern template int store_png<uint16_t>(FILE *file_out,
        i_image::const_ptr in_ptr, const image_attributes &attrs);

} // namespace rvg

#endif // RVG_PNGIO_H
