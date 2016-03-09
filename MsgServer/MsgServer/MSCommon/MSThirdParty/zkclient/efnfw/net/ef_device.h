#ifndef __EF_DEVICE_H__
#define __EF_DEVICE_H__

#include <string>
#include "ef_common.h"
#include "ef_sock.h"

namespace ef{

	class EventLoop;

	class Device{
	public:
		Device();
		virtual ~Device();



		virtual int32 handleRead(EventLoop* l){
			return 0;
		}
		virtual int32 handleWrite(EventLoop* l){
			return 0;
		}
		virtual int32 recycle();
		
		virtual int32 onFail(){
			return recycle();
		}

		void setFd(SOCKET fd){
			m_fd = fd;
		}

		SOCKET getFd() const{
			return m_fd;
		}

		int32 addNotify(EventLoop* l, int32 ev_type);
		int32 clearNotify(EventLoop* l);
		int32 modifyNotify(EventLoop* l, int32 ev_type);
		int32 getNotify() const{
			return m_noti;
		}

		int32 shutDownRead(){
			return shutdown(m_fd, 0);
		}

		int32 shutDownWrite(){
			return shutdown(m_fd, 1);
		}

		int32 setAddr(const std::string& addr, int32 port){
			m_addr = addr;
			m_port = port;
			return 0;
		}

		int32 getAddr(std::string& addr, int32 &port) const{
			addr = m_addr;
			port = m_port;
			return 0;
		}

		const std::string& getIp() const{
			return m_addr;
		}

		int32 getPort() const{
			return m_port;
		}

	private:
		int32 clear();
		SOCKET  m_fd;
		std::string m_addr;
		int32 m_port;
		int32 m_noti;
	};



}


#endif/*EF_DEVICE_H*/
