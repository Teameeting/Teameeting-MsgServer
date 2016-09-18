bin_PROGRAMS=SeqnClient

AM_CFLAGS=-g -O0 -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
SeqnClient_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/ClientSession/

AM_CPPFLAGS+= -I$(top_srcdir)/../ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSBase/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/common/include \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/efnfw \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/rtklog/

AM_CPPFLAGS+= -I$(top_srcdir)/../client_common \
			  -I$(top_srcdir)/../client_common/core

SeqnClient_SOURCES=$(top_srcdir)/ClientSession/ClientSession.cpp \
						   $(top_srcdir)/ClientManager.cpp \
						   $(top_srcdir)/SeqnClient.cpp \
						   $(top_srcdir)/main.cpp

SeqnClient_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lhiredis -lhttp -lrtklog -lzkclient
SeqnClient_LDADD+=-L$(top_srcdir)/../client_common/ -lclient_common
SeqnClient_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a /usr/local/lib/libprotobuf-lite.a
SeqnClient_LDADD+=-lpthread -lrt -ldl

