# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.cpprg/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################
# Ucc Jni
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := protobuf-lite 
LOCAL_SRC_FILES :=  \
										./src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc         \
										./src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc        \
										./src/google/protobuf/stubs/bytestream.cc                          \
										./src/google/protobuf/stubs/bytestream.h                           \
										./src/google/protobuf/stubs/common.cc                              \
										./src/google/protobuf/stubs/hash.h                                 \
										./src/google/protobuf/stubs/int128.cc                              \
										./src/google/protobuf/stubs/int128.h                               \
										./src/google/protobuf/stubs/map_util.h                             \
										./src/google/protobuf/stubs/mathutil.h                             \
										./src/google/protobuf/stubs/once.cc                                \
										./src/google/protobuf/stubs/shared_ptr.h                           \
										./src/google/protobuf/stubs/status.cc                              \
										./src/google/protobuf/stubs/status.h                               \
										./src/google/protobuf/stubs/status_macros.h                        \
										./src/google/protobuf/stubs/statusor.cc                            \
										./src/google/protobuf/stubs/statusor.h                             \
										./src/google/protobuf/stubs/stringpiece.cc                         \
										./src/google/protobuf/stubs/stringpiece.h                          \
										./src/google/protobuf/stubs/stringprintf.cc                        \
										./src/google/protobuf/stubs/stringprintf.h                         \
										./src/google/protobuf/stubs/structurally_valid.cc                  \
										./src/google/protobuf/stubs/strutil.cc                             \
										./src/google/protobuf/stubs/strutil.h                              \
										./src/google/protobuf/stubs/time.cc                                \
										./src/google/protobuf/stubs/time.h                                 \
										./src/google/protobuf/arena.cc                                     \
										./src/google/protobuf/arenastring.cc                               \
										./src/google/protobuf/extension_set.cc                             \
										./src/google/protobuf/generated_message_util.cc                    \
										./src/google/protobuf/message_lite.cc                              \
										./src/google/protobuf/repeated_field.cc                            \
										./src/google/protobuf/wire_format_lite.cc                          \
										./src/google/protobuf/io/coded_stream.cc                           \
										./src/google/protobuf/io/coded_stream_inl.h                        \
										./src/google/protobuf/io/zero_copy_stream.cc                       \
										./src/google/protobuf/io/zero_copy_stream_impl_lite.cc


			## 
## Widows (call host-path,/cygdrive/path/to/your/file/libstlport_shared.so) 	
#		   
#LOCAL_LDLIBS := -llog -lz 
LOCAL_CFLAGS := -frtti -DHAVE_PTHREAD

LOCAL_EXPORT_C_INCLUDES :=
LOCAL_EXPORT_LDLIBS :=

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
					$(LOCAL_PATH)/src
					
include $(BUILD_STATIC_LIBRARY)
