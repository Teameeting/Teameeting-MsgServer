// Copyright 2008, Google Inc.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  3. Neither the name of Google Inc. nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef GEARS_BASE_ANDROID_JAVA_STRING_H__
#define GEARS_BASE_ANDROID_JAVA_STRING_H__

#include "java_global_ref.h"
#include "basictypes.h"
#include "string16.h"

// Wrapper for jstring. It is common to convert between UTF-8 and
// UTF-16 encodings, as well as transfer between raw and std::string
// containers. This provides convenience methods for a few common
// cases, as well as maintaining a global reference. Copy constructor
// and assignment are safe.
class JavaString {
 public:
  // Creates a new global reference to an existing jstring.
  JavaString(jstring jstr) : java_string_(jstr) { }
  // Create a jstring from a raw UTF-8, zero terminated string.
  JavaString(const char *utf8);
  // Create a jstring from a raw UTF-16, zero terminated string.
  JavaString(const char16 *utf16);
  // Create a jstring from a UTF-8 encoded std::string.
  JavaString(const std::string &stl_string8);
  // Create a jstring from a UTF-16 encoded std::string16.
  JavaString(const std::string16 &stl_string16);
  // Copy constructor. The global reference container is safe to copy.
  JavaString(const JavaString &other) : java_string_(other.java_string_) { }
  // Assignment operator. The global reference is safe to assign.
  JavaString &operator=(const JavaString &rhs) {
    java_string_ = rhs.java_string_;
    return *this;
  }

  // Convert the jstring to a UTF-8 encoded std::string. Returns true
  // on success, false on error such as a malformed string.
  bool ToString8(std::string *out8) const;
  // Convert the jstring to a UTF-16 encoded std::string16. Returns
  // true on success, false on error such as a malformed string.
  bool ToString16(std::string16 *out16) const;
  // Convert the jstring to a UTF-8 encoded std::string returned by
  // value. Returns an empty string on error. Do not use when you need
  // to distinguish between an error and an empty string.
  std::string ToString8() const;
  // Convert the jstring to a UTF-16 encoded std::string returned by
  // value. Returns an empty string on error. Do not use when you need
  // to distinguish between an error and an empty string.
  std::string16 ToString16() const;
  // Get the global object referencing the jstring.
  jstring Get() const { return java_string_.Get(); }

 private:
  // A global reference to the jstring.
  JavaGlobalRef<jstring> java_string_;
};

#endif // GEARS_BASE_ANDROID_JAVA_STRING_H__
