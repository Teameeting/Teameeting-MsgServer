#ifndef	EF_COMMON_H
#define	EF_COMMON_H

#include "base/ef_btype.h"

namespace ef{

	enum{
		READ_EVENT = 0x1,
		WRITE_EVENT= 0x2,
		TIMER_EVENT= 0x4,
	};


#ifdef PRINT_DETAIL_NET_LOG
#define DETAIL_NET_LOG 1
#else
#define DETAIL_NET_LOG 0
#endif
};

#endif
