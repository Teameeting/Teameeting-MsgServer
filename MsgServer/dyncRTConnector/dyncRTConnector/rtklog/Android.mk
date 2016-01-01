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
LOCAL_PATH := $(call my-dir)/
include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cc .cpp

LOCAL_MODULE    := rtklog
LOCAL_SRC_FILES := rtklog.cpp \
			talk/base/common.cc \
			talk/base/event.cc \
			talk/base/fileutils.cc \
			talk/base/logging.cc \
			talk/base/messagehandler.cc \
			talk/base/messagequeue.cc \
			talk/base/pathutils.cc \
			talk/base/stream.cc \
			talk/base/stringencode.cc \
			talk/base/stringutils.cc \
			talk/base/thread.cc \
			talk/base/timeutils.cc \
			talk/base/timing.cc \
			talk/base/unixfilesystem.cc \
			talk/base/urlencode.cc \

LOCAL_CFLAGS := -DLOGGING=1 -D__avc_log__ -DNO_MAIN_THREAD_WRAPPING -DHASH_NAMESPACE=__gnu_cxx -DPOSIX -DDISABLE_DYNAMIC_CAST -DOS_LINUX=OS_LINUX -DLINUX -D_DEBUG 
	
LOCAL_LDLIBS := -llog 

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
												
include $(BUILD_SHARED_LIBRARY)