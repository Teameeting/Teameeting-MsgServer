#include "ef_client.h"
#include "ef_sock.h"
#include "ef_server.h" 
#include "ef_net_log.h"
#include <cassert>

namespace ef{


Client::Client():
	m_status(STATUS_INIT),
	m_reconnect_span(0),
	m_keep_alive_span(0)
{
}


Client::~Client(){
		
}

int32 Client::doConnect(const std::string& addr, int32 port){
	int32  consuc = 0;
	SOCKET fd = tcpNbConnect(addr.data(), port, NULL, 0, &consuc);
	if(fd == EF_INVALID_SOCKET){
		NLogError << "Client:" << std::hex << (int64)this
			<< " tcpNbConnect to, addr:" << addr
			<< ", port:" << std::dec << port << " fail";
		return -1;
	}
	setFd(fd);
	m_status = STATUS_CONNECTING;
	return 0;
}

int32 Client::connectTo(const std::string& addr, int32 port){
	m_addr = addr;
	m_port = port;
	return doConnect(addr, port);
}

int32 Client::reconnect(){
	int32 ret = 0;
	if(m_status == STATUS_INIT){
		ret = connectTo(m_addr, m_port);
		addNotify(getEventLoop(), READ_EVENT | WRITE_EVENT);
	}
	return ret;
}

int32 Client::onCreate(EventLoop* l){
	addNotify(l, READ_EVENT | WRITE_EVENT);
	if(m_reconnect_span)
		startTimer(RECONNECT_TIMER, m_reconnect_span);
	if(m_keep_alive_span)
		startTimer(KEEPALIVE_TIMER, m_keep_alive_span);
	return 0;
}

void Client::setReconnectSpan(int32 span){
	m_reconnect_span = span;
}

void Client::setKeepAliveSpan(int32 span){
	m_keep_alive_span = span;
}

int32 Client::handleTimer(EventLoop* l, int32 id){
	int32 ret = 0;
	switch(id){
	case RECONNECT_TIMER:
		startTimer(RECONNECT_TIMER, m_reconnect_span);
		ret = reconnect();
		break;
	case KEEPALIVE_TIMER:
		startTimer(KEEPALIVE_TIMER, m_keep_alive_span);
		if(m_status == STATUS_CONNECTED){
			ret = keepAlive();
		}
		break;
	}
	return ret;
}

int32 Client::onFail(){
	disconnect();
	if(!m_reconnect_span)
		recycle();
	return 0;	
}

int32 Client::disconnect(){
	onDisconnected();
	int32 ret = clearNotify(getEventLoop());
	closesocket(getFd());
	setFd(EF_INVALID_SOCKET);
	clearBuf();
	clearSendQueue();
	m_status = STATUS_INIT;
	return ret;
}

int32 Client::handleConnected(EventLoop* l){
	//check if connect success
	//if success, set status
	//else reconnect
	int32 ret = 0;
	int32 error_value = 0;
	socklen_t error_value_len = sizeof(error_value);
	ret = getsockopt(getFd(), SOL_SOCKET, SO_ERROR, 
		&error_value, &error_value_len );
	if(ret < 0){
		disconnect();
	}else{
		m_status = STATUS_CONNECTED;
		modifyNotify(getEventLoop(), READ_EVENT);
		ret = onConnected();
	}
	return ret;
}

int32 Client::handleWrite(EventLoop* l){
	int32 ret = 0;
	switch(m_status){
	case STATUS_CONNECTING:
		ret = handleConnected(l);
		break;
	case STATUS_CONNECTED:
		ret = Connection::handleWrite(l);
		break; 
	}
	return ret;
}


int32 Client::handleRead(EventLoop* l){
	int32 ret = 0;
	switch(m_status){
	case STATUS_CONNECTING:
		ret = handleConnected(l);
		break;
	case STATUS_CONNECTED:
		ret = Connection::handleRead(l);
		break; 
	}
	return ret;
}

int32 Client::dispatchToServer(Server* s){
	int32 i = getId();
	EventLoop& lp = s->getEventLoop(i 
		% s->getEventLoopCount());
	lp.asynAddConnection(this);
	return 0;	
}

}
