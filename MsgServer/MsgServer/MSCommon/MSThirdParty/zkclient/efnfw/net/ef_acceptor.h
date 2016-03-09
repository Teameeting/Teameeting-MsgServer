#ifndef __EF_ACCEPTOR_H__
#define __EF_ACCEPTOR_H__


#include "ef_common.h"
#include "ef_sock.h"
#include "ef_device.h"

namespace ef{
	class EventLoop;
	class Connection;

	class ConnectionFactory{
	public:
		virtual ~ConnectionFactory(){};
		virtual Connection* createConnection(EventLoop* l,
			int32 fd, const std::string& addr, int32 port) = 0;
	};

	
	class ConnectionDispatcher{
	public: 
		virtual ~ConnectionDispatcher(){}
		virtual int32 dispatchConnection(EventLoop* l, Connection* c) = 0;
	};

	class Acceptor:public Device{
	public:
		Acceptor(ConnectionFactory *confac = NULL,
			ConnectionDispatcher *dis = NULL);
		int32 setConnectionFactory(ConnectionFactory *confac);
		int32 setConnectionDispatcher(ConnectionDispatcher* condisp);
		virtual ~Acceptor();

		int32 startListen(const char *addr, int32 port);
		int32 stopListen();

		virtual Connection* acceptConnect(EventLoop* l);
		virtual int32 handleRead(EventLoop* l);
	private:
		ConnectionFactory* m_confac;
		ConnectionDispatcher* m_condispatch;
	};
};

#endif
