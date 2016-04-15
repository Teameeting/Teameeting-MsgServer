#include "ef_device.h"
#include <cassert>
#include <errno.h>
#include "ef_event_loop.h"
#include "ef_net_log.h"
#include "base/ef_utility.h"


namespace ef{

Device::Device()
	:m_fd(EF_INVALID_SOCKET), m_noti(0)
{

}

Device::~Device(){
	clear();
}

int32	Device::recycle(){
	delete 	this;
	return	0;
}

int32	Device::clear(){

	if(m_fd != EF_INVALID_SOCKET){
		//NLogInfo << "close fd:" << m_fd;
		sock_close(m_fd);
		m_fd = EF_INVALID_SOCKET;
		
	}

	return	0;
}

	
int32	Device::addNotify(EventLoop* l, int32 noti){
	int32	ret = 0;

	ret = l->addNotify(this, noti);
	if(ret < 0){
		return	ret;
	}
	m_noti = noti;
	return ret;
}

	
int32	Device::clearNotify(EventLoop* l){
	int32	ret = 0;

	ret = l->clearNotify(this);
	if(ret < 0){
		return	ret;
	}
	m_noti = 0;
	return ret;
}

	
int32	Device::modifyNotify(EventLoop* l, int32 noti){
	int32	ret = 0;

	ret = l->modifyNotify(this, noti);
	if(ret < 0){
		return	ret;
	}
	m_noti = noti;
	return ret;
}


};


