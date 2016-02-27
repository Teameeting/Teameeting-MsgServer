C++ = $(CPLUS)
# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

NAME = RtComm
C++ = g++
CC = gcc
INCLUDE_FLAG = -include
LINK = $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) $(INCLUDE_FLAG) ../PlatformHeader.h -g -Wall

# OPTIMIZATION
CCFLAGS += -O2

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I.
CCFLAGS += -I..

C++FLAGS = $(CCFLAGS)

CFILES = base64.c	GetWord.c	Trim.c      md5.c	LinkedList.c

CPPFILES =	atomic.cpp \
		ConfParser.cpp \
		DateTranslator.cpp \
		EventContext.cpp \
		IdleTask.cpp \
		InternalStdLib.cpp \
		MyAssert.cpp \
		OS.cpp \
		OSCodeFragment.cpp \
		OSCond.cpp \
		OSFileSource.cpp \
		OSHeap.cpp \
		OSBufferPool.cpp \
		OSMutex.cpp \
		OSMutexRW.cpp \
		OSQueue.cpp \
		OSRef.cpp \
		OSThread.cpp \
		Socket.cpp \
		SocketUtils.cpp \
		ResizeableStringFormatter.cpp \
		StringFormatter.cpp \
		StringParser.cpp \
		StringTranslator.cpp \
		StrPtrLen.cpp \
		Task.cpp \
		TCPListenerSocket.cpp \
		TCPSocket.cpp \
		TimeoutTask.cpp \
		epev.cpp \
		md5digest.cpp

all: libRtComm.a

libRtComm.a: $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ar -r libRtComm.a $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)
	ranlib libRtComm.a

install: 
	install -m 664 libRtComm.a ../lib_a/

clean:
	rm -f libRtComm.a $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

.SUFFIXES: .cpp .c .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c

