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

#ifndef GEARS_BASE_ANDROID_JAVA_GLOBAL_REF_H__
#define GEARS_BASE_ANDROID_JAVA_GLOBAL_REF_H__

#include <assert.h>
#include "basictypes.h"
#include "helpers_android.h"

extern JavaVM* g_vm;

// Based on scoped_refptr, this class encapsulates any java object
// that the VM reference counts. It prevents garbage collection by
// maintaining a "global reference" for the container's lifetime.
// Always assign objects to one of these when it is needed for longer
// than the lifetime of a function or a JavaLocalFrame.
// "T" may be jobject or any of the derived types:
//   jobject
//   jclass
//   jstring
//   jarray
//   jobjectArray
//   jbooleanArray
//   jbyteArray
//   jcharArray
//   jshortArray
//   jintArray
//   jlongArray
//   jfloatArray
//   jdoubleArray
//   jthrowable
// Sample usage:
//   JavaGlobalRef<jclass> my_class;
//   jclass foo = env->FindClass("java/lang/String");
//   my_class.Reset(foo);
template <class T>
class JavaGlobalRef {
 public:
  // Construct with a null reference.
  JavaGlobalRef() : global_ref_(NULL) { }
  // Construct with a global reference to the given object.
  JavaGlobalRef(T object) : global_ref_(NULL) { NewRef(object); }
  // Copy constructor. Create a new unique global reference, which
  // causes another increment of the VM's reference count.
  JavaGlobalRef(const JavaGlobalRef<T> &other) : global_ref_(NULL) {
    NewRef(other.Get());
  }
  // Assignment operator. Equivalent to Reset.
  JavaGlobalRef<T> &operator=(const JavaGlobalRef<T> &rhs) {
    Reset(rhs.Get());
    return *this;
  }

  // Destruction deletes the global reference. If this causes the VM's
  // count to drop to zero, it will become a candidate for garbage
  // collection.
  ~JavaGlobalRef() { DeleteRef(); }
  // Return the contained object.
  T Get() const { return global_ref_; }
  // Transfers responsibility for the global reference to the caller.
  // Returns the global reference and reinitializes this structure to
  // a null reference.
  T Release() {
    T val = global_ref_;
    global_ref_ = NULL;
    return val;
  }
  // Reset to a new object. If we already have a global reference,
  // delete it. If the new reference is non-null, create a new global
  // reference to it. If the new reference is null, initialize as
  // null. The new global reference or null is returned.
  T Reset(T new_object) {
    DeleteRef();
    NewRef(new_object);
    return Get();
  }
  // As above, but for use when the local references is no longer
  // needed after the call. The local reference is deleted before
  // returning the new global reference (or null).
  T MoveLocal(T new_object) {
    DeleteRef();
    if (new_object) {
      NewRef(new_object);
      AttachThreadScoped ats(g_vm);
      ats.env()->DeleteLocalRef(new_object);
    }
    return Get();
  }

 private:
  // Create a global reference to the object, if non-null.
  void NewRef(T object) {
    if (object) {
    	AttachThreadScoped ats(g_vm);
    	global_ref_ = static_cast<T>(ats.env()->NewGlobalRef(object));
    	assert(global_ref_);
    } else {
    	global_ref_ = NULL;
    }
  }
  // Delete the global reference if created.
  void DeleteRef() {
    if (global_ref_) {
    	AttachThreadScoped ats(g_vm);
    	ats.env()->DeleteGlobalRef(global_ref_);
      global_ref_ = NULL;
    }
  }

  T global_ref_;
};

#endif // GEARS_BASE_ANDROID_JAVA_GLOBAL_REF_H__
