#ifndef WEBRTC_BASE_STRINGENCODE_H_
#define WEBRTC_BASE_STRINGENCODE_H_

#include <string>

namespace rtc {

// Convert an unsigned value from 0 to 15 to the hex character equivalent...
char hex_encode(unsigned char val);
// ...and vice-versa.
bool hex_decode(char ch, unsigned char* val);

// hex_encode shows the hex representation of binary data in ascii.
size_t hex_encode(char* buffer, size_t buflen,
                  const char* source, size_t srclen);

// hex_encode, but separate each byte representation with a delimiter.
// |delimiter| == 0 means no delimiter
// If the buffer is too short, we return 0
size_t hex_encode_with_delimiter(char* buffer, size_t buflen,
                                 const char* source, size_t srclen,
                                 char delimiter);

// Helper functions for hex_encode.
std::string hex_encode(const char* source, size_t srclen);
std::string hex_encode_with_delimiter(const char* source, size_t srclen,
                                      char delimiter);

// hex_decode converts ascii hex to binary.
size_t hex_decode(char* buffer, size_t buflen,
                  const char* source, size_t srclen);

// hex_decode, assuming that there is a delimiter between every byte
// pair.
// |delimiter| == 0 means no delimiter
// If the buffer is too short or the data is invalid, we return 0.
size_t hex_decode_with_delimiter(char* buffer, size_t buflen,
                                 const char* source, size_t srclen,
                                 char delimiter);

// Helper functions for hex_decode.
size_t hex_decode(char* buffer, size_t buflen, const std::string& source);
size_t hex_decode_with_delimiter(char* buffer, size_t buflen,
                                 const std::string& source, char delimiter);
}	// namespace rtc

#endif