#ifndef RVG_BASE64_H
#define RVG_BASE64_H

#include <string>

namespace rvg {

std::string encode_base64(const std::string &input);

std::string decode_base64(const std::string &input);

} // namespace rvg

#endif
