# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

C++ 	= g++
LINK 	= $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) -g -Wall -std=c++11 -DPOSIX -std=c++11 -D__PTHREADS__ -D__PTHREADS_MUTEXES__

# OPTIMIZATION
CCFLAGS += -O0

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS

CCFLAGS += -I. -I./include
##CCFLAGS += -I/usr/local/include

C++FLAGS =  $(CCFLAGS)

CPPFILES := main.cpp test_client_cmd.cpp test_client_publish.cpp test_client_common.cpp

CPPFLAGS =-L./lib/ -lxredis
CPPFLAGS +=-L/usr/local/lib -lhiredis -ljsoncpp
CPPFLAGS +=-lpthread
	
all: xredis-client

xredis-client: $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)
	$(C++) -o xredis-client $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o) $(CPPFLAGS) $(C++FLAGS)

o_clean:
	rm -rf $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

clean:
	rm -rf xredis-client $(CPPFILES:.cpp=.o) $(CCFILES:.cc=.o)

.SUFFIXES: .cpp .cc .o

.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc
