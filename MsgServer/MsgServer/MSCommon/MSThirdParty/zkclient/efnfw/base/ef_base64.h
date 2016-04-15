#ifndef __EF_BASE64_H__
#define __EF_BASE64_H__

#include <string>

namespace ef{

std::string base64Encode(const std::string& s);
std::string base64Decode(const std::string& s);

}

#endif
