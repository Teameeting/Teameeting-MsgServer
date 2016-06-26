# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.

CC    	=      gcc
AR    	=      ar
RM    	=      rm
CP    	=      cp
ECHO  	=      echo

C++ 	= g++
LINK 	= $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) -g -Wall -std=c++11 -DPOSIX -DENABLE_DEBUG=1 -DWEBRTC_LINUX -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DNO_MAIN_THREAD_WRAPPING

# OPTIMIZATION
CCFLAGS += -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.
CCFLAGS += -I./core

CCFLAGS += -I/usr/local/include

C++FLAGS= $(CCFLAGS)

#############################################################

CFILES = 

CPPFILES := core/XMsgProcesser.cpp \
			core/XMsgClient.cpp \
			core/XJSBuffer.cpp \
			core/XTcpClientImpl.cpp \
			core/XTcpTick.cpp

CCFILES :=  webrtc/base/asyncfile.cc \
			webrtc/base/asyncpacketsocket.cc \
			webrtc/base/asyncresolverinterface.cc \
			webrtc/base/asyncsocket.cc \
			webrtc/base/asynctcpsocket.cc \
			webrtc/base/asyncudpsocket.cc \
			webrtc/base/base64.cc \
			webrtc/base/checks.cc \
			webrtc/base/common.cc \
			webrtc/base/criticalsection.cc \
			webrtc/base/event.cc \
			webrtc/base/event_tracer.cc \
			webrtc/base/ifaddrs-android.cc \
			webrtc/base/ipaddress.cc \
			webrtc/base/logging.cc \
			webrtc/base/messagehandler.cc \
			webrtc/base/messagequeue.cc \
			webrtc/base/nethelpers.cc \
			webrtc/base/physicalsocketserver.cc \
			webrtc/base/platform_thread.cc \
			webrtc/base/signalthread.cc \
			webrtc/base/sigslot.cc \
			webrtc/base/socketaddress.cc \
			webrtc/base/stringencode.cc \
			webrtc/base/thread.cc \
			webrtc/base/thread_checker_impl.cc \
			webrtc/base/timeutils.cc

CCFILES += \
			proto/common_msg.pb.cc \
			proto/entity_msg.pb.cc \
			proto/entity_msg_type.pb.cc \
			proto/sys_msg.pb.cc \
			proto/sys_msg_type.pb.cc \
			proto/storage_msg_type.pb.cc \
			proto/storage_msg.pb.cc

####################################################################

include    tool.mak
TARGET     = libclient_common.a
LIBS       = libprotobuf-lite.a
OBJS       = $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

all:$(TARGET)

$(TARGET):$(OBJS) $(LIBS)
		$(BUILD_LIBRARY)

install: 
	install -m 664 libclient_common.a ./lib_linux_a/

clean:
	rm -rf libclient_common.a $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

.SUFFIXES: .cpp .cc .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c

