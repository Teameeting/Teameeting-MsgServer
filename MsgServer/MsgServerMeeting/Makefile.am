bin_PROGRAMS=MsgServerMeeting

AM_CFLAGS=-g -O0 -std=gnu++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
MsgServerMeeting_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/ \
			 -I$(top_srcdir)/MRTHttp/ \
			 -I$(top_srcdir)/MRTTransfer/

AM_CPPFLAGS+= -I$(top_srcdir)/../MsgServer/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSBase/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSCommon/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSProtocol/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/../MsgServer/MSCommon/MSThirdParty/rtklog/

MsgServerMeeting_SOURCES=$(top_srcdir)/MRTHttp/MRTConnHttp.cpp \
						   $(top_srcdir)/MRTHttp/MRTHttp.cpp \
						   $(top_srcdir)/MRTHttp/MRTHttpSender.cpp \
						   $(top_srcdir)/MRTHttp/MRTHttpSvrConn.cpp \
						   $(top_srcdir)/MRTTransfer/MRTTransfer.cpp \
						   $(top_srcdir)/MRTTransfer/MRTTransferSession.cpp \
						   $(top_srcdir)/MRTConnectionManager.cpp \
						   $(top_srcdir)/MRTMeeting.cpp \
						   $(top_srcdir)/MRTMeetingRoom.cpp \
						   $(top_srcdir)/MRTRoomManager.cpp \
						   $(top_srcdir)/main.cpp

MsgServerMeeting_LDADD=-L$(top_srcdir)/../MsgServer/MSCommonLib/ -lMSCommon -lRtComm -lhiredis -lhttp -lrtklog
MsgServerMeeting_LDADD+=-lpthread -lrt -ldl
