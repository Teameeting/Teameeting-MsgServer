bin_PROGRAMS=mstest

AM_CFLAGS=-g -O0 -std=gnu++11 -Wall -fPIC -D__PTHREADS__ -D__PTHREADS_MUTEXES__
###AM_CFLAGS+=-DAUTO_RECONNECT
mstest_LDFLAGS=-static-libgcc

AM_CPPFLAGS=$(AM_CFLAGS)

AM_CPPFLAGS+=-I$(top_srcdir)/

AM_CPPFLAGS+= -I$(top_srcdir)/../MSCommon/ \
		   -I$(top_srcdir)/../MSCommon/MSBase/ \
		   -I$(top_srcdir)/../MSCommon/MSCommon/ \
		   -I$(top_srcdir)/../MSCommon/MSHttp/ \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/ \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/autoref/ \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/hiredis/ \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/libhttp/ \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/RTComm/ \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/zkclient/common/include \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/zkclient/efnfw \
		   -I$(top_srcdir)/../MSCommon/MSThirdParty/rtklog/

mstest_SOURCES=$(top_srcdir)/TUtil.cpp \
						   $(top_srcdir)/TTask.cpp \
						   $(top_srcdir)/main.cpp

mstest_LDADD=-L$(top_srcdir)/../MSCommonLib/ -lMSCommon -lRtComm -lhiredis -lhttp -lrtklog -lzkclient
mstest_LDADD+=/usr/local/lib/libjsoncpp.a /usr/local/lib/libzookeeper_mt.a
mstest_LDADD+=-lpthread -lrt -ldl
