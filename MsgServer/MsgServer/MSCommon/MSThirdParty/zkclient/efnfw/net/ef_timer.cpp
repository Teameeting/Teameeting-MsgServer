#include "ef_timer.h"
#include "ef_connection.h"

namespace	ef{

	Timer::Timer(time_tv timeouttime)
		:m_timeouttime(timeouttime)
	{

	}

	Timer::~Timer(){

	}

	void Timer::setTimeOutAfter(int32 ms){
		timeval tv;
		gettimeofday(&tv, NULL);
		tv.tv_sec += ms / 1000;
		tv.tv_usec += ms % 1000 * 1000;
		tv.tv_sec += tv.tv_usec / 1000000;
		tv.tv_usec = tv.tv_usec % 1000000;
		setTimeoutTime(tv);
	}

	ConnectionTimer::ConnectionTimer(Connection* con, 
		int32 id)
		:m_con(con), m_id(id), m_status(STATUS_INIT){
	}

	int32 ConnectionTimer::timeout(EventLoop* l){
		m_status = STATUS_STOP;
		return m_con->handleTimer(l, m_id);
	}	

};

