bin_PROGRAMS=MsgServerRTLive

AM_CFLAGS=-g -O0 -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
MsgServerRTLive_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/LRTListener/ \
			 -I$(top_srcdir)/LRTTransfer/

AM_CPPFLAGS+= -I$(top_srcdir)/../ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSBase/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/xredis/include \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/common/include \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/efnfw \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/rtklog/

MsgServerRTLive_SOURCES=$(top_srcdir)/LRTListener/LRTRTLiveListener.cpp \
						   $(top_srcdir)/LRTListener/LRTGroupListener.cpp \
						   $(top_srcdir)/LRTTransfer/LRTTransferSession.cpp \
						   $(top_srcdir)/LRTTransfer/LRTGroupSession.cpp \
						   $(top_srcdir)/LRTGrpConnTcp.cpp \
						   $(top_srcdir)/LRTConnManager.cpp \
						   $(top_srcdir)/LRTConnDispatcher.cpp \
						   $(top_srcdir)/LRTRTLiveManager.cpp \
						   $(top_srcdir)/LRTRTLive.cpp \
						   $(top_srcdir)/main.cpp

MsgServerRTLive_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lxredis -lhiredis -lhttp -lrtklog -lzkclient
MsgServerRTLive_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a /usr/local/lib/libprotobuf-lite.a
MsgServerRTLive_LDADD+=-lpthread -lrt -ldl

