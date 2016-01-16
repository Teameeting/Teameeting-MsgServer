#include "hexencode.h"
#include <stdio.h>
#include <stdlib.h>

#include "basictypes.h"

namespace rtc {

#define STACK_ARRAY(TYPE, LEN) static_cast<TYPE*>(::alloca((LEN)*sizeof(TYPE)))

// Convert an unsigned value from 0 to 15 to the hex character equivalent...
static const char HEX[] = "0123456789abcdef";

char hex_encode(unsigned char val) {
  //DCHECK_LT(val, 16);
  return (val < 16) ? HEX[val] : '!';
}

bool hex_decode(char ch, unsigned char* val) {
  if ((ch >= '0') && (ch <= '9')) {
    *val = ch - '0';
  } else if ((ch >= 'A') && (ch <= 'Z')) {
    *val = (ch - 'A') + 10;
  } else if ((ch >= 'a') && (ch <= 'z')) {
    *val = (ch - 'a') + 10;
  } else {
    return false;
  }
  return true;
}

size_t hex_encode(char* buffer, size_t buflen,
                  const char* csource, size_t srclen) {
  return hex_encode_with_delimiter(buffer, buflen, csource, srclen, 0);
}

size_t hex_encode_with_delimiter(char* buffer, size_t buflen,
                                 const char* csource, size_t srclen,
                                 char delimiter) {
  if (buffer == NULL || buflen == 0)
    return 0;

  // Init and check bounds.
  const unsigned char* bsource =
      reinterpret_cast<const unsigned char*>(csource);
  size_t srcpos = 0, bufpos = 0;
  size_t needed = delimiter ? (srclen * 3) : (srclen * 2 + 1);
  if (buflen < needed)
    return 0;

  while (srcpos < srclen) {
    unsigned char ch = bsource[srcpos++];
    buffer[bufpos  ] = hex_encode((ch >> 4) & 0xF);
    buffer[bufpos+1] = hex_encode((ch     ) & 0xF);
    bufpos += 2;

    // Don't write a delimiter after the last byte.
    if (delimiter && (srcpos < srclen)) {
      buffer[bufpos] = delimiter;
      ++bufpos;
    }
  }

  // Null terminate.
  buffer[bufpos] = '\0';
  return bufpos;
}

std::string hex_encode(const char* source, size_t srclen) {
  return hex_encode_with_delimiter(source, srclen, 0);
}

std::string hex_encode_with_delimiter(const char* source, size_t srclen,
                                      char delimiter) {
  const size_t kBufferSize = srclen * 3;
  char* buffer = STACK_ARRAY(char, kBufferSize);
  size_t length = hex_encode_with_delimiter(buffer, kBufferSize,
                                            source, srclen, delimiter);
  //DCHECK(srclen == 0 || length > 0);
  return std::string(buffer, length);
}

size_t hex_decode(char * cbuffer, size_t buflen,
                  const char * source, size_t srclen) {
  return hex_decode_with_delimiter(cbuffer, buflen, source, srclen, 0);
}

size_t hex_decode_with_delimiter(char* cbuffer, size_t buflen,
                                 const char* source, size_t srclen,
                                 char delimiter) {
  if (cbuffer== NULL || buflen == 0)
    return 0;

  // Init and bounds check.
  unsigned char* bbuffer = reinterpret_cast<unsigned char*>(cbuffer);
  size_t srcpos = 0, bufpos = 0;
  size_t needed = (delimiter) ? (srclen + 1) / 3 : srclen / 2;
  if (buflen < needed)
    return 0;

  while (srcpos < srclen) {
    if ((srclen - srcpos) < 2) {
      // This means we have an odd number of bytes.
      return 0;
    }

    unsigned char h1, h2;
    if (!hex_decode(source[srcpos], &h1) ||
        !hex_decode(source[srcpos + 1], &h2))
      return 0;

    bbuffer[bufpos++] = (h1 << 4) | h2;
    srcpos += 2;

    // Remove the delimiter if needed.
    if (delimiter && (srclen - srcpos) > 1) {
      if (source[srcpos] != delimiter)
        return 0;
      ++srcpos;
    }
  }

  return bufpos;
}

size_t hex_decode(char* buffer, size_t buflen, const std::string& source) {
  return hex_decode_with_delimiter(buffer, buflen, source, 0);
}
size_t hex_decode_with_delimiter(char* buffer, size_t buflen,
                                 const std::string& source, char delimiter) {
  return hex_decode_with_delimiter(buffer, buflen,
                                   source.c_str(), source.length(), delimiter);
}

}	// namespace rtc