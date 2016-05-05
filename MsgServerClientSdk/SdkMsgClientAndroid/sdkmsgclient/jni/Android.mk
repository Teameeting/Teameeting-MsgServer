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

LOCAL_MODULE    := msgclient-jni
LOCAL_SRC_FILES := on_load.cpp \
			japp_jni.cpp \
			jcontext.cpp \
			jmclient_app.cpp

LOCAL_SRC_FILES += empty.c

## 
## Widows (call host-path,/cygdrive/path/to/your/file/libstlport_shared.so) 	
#		   
LOCAL_LDLIBS := -llog
####LOCAL_LDLIBS += -L$(call host-path,$(LOCAL_PATH)/lib_static)/
####LOCAL_LDFLAGS += lib_static/librtcmsgclient-android.a
LOCAL_CFLAGS := -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS -std=c++11

LOCAL_C_INCLUDES += $(NDK_STL_INC) \
					$(LOCAL_PATH)/ \
					$(LOCAL_PATH)/jutils \
					$(LOCAL_PATH)/dyncRTCMsgClient/ \
                    $(LOCAL_PATH)/dyncRTCMsgClient/rtcmsgs \
                    $(LOCAL_PATH)/dyncRTCMsgClient/client \
                    $(LOCAL_PATH)/dyncRTCMsgClient/proto \
                    $(LOCAL_PATH)/protobuf/src

										

LOCAL_STATIC_LIBRARIES := jni_utils
LOCAL_STATIC_LIBRARIES += rtcmsgclient-android
LOCAL_STATIC_LIBRARIES += libgnustl
LOCAL_STATIC_LIBRARIES += protobuf-lite

include $(BUILD_SHARED_LIBRARY)
