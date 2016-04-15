#ifndef __EF_CLIENT_H__
#define __EF_CLIENT_H__

#include "ef_connection.h"

namespace ef{

class Server;

class Client:public Connection{
public:
	enum{
		STATUS_INIT = 0,
		STATUS_CONNECTING = 1,
		STATUS_CONNECTED = 2,
	};

	enum{
		RECONNECT_TIMER = 101,
		KEEPALIVE_TIMER = 102,
	};

	Client();
	virtual ~Client();
	int32 connectTo(const std::string& addr, int32 port);
	//if the span is 0, recycle when disconenct
	void setReconnectSpan(int32 span_ms);
	void setKeepAliveSpan(int32 span_ms);
	int32 reconnectSpan() const{
		return m_reconnect_span;
	}
	int32 keepAliveSpan() const{
		return m_keep_alive_span;
	}
	int32 disconnect();

	int32 getStatus() const{
		return m_status;
	}

	virtual int32 onConnected() = 0;
	virtual int32 onDisconnected() = 0;
	virtual int32 checkPackLen() = 0;
	virtual int32 keepAlive() = 0;
	virtual int32 onFail();
	virtual int32 handlePacket(const std::string& p) = 0;
	
	//set id and dispatch to server,default maybe enough
	virtual int32 dispatchToServer(Server* s);
	virtual int32 onCreate(EventLoop* l);
	virtual int32 handleRead(EventLoop* l);
	virtual int32 handleWrite(EventLoop* l);
	virtual int32 handleTimer(EventLoop* l, int32 id);

private:
	int32 reconnect();
	int32 doConnect(const std::string& addr, int32 port);
	int32 handleConnected(EventLoop* l);

	int32 m_status; 
	std::string m_addr;
	int32 m_port;
	int32 m_reconnect_span;
	int32 m_keep_alive_span;
};


};

#endif
