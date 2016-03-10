#include "ef_acceptor.h"
#include "ef_connection.h"
#include "ef_event_loop.h"
#include "ef_sock.h"
#include "ef_net_log.h"
#include <string.h>
#ifndef _WIN32
#include <errno.h>
#endif

namespace ef{


	Acceptor::Acceptor(ConnectionFactory *confac, ConnectionDispatcher* disp)
		:m_confac(confac), m_condispatch(disp)
	{
	}

	Acceptor::~Acceptor(){
		//if(m_confac)
		//	delete m_confac;
	}

	Connection* Acceptor::acceptConnect(EventLoop* l){
		struct sockaddr_in addr;
		socklen_t addrlen= sizeof(addr);

		memset(&addr, 0, sizeof(addr));
		Connection *con = NULL;

		while(1){
			SOCKET client = ::accept(getFd(), (sockaddr*)&addr, &addrlen);
			if(client < 0){
				if(errno != 11)
					NLogError << "addr[" << getIp() << ":" << getPort() 
						<< "] accept connect fail, errno:" << errno; 
				return NULL;
			}else{
				if(!m_confac || !m_condispatch){
					sock_close(client);
					NLogWarn << "addr[" << getIp() << ":" << getPort() 
						<< "] connection factory or dispatcher is NULL!"; 
					return NULL;
				}
				setSocketNonblocking(client);
				std::string addrstr = inet_ntoa(addr.sin_addr);
				con = m_confac->createConnection(l, client, 
					addrstr, htons(addr.sin_port));
				if(!con){
					sock_close(client);
					NLogWarn << "addr[" << getIp() << ":" << getPort() 
						<<"] get connection fail, from "
						<< inet_ntoa(addr.sin_addr) 
						<< ":" << htons(addr.sin_port); 
					return NULL;
				}

				con->setFd(client);
				con->setAddr(addrstr, htons(addr.sin_port));
				//print log at first, because the con maybe recycle at event loop	
				NLogTrace << "EventLoop:" << std::hex << l 
					<< std::dec << ", addr[" << getIp()
					<< ":" << getPort() <<"], accept new connection:" 
					<< std::hex << con << std::dec << ", id:" 
					<< con->getId() << ", fd:" << con->getFd() 
					<< ", from " << addrstr << ":" 
					<< htons(addr.sin_port);
				m_condispatch->dispatchConnection(l, con);

			}
		}

		return con;
	}

	int32 Acceptor::startListen(const char* addr, int32 port){
		SOCKET fd = tcpServerCreate((const char*)addr, port);
		if(fd < 0){
			NLogError << "Acceptor:" << std::hex << this
					<< std::dec 
					<<  " start_listen at[" << addr
					<< ":" << port << "] fail!";
			return -1;
		}
		setSocketNonblocking(fd);
		setAddr(addr, port);
		setFd(fd);
		return 0;
	}


	int32 Acceptor::setConnectionFactory(ConnectionFactory *confac){
		//if(m_confac){
		//	delete m_confac;
		//}
		m_confac = confac;
		return 0;
	}

	int32 Acceptor::setConnectionDispatcher(ConnectionDispatcher* condisp){
		//if(m_condispatch)
		//	delete m_condispatch;
		m_condispatch = condisp;
		return 0;
	}
	int32 Acceptor::handleRead(EventLoop* l){
		acceptConnect(l);
		return 0;
	}

};

