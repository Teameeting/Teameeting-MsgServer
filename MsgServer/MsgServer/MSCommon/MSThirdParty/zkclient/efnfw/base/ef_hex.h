#ifndef __EF_HEX_H__
#define __EF_HEX_H__

#include "ef_btype.h"
#include <string>

namespace ef{


char* byteToHex(uint8 c, char buf[2]);
int hexToByte(char buf[2]);

int32 bytesToHexs(const std::string& bytes, std::string& hex);
int32 bytesToHexs(const char* bytes, int32 len, char* out, int32 outlen);
int32 hexToBytes(const std::string& hex, std::string& bytes);
int32 hexToBytes(const char* hex, int32 len, char* out, int32 outlen);

int32 hexs_bytes_test(const std::string& input);

};

#endif
