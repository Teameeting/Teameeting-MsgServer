C++ = $(CPLUS)
# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

NAME = rtcmsgclient-linux
C++ = g++
CC = gcc
###INCLUDE_FLAG = -include
WEBRTC_BASE_PATH = .
INCLUDE_FLAG = 
LINK = $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) $(INCLUDE_FLAG) -g -Wall -std=c++11 -DPOSIX -DENABLE_DEBUG=1 -DWEBRTC_LINUX -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS

# OPTIMIZATION
CCFLAGS += -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.
CCFLAGS += -I./rtcmsgs -I./client -I$(WEBRTC_BASE_PATH)

C++FLAGS= $(CCFLAGS)
C++FLAGS+=-I./RTJson -I./RTHttp -I./RTManager


CFILES = 

CPPFILES := main.cpp \
			RTJson/RTJson.cpp \
			RTHttp/RTCurlClient.cpp \
			RTHttp/RTCURL.cpp \
			RTManager/RTConnClientManager.cpp \
			RTManager/RTMsgClient.cpp \
			RTManager/RTMsgThreadPool.cpp \
			RTManager/RTHttpClient.cpp \
			RTManager/RTMsgRoom.cpp

CPPFILES += client/XMsgProcesser.cpp \
			client/XJSBuffer.cpp \
			client/XMsgClient.cpp \
			client/XTcpClientImpl.cpp \
			client/XTcpTick.cpp 

CPPFILES += ./rtcmsgs/RTMeetMsg.cpp \
			./rtcmsgs/RTMessage.cpp \
			./rtcmsgs/RTSignalMsg.cpp

CCFILES := $(WEBRTC_BASE_PATH)/webrtc/base/asyncfile.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/asyncpacketsocket.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/asyncresolverinterface.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/asyncsocket.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/asynctcpsocket.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/asyncudpsocket.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/base64.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/checks.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/common.cc \
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
			$(WEBRTC_BASE_PATH)/webrtc/base/thread_checker_impl.cc \
			$(WEBRTC_BASE_PATH)/webrtc/base/timeutils.cc

CCFILES += \
			./rtcmsgs/proto/msg_type.pb.cc \
			./rtcmsgs/proto/meet_msg.pb.cc \
			./rtcmsgs/proto/sys_msg.pb.cc

CPPFLAGS+=-lcurl
	
all: msgclient

librtcmsgclient-linux.a: $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)
	ar -r librtcmsgclient-linux.a $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)
	ranlib librtcmsgclient-linux.a

msgclient: $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)
	$(C++) -o msgclient $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o) $(CPPFLAGS) -L/usr/local/lib -lprotobuf

install: 
	install -m 664 librtcmsgclient-linux.a ./lib_linux_a/

clean:
	rm -rf librtcmsgclient-linux.a msgclient $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

.SUFFIXES: .cpp .cc .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c

