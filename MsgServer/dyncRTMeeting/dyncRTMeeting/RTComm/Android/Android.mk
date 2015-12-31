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
LOCAL_PATH := $(call my-dir)/..
NDK_PATH := /cygdrive/c/Android/NDK/android-ndk-r5b

################################################################
# Ucc Jni
include $(CLEAR_VARS)
LOCAL_MODULE    := CommonUtilities

LOCAL_SRC_FILES := \
	base64.c \
	getopt.c \
	GetWord.c \
	LinkedList.c \
	md5.c \
	Trim.c \
	atomic.cpp \
	epev.cpp \
	ConfParser.cpp \
	DateTranslator.cpp \
	EventContext.cpp \
	IdleTask.cpp \
	InternalStdLib.cpp \
	md5digest.cpp \
	MyAssert.cpp \
	OS.cpp \
	OSBufferPool.cpp \
	OSCodeFragment.cpp \
	OSCond.cpp \
	OSFileSource.cpp \
	OSHeap.cpp \
	OSMutex.cpp \
	OSMutexRW.cpp \
	OSQueue.cpp \
	OSRef.cpp \
	OSThread.cpp \
	ResizeableStringFormatter.cpp \
	SDPUtils.cpp \
	Socket.cpp \
	SocketUtils.cpp	\
	StringFormatter.cpp \
	StringParser.cpp \
	StringTranslator.cpp \
	StrPtrLen.cpp \
	Task.cpp \
	TCPListenerSocket.cpp \
	TCPSocket.cpp \
	TimeoutTask.cpp \
	UDPDemuxer.cpp \
	UDPSocket.cpp \
	UDPSocketPool.cpp \
	UserAgentParser.cpp
	

## 
## Widows 环境下连接器(-L)不支持Cygwin paths,所以要使用$(call host-path,/cygdrive/path/to/your/file/libstlport_shared.so) 	
#		   
LOCAL_LDLIBS := -llog -lC 

LOCAL_C_INCLUDES += $(NDK_PATH)/sources/cxx-stl/stlport/stlport 
										
include $(BUILD_STATIC_LIBRARY)
