#ifndef __EF_CONNECTION_H__
#define __EF_CONNECTION_H__

#include <map>
#include <string>
#include "ef_device.h"
#include "base/ef_loop_buf.h"

namespace ef{

	class EventLoop;
	class Message;
	class ConnectionTimer;

	class Connection:public Device{
	public:
		enum{
			FLAG_NULL = 0,
			FLAG_SAFE_CLOSE = 1,
		};

		Connection();
		virtual ~Connection();

		void setEventLoop(EventLoop* l){
			m_evlp = l;
		}

		EventLoop* getEventLoop(){
			return m_evlp;
		}


		int32 startTimer(int32 id, int32 timeout_ms);
		int32 stopTimer(int32 id);

		//close after all send buf has send
		int32 safeClose();


		virtual int32 handleRead(EventLoop* l);
		virtual int32 handleWrite(EventLoop* l);

		virtual int32 handleTimer(EventLoop* l, int32 id);
		virtual int32 handlePacket(const std::string& req) = 0;
		// < 0, error, = 0, wait more, > 0, recv whole pack
		virtual int32 checkPackLen() = 0;
		virtual int32 onCreate(EventLoop* l) = 0;

		//peek from recv buf
		int32 peekBuf(char* out, int32 outlen) const{
			return m_buf.peek((uint8*)out, outlen);
		}
		//read from recv buf
		int32 readBuf(char* out, int32 outlen){
			return m_buf.read((uint8*)out, outlen);
		}
		//recv buf len
		int32 bufLen() const{
			return m_buf.size();
		}

		void clearBuf(){
			m_buf.clear();
		}

		void clearSendQueue(){
			m_send_buf.clear();
		}

		int32 sendQueueSize(){
			return m_send_buf.size();
		}

		void setId(int32 id){
			m_id = id;
		}

		int32 getId() const{
			return m_id;
		}

		int32 sendMessage(const std::string &msg);


	private:
		friend class ConnectionTimer;
		ConnectionTimer* getTimer(int32 id);
		int32 clearTimer();
		int32 doSend();
		int32 send_(const char* buf, int32 len);
		int32 clear();

		int32  m_id;

		LoopBuf m_buf;
		std::map<int32, ConnectionTimer*> m_timers;
		LoopBuf m_send_buf;

		int32 m_flag;
		EventLoop* m_evlp;
	};


};

#endif

