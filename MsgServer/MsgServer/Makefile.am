noinst_LIBRARIES=libMSCommon.a

AM_CFLAGS=-g -std=c++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+= -I$(top_srcdir)/MSCommon/MSBase/ \
		   -I$(top_srcdir)/MSCommon/MSCommon/ \
		   -I$(top_srcdir)/MSCommon/MSProtocol/ \
		   -I$(top_srcdir)/MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/MSCommon/MSThirdParty/rtklog/

noinst_HEADERS=$(top_srcdir)/MSCommon/PlatformHeader.h \
			   $(top_srcdir)/MSCommon/revision.h \
			   $(top_srcdir)/MSCommon/sigslot.h

libMSCommon_a_SOURCES=$(top_srcdir)/MSCommon/sigslot.cc \
				$(top_srcdir)/MSCommon/MSBase/RTHiredis.cpp \
				$(top_srcdir)/MSCommon/MSBase/RTTcp.cpp \
				$(top_srcdir)/MSCommon/MSProtocol/RTHttpMsg.cpp \
				$(top_srcdir)/MSCommon/MSProtocol/RTMeetMsg.cpp \
				$(top_srcdir)/MSCommon/MSProtocol/RTMessage.cpp \
				$(top_srcdir)/MSCommon/MSProtocol/RTSignalMsg.cpp

