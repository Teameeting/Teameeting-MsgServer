#include "ef_statistic.h"
#include "ef_tsd_ptr.h" 
#include <map>
#include <pthread.h>
#include <iostream>
#include <sstream>

namespace ef{


struct stat_t{
public:
	stat_t():us(0), count(0){
	}
	long long us;
	long	count;
};

long long time_diff_us(const timeval& n, const timeval& p){
	long long d_s = n.tv_sec - p.tv_sec;
	long long d_us = n.tv_usec - p.tv_usec;
	return d_s * 1000000 + d_us; 
}

class AutoLock{
public:
	AutoLock(pthread_mutex_t& m):m_mutex(m){
		pthread_mutex_lock(&m_mutex);
	}
	
	~AutoLock(){
		pthread_mutex_unlock(&m_mutex);
	}

private:
	pthread_mutex_t& m_mutex;
};

class GlobalStatistic{
public:
	int init(OUT_FUNC of, void* par, const int outspan = DEFAULT_OUT_SPAN){
		m_of = of;
		m_par = par;
		pthread_mutex_init(&m_mutex, NULL);
		m_out_span = outspan;
		m_flush_time.tv_sec = 0;
		m_flush_time.tv_usec = 0;
		return 0;
	}

	int uninit(){
		flush(true);
		pthread_mutex_destroy(&m_mutex);
		return 0;		
	}

	int add_one_record(const std::string& op, const stat_t& t){
		pthread_mutex_lock(&m_mutex);		
		stat_t& s = m_statis[op];
		s.us += t.us;
		s.count += t.count;
		pthread_mutex_unlock(&m_mutex);
		
		return 0;
	}

	int flush(bool force){
		timeval n;
		gettimeofday(&n, NULL);
		
		if(!force && time_diff_us(n, m_flush_time) < m_out_span){
			return 0;
		}

		std::stringstream os;
		{
			AutoLock l(m_mutex);
			if(!force && time_diff_us(n, m_flush_time) < m_out_span){
				return 0;
			}

			std::map<std::string, stat_t>::iterator itor = m_statis.begin();
			for(; itor != m_statis.end(); ++itor){
				os << "<" << itor->first << ":" << itor->second.us 
					<< "/" << itor->second.count << "=" 
					<< (itor->second.count ? 
					itor->second.us / itor->second.count : 0)
					<< "> ";
				itor->second.us = 0;
				itor->second.count = 0;
			}
			m_flush_time = n;
		}
		//do not lock  when write qlog
		if(m_of)
			m_of(m_par, os.str());

		return 0;	

	}
public:
	OUT_FUNC m_of;
	std::map<std::string, stat_t> m_statis;
	pthread_mutex_t m_mutex;
	timeval m_flush_time;
	long long m_out_span;
	void* m_par;
};

class ThreadStatistic{
public:
	ThreadStatistic(){
		m_flush_time.tv_sec = 0;
		m_flush_time.tv_usec = 0;
	}

	~ThreadStatistic(){
		flush(true);
	}

	int init(GlobalStatistic* gs, int outspan = DEFAULT_OUT_SPAN){
		m_gs = gs;
		m_out_span = outspan;
		return 0;
	}

	//stat_t& get_statis(const std::string& op){
	//	return m_statis[op];
	//}

	int add_one_record(const std::string& op, long long us){
		//stat_t& s = m_statis[op];
		//s.us += us;
		//++s.count;
		stat_t s;
		s.us += us;
		++s.count;
		m_gs->add_one_record(op, s);
		return 0;
	}

	int flush(bool force = false){
		timeval n;
		gettimeofday(&n, NULL);
		if(!force && time_diff_us(n, m_flush_time) < m_out_span){
			return 0;
		}

		//map<string, stat_t>::iterator itor = m_statis.begin();
		//for(; itor != m_statis.end(); ++itor){
		//	m_gs->add_one_record(itor->first, itor->second);
		//	itor->second.us = 0;
		//	itor->second.count = 0;
		//}
		m_flush_time = n;
		m_gs->flush(false);

		return 0;	
	}
private:
	GlobalStatistic* m_gs;
	//map<string, stat_t> m_statis;
	timeval m_flush_time;
	long long m_out_span;
};

GlobalStatistic g_s;
TSDPtr<ThreadStatistic> g_DbStat;

int initStatistic(OUT_FUNC of, void* par, int outspan){
	g_s.init(of, par, outspan);
	return 0;
}

TimeRecorder::TimeRecorder(const std::string& op):m_op(op){
	gettimeofday(&m_start, NULL);
}

TimeRecorder::~TimeRecorder(){
	timeval n;

	gettimeofday(&n, NULL);
	long long cost_us = time_diff_us(n, m_start);
	ThreadStatistic* ts = g_DbStat.get();

        if (!ts) {
                ts = new ThreadStatistic();
                g_DbStat.set(ts);
        }

	if(ts){
		ts->init(&g_s, g_s.m_out_span);
		ts->add_one_record(m_op, cost_us);	
		ts->flush();
	}
}



};
