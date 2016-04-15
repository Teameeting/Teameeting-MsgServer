#ifndef EF_NET_SETTINGS_H
#define EF_NET_SETTINGS_H

#include "ef_common.h"
namespace ef{

	enum{
		DEFAULT_EPOLL_SIZE = 65536,
		DEFAULT_PROC_EVENT_CNT = 16,
		MAX_PROC_EVENT_CNT = 1024,
		DEFAULT_PROC_TIMER_CNT = 32,
		MAX_PROC_TIMER_CNT = 2048,
	};

	class NetSettings{
	public:
	static	int32 epollSize;
	static	int32 procEventCnt;
	static	int32 procTimerCnt;
	};

}

#endif
