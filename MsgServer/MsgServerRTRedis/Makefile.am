bin_PROGRAMS=MsgServerRTRedis

AM_CFLAGS=-g -O0 -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
MsgServerRTRedis_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/RRTListener/ \
			 -I$(top_srcdir)/RRTTransfer/

AM_CPPFLAGS+= -I$(top_srcdir)/../ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSBase/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSProtocol/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/common/include \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/efnfw \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/rtklog/

MsgServerRTRedis_SOURCES=$(top_srcdir)/RRTListener/RRTRTRedisListener.cpp \
						   $(top_srcdir)/RRTTransfer/RRTTransferSession.cpp \
						   $(top_srcdir)/RRTRTRedisManager.cpp \
						   $(top_srcdir)/RRTRTRedis.cpp \
						   $(top_srcdir)/main.cpp

MsgServerRTRedis_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lhiredis -lhttp -lrtklog -lzkclient
MsgServerRTRedis_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a /usr/local/lib/libprotobuf-lite.a
MsgServerRTRedis_LDADD+=-lpthread -lrt -ldl

