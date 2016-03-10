#ifndef __EF_SERVER_H__
#define __EF_SERVER_H__


#include <map>
#include "ef_connection.h"
#include "ef_event_loop.h"
#include "ef_acceptor.h"
#include "base/ef_auto_ptr.h"


namespace ef{

class Server;

class GDispatcher:public ConnectionDispatcher{
public:
	GDispatcher(Server* s):m_s(s){}
	int32 dispatchConnection(EventLoop*l, Connection* c);
private:
	Server* m_s;
};


class Server{
public:
	enum{
		DEFAULT_THREAD_COUNT = 64,
	};

	Server(int32 thread_count = DEFAULT_THREAD_COUNT)
		:m_thread_cnt(thread_count), 
		m_default_disp(new GDispatcher(this))
		{
	}

	virtual ~Server();


	int32 setEventLoopCount(int32 cnt);


	int32 getEventLoopCount(){
		return m_thread_cnt;
	}

	EventLoop& getEventLoop(int32 idx){
		return m_nthreads[idx];
	}
	
	EventLoop& getAcceptEventLoop(){
		return m_athread;
	}

	int32 init();
	int32 startListen(int32 port, ConnectionFactory*, 
		ConnectionDispatcher* d = NULL);
	int32 stopListen(int32 port);
	int32 run();
	int32 stop();
	int32 clear();

private:
	int32 m_thread_cnt;
	std::map<int32, RefCntPtr<Acceptor> > m_acceptors;
	EventLoop m_athread;
	THREADHANDLE m_athreadid; 
	ArrayRefCntPtr<EventLoop> m_nthreads; 
	ArrayRefCntPtr<THREADHANDLE> m_nthread_ids; 
	ConnectionDispatcher* m_default_disp;
};



}

#endif
