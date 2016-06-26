# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

C++ 	= g++
CC 		= gcc
LINK 	= $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) -g -Wall -std=c++11 -DPOSIX -DENABLE_DEBUG=1 -DWEBRTC_LINUX -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DNO_MAIN_THREAD_WRAPPING

# OPTIMIZATION
CCFLAGS += -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.
##CCFLAGS += -I/usr/local/include
CCFLAGS += -I../client_common -I../client_common/proto

C++FLAGS =  $(CCFLAGS)
C++FLAGS += -I./RTJson -I./RTHttp -I./RTManager -I./RTGroup

CFILES = 

CPPFILES := main.cpp \
			RTJson/RTJson.cpp \
			RTHttp/RTCurlClient.cpp \
			RTHttp/RTCURL.cpp \
			RTManager/RTConnClientManager.cpp \
			RTManager/RTMsgClient.cpp \
			RTManager/RTMsgThreadPool.cpp \
			RTManager/RTHttpClient.cpp \
			RTManager/RTMsgRoom.cpp \
			RTManager/RTClientTest.cpp \
			RTManager/RTGroupTest.cpp

CPPFILES += RTGroup/XGrpMsgClient.cpp \
			RTGroup/XGrpMsgProcesser.cpp

CPPFLAGS+=-lcurl
	
all: msgclient

msgclient: $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)
	$(C++) -o msgclient $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o) $(CPPFLAGS) $(C++FLAGS) -L../client_common -lclient_common

clean:
	rm -rf msgclient $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

.SUFFIXES: .cpp .cc .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c

