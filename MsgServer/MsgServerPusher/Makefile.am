bin_PROGRAMS=MsgServerPusher

AM_CFLAGS=-g -O0 -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
MsgServerPusher_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/PRTListener/ \
			 -I$(top_srcdir)/PRTTransfer/ \
			 -I$(top_srcdir)/PRTXRedis/ \
			 -I$(top_srcdir)/Ios/

AM_CPPFLAGS+= -I$(top_srcdir)/../ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSBase/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/xredis/src \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/common/include \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/zkclient/efnfw \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/rtklog/

MsgServerPusher_SOURCES=$(top_srcdir)/PRTListener/PRTPusherListener.cpp \
						   $(top_srcdir)/PRTTransfer/PRTTransferSession.cpp \
						   $(top_srcdir)/PRTTransfer/XPushMsgProcesser.cpp \
						   $(top_srcdir)/PRTXRedis/PRTApnsPusher.cpp \
						   $(top_srcdir)/PRTXRedis/RCPthread.cpp \
						   $(top_srcdir)/Ios/IosPusher.cpp \
						   $(top_srcdir)/PRTXRedis.cpp \
						   $(top_srcdir)/PRTConnManager.cpp \
						   $(top_srcdir)/PRTConnDispatcher.cpp \
						   $(top_srcdir)/PRTPusherManager.cpp \
						   $(top_srcdir)/PRTPusher.cpp \
						   $(top_srcdir)/main.cpp

MsgServerPusher_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lxredis -lhiredis -lhttp -lrtklog -lzkclient
MsgServerPusher_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a /usr/local/lib/libprotobuf-lite.a
MsgServerPusher_LDADD+=-lpthread -lrt -ldl
MsgServerPusher_LDADD+=-L/usr/lib64/capn/ -lcapn

