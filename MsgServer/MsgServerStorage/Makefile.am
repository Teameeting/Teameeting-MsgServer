bin_PROGRAMS=MsgServerStorage

AM_CFLAGS=-g -O0 -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
MsgServerStorage_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/SRTListener/ \
			 -I$(top_srcdir)/SRTTransfer/

AM_CPPFLAGS+= -I$(top_srcdir)/../ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSBase/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSProtocol/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/xredis/include \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/common/include \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/efnfw \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/rtklog/

MsgServerStorage_SOURCES=$(top_srcdir)/SRTListener/SRTStorageListener.cpp \
						   $(top_srcdir)/SRTTransfer/SRTTransferSession.cpp \
						   $(top_srcdir)/SRTStorageRedis.cpp \
						   $(top_srcdir)/SRTRedisGroup.cpp \
						   $(top_srcdir)/SRTRedisManager.cpp \
						   $(top_srcdir)/SRTResponseSender.cpp \
						   $(top_srcdir)/SRTStorageManager.cpp \
						   $(top_srcdir)/SRTStorage.cpp \
						   $(top_srcdir)/main.cpp

MsgServerStorage_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lxredis -lhiredis -lhttp -lrtklog -lzkclient
MsgServerStorage_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a /usr/local/lib/libprotobuf-lite.a
MsgServerStorage_LDADD+=-lpthread -lrt -ldl

