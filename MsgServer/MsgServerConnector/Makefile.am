bin_PROGRAMS=MsgServerConnector

AM_CFLAGS=-g -O0 -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
MsgServerConnector_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/CRTConnection/ \
			 -I$(top_srcdir)/CRTListener/ \
			 -I$(top_srcdir)/CRTTransfer/

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

MsgServerConnector_SOURCES=$(top_srcdir)/CRTConnection/CRTConnHttp.cpp \
						   $(top_srcdir)/CRTConnection/CRTConnTcp.cpp \
						   $(top_srcdir)/CRTConnection/CRTConnection.cpp \
						   $(top_srcdir)/CRTConnection/CRTConnectionTcp.cpp \
						   $(top_srcdir)/CRTListener/CRTConnListener.cpp \
						   $(top_srcdir)/CRTListener/CRTConnTcpListener.cpp \
						   $(top_srcdir)/CRTListener/CRTModuleListener.cpp \
						   $(top_srcdir)/CRTTransfer/CRTTransferSession.cpp \
						   $(top_srcdir)/CRTConnManager.cpp \
						   $(top_srcdir)/CRTConnector.cpp \
						   $(top_srcdir)/CRTDispatchConnection.cpp \
						   $(top_srcdir)/main.cpp

MsgServerConnector_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lhiredis -lhttp -lrtklog -lzkclient
MsgServerConnector_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a /usr/local/lib/libprotobuf-lite.a
MsgServerConnector_LDADD+=-lpthread -lrt -ldl

