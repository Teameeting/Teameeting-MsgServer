bin_PROGRAMS=MsgServerSequence

AM_CFLAGS=-g -O0 -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
MsgServerSequence_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/SRTListener/ \
			 -I$(top_srcdir)/SRTTransfer/

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

MsgServerSequence_SOURCES=$(top_srcdir)/SRTListener/SRTSequenceListener.cpp \
						   $(top_srcdir)/SRTTransfer/SRTTransferSession.cpp \
						   $(top_srcdir)/SRTSequenceRedis.cpp \
						   $(top_srcdir)/SRTResponseCollection.cpp \
						   $(top_srcdir)/SRTConnManager.cpp \
						   $(top_srcdir)/SRTConnDispatcher.cpp \
						   $(top_srcdir)/SRTRedisManager.cpp \
						   $(top_srcdir)/SRTSequenceGenerator.cpp \
						   $(top_srcdir)/SRTSequenceManager.cpp \
						   $(top_srcdir)/SRTSequence.cpp \
						   $(top_srcdir)/main.cpp

MsgServerSequence_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lxredis -lhiredis -lhttp -lrtklog -lzkclient
MsgServerSequence_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a /usr/local/lib/libprotobuf-lite.a
MsgServerSequence_LDADD+=-lpthread -lrt -ldl

