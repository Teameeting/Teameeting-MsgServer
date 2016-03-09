#ifndef __EF_TIMER_H__
#define __EF_TIMER_H__

#include "ef_common.h"
#include "ef_sock.h"

namespace ef{

	class Connection;
	class EventLoop;

	struct time_tv{
	time_tv(long sec = 0,long usec = 0)
		:m_sec(sec), m_usec(usec){

		}

	time_tv(struct timeval tv)
		:m_sec(tv.tv_sec), m_usec(tv.tv_usec){
		}

	operator struct timeval(){
		timeval tv;
		tv.tv_sec = m_sec;
		tv.tv_usec = m_usec;
		return tv;
	}

	bool operator < (const time_tv& t) const {
		if(this == &t)
			return false;
		if (m_sec < t.m_sec) {
			return true;
		} else if (m_sec > t.m_sec) {
			return false;
		}
		return m_usec < t.m_usec;
	}

	bool operator== (const time_tv& t) const {
		if(this == &t)
			return true;
		return m_sec == t.m_sec && m_usec == t.m_usec;
	}

	bool operator > (const time_tv& t) const {
		if(this == &t)
			return false;
		return t < *this;
	}

	long m_sec;
	long m_usec;

	};

	class Timer{
	public:
		Timer(time_tv timouttime = time_tv());

		void setTimeoutTime(const time_tv& t){
			m_timeouttime = t;
		}

		void setTimeOutAfter(int32 ms);

		virtual ~Timer();

		virtual int32 timeout(EventLoop* l){
			return 0;
		}

		time_tv getTimeoutTime() const {
			return m_timeouttime;
		}


		bool operator < (const Timer& t) const {
			if(this == &t)
				return false;
			if(m_timeouttime < t.m_timeouttime){
				return true;
			}
			if(m_timeouttime > t.m_timeouttime){
				return false;
			}

			return (int64)this < (int64)&t; 
		}


	private:
		time_tv  m_timeouttime;
	};

	class ConnectionTimer: public Timer{
	public:
		static const int32 STATUS_INIT = 0;
		static const int32 STATUS_START = 1;
		static const int32 STATUS_STOP = 2;

		ConnectionTimer(Connection* con, int32 id);

		virtual ~ConnectionTimer(){
		}

		virtual int32 timeout(EventLoop* l);

		int32 status() const{
			return m_status;
		} 
		
		void setStatus(int32 status){
			m_status = status;
		}

	private:
		Connection* m_con;
		int32 m_id;
		int32 m_status;	
	};

};

#endif/*EF_TIMER_H*/

