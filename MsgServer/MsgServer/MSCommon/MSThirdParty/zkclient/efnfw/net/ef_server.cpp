#include "ef_server.h"
#include "ef_net_log.h"
#include "cassert"

namespace ef{

int32 GDispatcher::dispatchConnection(EventLoop*l, Connection* c){
	static volatile int32 i = 0;
	atomicIncrement32(&i);
	EventLoop& lp = m_s->getEventLoop(i 
		% m_s->getEventLoopCount());
	lp.asynAddConnection(c);
	return 0;		
}

int32 Server::setEventLoopCount(int32 cnt){
	if(cnt <= 0)
		cnt = 1;
	m_thread_cnt = cnt;

	m_nthreads.reset(new EventLoop[m_thread_cnt]);
	if(!m_nthreads.get()){
		NLogError << "init new EventLoop fail!";
		return -1;
	}
	m_nthread_ids.reset(new THREADHANDLE[m_thread_cnt]);

	return 0;

}

int32 Server::init(){
	int32 ret = 0;
	ret = m_athread.init();
	if(ret < 0){
		goto error;
	}

	for(int32 i = 0; i < m_thread_cnt; ++i){
		ret = m_nthreads[i].init();
		if(ret < 0){
			goto error;
		}
		m_nthreads[i].setId(i);
	}
	goto exit;
error:
exit:
	return ret;
	
}

int32 Server::clear(){
	return 0;
}


Server::~Server(){
	delete m_default_disp;
}

int32 Server::startListen(int32 port, ConnectionFactory* confac, 
	ConnectionDispatcher* d){
	int32	ret = 0;
	if(!d)
		d = m_default_disp;
	RefCntPtr<Acceptor> a(new Acceptor(confac, d));	
	ret = a->startListen("0.0.0.0", port);
	if(ret < 0){
		NLogError << "startListen fail!";
		return	-1;
	}
	NLogError << "startListen at port:" << port;
	a->addNotify(&m_athread, READ_EVENT);
	m_acceptors[port] = a;

	return	ret;
}

int32 Server::stopListen(int32 port){
	RefCntPtr<Acceptor> a = m_acceptors[port];
	if(a.get()){
		a->clearNotify(&m_athread);
	}
	m_acceptors.erase(port);
	NLogError << "startListen at port:" << port;
	return 0;
}

static void* threadProcess(void *param){
	EventLoop *thr = (EventLoop *)param;
	assert(thr);
	thr->run();

	return  0;
}

int32 Server::run(){
	int     ret = 0;
	threadCreate(&m_athreadid, NULL, threadProcess, &m_athread);
	if(ret < 0){
		return ret;
	}
	for(int i = 0; i < m_thread_cnt; ++i){
		threadCreate(&m_nthread_ids[i],
			NULL, threadProcess, &m_nthreads[i]);    
	}

	return ret;
}

int32 Server::stop(){
	m_athread.stop();
	threadJoin(&m_athreadid);
	for(int i = 0; i < m_thread_cnt; ++i){
		m_nthreads[i].stop();
	}
	for(int j = 0; j < m_thread_cnt; ++j){
		threadJoin(&m_nthread_ids[j]);
	}
	return 0;
}



}
