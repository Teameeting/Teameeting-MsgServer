C++ = $(CPLUS)
# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

NAME = rtcmsgclient-linux
C++ = g++
CC = gcc
###INCLUDE_FLAG = -include
WEBRTC_BASE_PATH = ./webrtc_normal
INCLUDE_FLAG = 
LINK = $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) $(INCLUDE_FLAG) -g -Wall -std=c++0x -DPOSIX -DWEBRTC_LINUX -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS

# OPTIMIZATION
CCFLAGS += -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.
CCFLAGS += -I./rtcmsgs -I$(WEBRTC_BASE_PATH)

C++FLAGS = $(CCFLAGS)

CFILES = 

CPPFILES := XMsgProcesser.cpp \
			XMsgClient.cpp \
			XTcpClientImpl.cpp \
			XTcpTick.cpp 

CPPFILES += ./rtcmsgs/RTMeetMsg.cpp \
									 ./rtcmsgs/RTMessage.cpp \
									 ./rtcmsgs/RTSignalMsg.cpp
			
CPPFILES += $(WEBRTC_BASE_PATH)/webrtc/base/asyncfile.cc \
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
	
all: librtcmsgclient-linux.a

librtcmsgclient-linux.a: $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ar -r librtcmsgclient-linux.a $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ranlib librtcmsgclient-linux.a

install: 
	install -m 664 librtcmsgclient-linux.a ./lib_linux_a/

clean:
	rm -f librtcmsgclient-linux.a *.o

.SUFFIXES: .cpp .cc .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c

