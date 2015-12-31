C++ = $(CPLUS)
# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

NAME = rtcmsgclient-linux
C++ = g++
CC = gcc
###INCLUDE_FLAG = -include
INCLUDE_FLAG = 
LINK = $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) $(INCLUDE_FLAG) -g -Wall -std=c++0x -DPOSIX -DWEBRTC_LINUX -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS

# OPTIMIZATION
CCFLAGS += -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.
CCFLAGS += -I./rtcmsgs -I./webrtc -I./webrtc/base

C++FLAGS = $(CCFLAGS)

CFILES = 

CPPFILES := XMsgProcesser.cpp \
			XMsgClient.cpp \
			XTcpClientImpl.cpp \
			XTcpTick.cpp 

CPPFILES += ./rtcmsgs/RTMeetMsg.cpp \
									 ./rtcmsgs/RTMessage.cpp \
									 ./rtcmsgs/RTSignalMsg.cpp
			
CPPFILES += ./webrtc/base/asyncfile.cc \
				./webrtc/base/asyncresolverinterface.cc \
				./webrtc/base/asyncsocket.cc \
				./webrtc/base/asynctcpsocket.cc \
				./webrtc/base/asyncudpsocket.cc \
				./webrtc/base/base64.cc \
				./webrtc/base/checks.cc \
				./webrtc/base/criticalsection.cc \
				./webrtc/base/event.cc \
				./webrtc/base/event_tracer.cc \
				./webrtc/base/ifaddrs-android.cc \
				./webrtc/base/ipaddress.cc \
				./webrtc/base/logging.cc \
				./webrtc/base/messagehandler.cc \
				./webrtc/base/messagequeue.cc \
				./webrtc/base/nethelpers.cc \
				./webrtc/base/physicalsocketserver.cc \
				./webrtc/base/platform_thread.cc \
				./webrtc/base/signalthread.cc \
				./webrtc/base/sigslot.cc \
				./webrtc/base/socketaddress.cc \
				./webrtc/base/stringencode.cc \
				./webrtc/base/thread.cc \
				./webrtc/base/timeutils.cc \
	
all: librtcmsgclient-linux.a

librtcmsgclient-linux.a: $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ar -r librtcmsgclient-linux.a $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ranlib librtcmsgclient-linux.a

install: 
	install -m 664 librtcmsgclient-linux.a ./lib_linux_a/

clean:
	rm -f librtcmsgclient-linux.a $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.SUFFIXES: .cpp .cc .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c

