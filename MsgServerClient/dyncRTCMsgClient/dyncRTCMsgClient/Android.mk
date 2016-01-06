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
LOCAL_PATH := $(call my-dir)/
include $(CLEAR_VARS)
WEBRTC_BASE_PATH = webrtc_normal

LOCAL_MODULE    := rtcmsgclient-android
LOCAL_SRC_FILES := XMsgProcesser.cpp \
			XMsgClient.cpp \
			XTcpClientImpl.cpp \
			XTcpTick.cpp 

LOCAL_SRC_FILES += rtcmsgs/RTMeetMsg.cpp \
									 rtcmsgs/RTMessage.cpp \
									 rtcmsgs/RTSignalMsg.cpp
			
LOCAL_SRC_FILES += $(WEBRTC_BASE_PATH)/webrtc/base/asyncfile.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/asyncresolverinterface.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/asyncsocket.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/asynctcpsocket.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/asyncudpsocket.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/base64.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/checks.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/criticalsection.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/event.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/event_tracer.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/ifaddrs-android.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/ipaddress.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/logging.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/messagehandler.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/messagequeue.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/nethelpers.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/physicalsocketserver.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/platform_thread.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/signalthread.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/sigslot.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/socketaddress.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/stringencode.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/thread.cc \
		$(WEBRTC_BASE_PATH)/webrtc/base/timeutils.cc \
	
## 
## Widows (call host-path,/cygdrive/path/to/your/file/libstlport_shared.so) 	
#		   
#LOCAL_LDLIBS := -llog -lz 
LOCAL_CFLAGS := -std=gnu++11 -DPOSIX -D__UCLIBC__ -DWEBRTC_LINUX -DWEBRTC_ANDROID -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS

LOCAL_C_INCLUDES += ./ \
										./rtcmsgs \
										$(NDK_STL_INC) \
										$(LOCAL_PATH)/ \
										$(LOCAL_PATH)/webrtc_normal \

LOCAL_STATIC_LIBRARIES +=  libstlport
					
include $(BUILD_STATIC_LIBRARY)
