#ifndef EF_NET_THREAD_H
#define EF_NET_THREAD_H

#include <map>
#include <set>
#include <list>
#include <string>
#include <iostream>
#include "ef_common.h"
#include "ef_sock.h"
#include "ef_timer.h"
#include "base/ef_thread.h"
#include "base/ef_atomic.h"
#include "base/ef_loop_buf.h"

namespace ef{

	class Device;
	class Connection;
	class Acceptor;
	class NetOperator;
	class EventLoop;

	typedef int32 (*OBJ_INIT)(EventLoop* l, void* obj);
	typedef int32 (*OBJ_CLEAN)(EventLoop* l, void* obj);


	class EventLoop{
	public:
		EventLoop();

		virtual ~EventLoop();

		int32 addNotify(Device *con, int32 noti);
		int32 clearNotify(Device *con);
		int32 modifyNotify(Device *con, int32 noti);

		int32 asynAddConnection(Connection *con);
		int32 asynCloseConnection(int32 id);
		int32 asynCloseAllConnections();
		int32 asynSendMessage(int32 id, const std::string &msg);
		int32 addAsynOperator(NetOperator* op = NULL);

		int32 init();
		int32 run();
		int32 stop();

		int32 setObj(void* obj, OBJ_INIT it, OBJ_CLEAN cl){
			int32 ret = 0;
			if(m_clean)
				ret = m_clean(this, m_obj);
			m_obj = obj;
			m_init = it;
			m_clean = cl;
			return ret;
		}

		void* getObj(){
			return m_obj;
		}

		int32 getId(){
			return m_id;
		}

		void setId(int32 id){
			m_id = id;
		}

		//the function below is not thread safe	
		int32 addTimer(Timer* tm);
		int32 delTimer(Timer* tm);
		int32 addConnection(Connection *con);
		int32 delConnection(int32 id);
		int32 closeConnection(int32 id);
		Connection* getConnection(int32 id);
		int32 sendMessage(int32 id, const std::string &msg);
		int32 closeAllConnections();
		size_t ConnectionsCount();
	private:
		int32 startCtl();

		int32 delAllConnections();
		int32 delAllOp();

		//return next timer start after ms
		int64 processTimers();
		int32 processOps();

		struct less
		{ // functor for operator<
			bool operator()
				(const Timer* _Left, const Timer* _Right) const
			{
				if(_Left == _Right)
					return false;
				bool ret = *_Left < *_Right;
				//std::cout <<"less ret:" << ret << ", L:" << std::hex << _Left
				//<< ", R:" << _Right << std::dec << ", l.sec:" << _Left->getTimeoutTime().m_sec << ", l.usec:" 
				//<< _Left->getTimeoutTime().m_usec << ", R.sec:" << _Right->getTimeoutTime().m_sec 
				//	<< ", R.usec:" << _Right->getTimeoutTime().m_usec << std::endl;
				return ret;
			}
		};


		enum{
			STATUS_INIT = 0,
			STATUS_RUNNING = 1,
			STATUS_CLOSING = 2,
			STATUS_CLOSED = 3,
		};

		int32 m_max_fds;
		volatile int32 m_status;
		SOCKET m_epl;
		SOCKET m_ctlfd;
		int32 m_ctlport;
		SOCKET m_ctlfd1;

		typedef std::map<int32, Connection*> con_map;
		con_map m_con_map;

		typedef std::set<Timer*, less> timer_map;
		timer_map m_timer_map;

		MUTEX m_opcs;
		volatile int32 m_ops_flag;

		LoopBuf m_ops;

		int32 m_id;
		void* m_obj;
		OBJ_INIT m_init;	
		OBJ_CLEAN m_clean;	
	};

};

#endif/*EF_NET_THREAD_H*/


