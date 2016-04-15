#include "ef_net_settings.h"

namespace ef{

int32 NetSettings::epollSize = DEFAULT_EPOLL_SIZE;
int32 NetSettings::procEventCnt = DEFAULT_PROC_EVENT_CNT;
int32 NetSettings::procTimerCnt = DEFAULT_PROC_EVENT_CNT;

};
