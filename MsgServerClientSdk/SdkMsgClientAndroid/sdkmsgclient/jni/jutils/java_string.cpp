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
//
// Java string wrapper with handy constructors and converters.

#include <assert.h>
#include "java_string.h"
#include "string_utils.h"
#include "jni_helpers.h"

extern JavaVM* g_vm;

// Create a jstring from a raw UTF-8, zero terminated string.
JavaString::JavaString(const char *utf8)
    : java_string_(NULL) {
  // Use our internal UTF8 conversion as JNI has a "modified UTF-8"
  // encoding.
  std::string16 stl_string16;
  if (UTF8ToString16(utf8, &stl_string16)) {
	  AttachThreadScoped ats(g_vm);
	  java_string_.MoveLocal(ats.env()->NewString(stl_string16.data(),
                                                  stl_string16.size()));
  } else {
	  LGE("JavaString utf8 trans failed.");
    java_string_.Reset(NULL);
  }
}

// Create a jstring from a raw UTF-16, zero terminated string.
JavaString::JavaString(const char16 *utf16)
    : java_string_(NULL) {
	AttachThreadScoped ats(g_vm);
	java_string_.MoveLocal(
			ats.env()->NewString(utf16, std::char_traits<char16>::length(utf16)));
}

// Create a jstring from a UTF-8 encoded std::string.
JavaString::JavaString(const std::string &stl_string8)
    : java_string_(NULL) {
  // Use our internal UTF8 conversion as JNI has a "modified UTF-8"
  // encoding.
  std::string16 stl_string16;
  if (UTF8ToString16(stl_string8, &stl_string16)) {
	  AttachThreadScoped ats(g_vm);
	  java_string_.MoveLocal(ats.env()->NewString(stl_string16.data(),
                                                  stl_string16.size()));
  } else {
    java_string_.Reset(NULL);
  }
}

// Create a jstring from a UTF-16 encoded std::string16.
JavaString::JavaString(const std::string16 &stl_string16)
    : java_string_(NULL) {
	AttachThreadScoped ats(g_vm);
	java_string_.MoveLocal(ats.env()->NewString(stl_string16.data(),
                                                stl_string16.size()));
}

// Convert the jstring to a UTF-8 encoded std::string. Returns true on
// success.
bool JavaString::ToString8(std::string *out8) const {
  // Use out internal UTF8 conversion as JNi has a "modified UTF-8"
  // encoding.
  std::string16 out16;
  return ToString16(&out16) && String16ToUTF8(out16, out8);
}

// As above, but with the std::string returned by value, and an empty
// string returned on error.
std::string JavaString::ToString8() const {
  std::string result;
  if (ToString8(&result)) {
    return result;
  } else {
    return std::string();
  }
}

// Convert the jstring to a UTF-16 encoded std::string16. Returns true
// on success.
bool JavaString::ToString16(std::string16 *out16) const {
	AttachThreadScoped ats(g_vm);
  JNIEnv *env = ats.env();
  jstring jstr = java_string_.Get();
  if (!jstr) {
    // Can't convert a null jstring.
    return false;
  }

  // Allocate and convert to a UTF-16 encoded output string.
  const char16 *ustr = env->GetStringChars(jstr, 0);
  CHECK_EXCEPTION(env, "Error during GetStringChars");
  if (!ustr)
    return false;
  int length = env->GetStringLength(jstr);
  CHECK_EXCEPTION(env, "Error during GetStringLength");
  *out16 = std::string16(ustr, length);
  // Release the conversion buffer.
  env->ReleaseStringChars(jstr, ustr);
  CHECK_EXCEPTION(env, "Error during ReleaseStringChars");
  return true;
}

// As above, but with the std::string16 returned by value, and an
// empty string returned on error.
std::string16 JavaString::ToString16() const {
  std::string16 result;
  if (ToString16(&result)) {
    return result;
  } else {
    return std::string16();
  }
}
