# Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
#  

NAME = rtklog
C++ = g++
CC = gcc
INCLUDE_FLAG = -include
INCPATH = -I./
LINK = $(LINKER)
CCFLAGS += $(COMPILER_FLAGS) -g -Wall -fPIC

# OPTIMIZATION
CCFLAGS += -O3

# EACH DIRECTORY WITH HEADERS MUST BE APPENDED IN THIS MANNER TO THE CCFLAGS
CCFLAGS += $(INCPATH)

CCFLAGS += -DLOGGING=1 -D__avc_log__ -DNO_MAIN_THREAD_WRAPPING -DHASH_NAMESPACE=__gnu_cxx -DPOSIX -DDISABLE_DYNAMIC_CAST -DOS_LINUX=OS_LINUX -DLINUX -D_DEBUG 

C++FLAGS = $(CCFLAGS)

SRC_DIR = ./
SRC_BASE_DIR = ./talk/base

CFILES = \
	
CCFILES = $(SRC_DIR)/rtklog.cc \
		$(SRC_BASE_DIR)/common.cc \
		$(SRC_BASE_DIR)/event.cc \
		$(SRC_BASE_DIR)/fileutils.cc \
		$(SRC_BASE_DIR)/logging.cc \
		$(SRC_BASE_DIR)/messagehandler.cc \
		$(SRC_BASE_DIR)/messagequeue.cc \
		$(SRC_BASE_DIR)/pathutils.cc \
		$(SRC_BASE_DIR)/stream.cc \
		$(SRC_BASE_DIR)/stringencode.cc \
		$(SRC_BASE_DIR)/stringutils.cc \
		$(SRC_BASE_DIR)/thread.cc \
		$(SRC_BASE_DIR)/timeutils.cc \
		$(SRC_BASE_DIR)/timing.cc \
		$(SRC_BASE_DIR)/unixfilesystem.cc \
		$(SRC_BASE_DIR)/urlencode.cc 


all: librtklog.a

librtklog.a: $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)
	ar -r librtklog.a $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)
	ranlib librtklog.a

install: 
	install -m 664 librtklog.a ../lib_a/
	
clean:
	rm -f librtklog.a $(CFILES:.c=.o) $(CCFILES:.cc=.o) $(CPPFILES:.cpp=.o)

.SUFFIXES: .cc .cpp .c .o

.cc.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cc
	
.cpp.o:
	$(C++) -c -o $*.o $(DEFINES) $(C++FLAGS) $*.cpp

.c.o:
	$(CC) -c -o $*.o $(DEFINES) $(CCFLAGS) $*.c
