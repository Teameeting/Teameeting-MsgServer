#include "ef_event_loop.h"
#include "ef_connection.h"
#include "ef_operator.h"
#include "ef_net_settings.h"
#include "base/ef_utility.h"
#include "ef_net_log.h"
#if defined(GPUSH_LINUX)
#include <sys/epoll.h>
#else
#include <sys/select.h>
#endif
#include <errno.h>
#include <string.h>
#include <cassert>
#include <iostream>
#include <sstream>


namespace ef{

	EventLoop::EventLoop()
		:m_max_fds(NetSettings::epollSize),
		m_status(STATUS_CLOSED),
		m_epl(EF_INVALID_SOCKET),
		m_ctlfd(EF_INVALID_SOCKET),
		m_ctlfd1(EF_INVALID_SOCKET),
		m_ops_flag(0),
		m_id(-1),
		m_obj(NULL),m_init(NULL), m_clean(NULL)
	{
		mutexInit(&m_opcs);
	}

	EventLoop::~EventLoop(){
		stop();
		while(m_status != STATUS_CLOSED){
			sleep_ms(50);
			addAsynOperator();
		}
		delAllOp();
		delAllConnections();
		sock_close(m_ctlfd);
		sock_close(m_ctlfd1);
		mutexDestroy(&m_opcs);
		if(m_clean)
			m_clean(this, m_obj);
	}

	int32 EventLoop::delAllConnections(){
		return 0;	
	}


	int32 EventLoop::startCtl(){
		int32 ret = 0;
		int32 sockets[2];
		if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0){
			NLogError << "EventLoop" 
				<< std::hex << this 
				<< "create ctl sock fail!";
			return -1;	
		}
		m_ctlfd = sockets[0];
		m_ctlfd1 = sockets[1];
		setSocketNonblocking(m_ctlfd);
		setSocketNonblocking(m_ctlfd1);

#if defined(GPUSH_LINUX)
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = m_ctlfd;
		ret = epoll_ctl(m_epl, EPOLL_CTL_ADD, m_ctlfd, &ev);
#else
        printf("file %s func:%s line:%d not implement!!!\n", __FILE__, __FUNCTION__, __LINE__);
#endif

		if(ret < 0){
			sock_close(m_ctlfd);
			m_ctlfd = EF_INVALID_SOCKET; 
			sock_close(m_ctlfd1);
			m_ctlfd1 = EF_INVALID_SOCKET; 
		}
		return ret;
	}

	int32 EventLoop::addConnection(Connection *con){
		assert(con);
		std::string addr;
		int32 port;
		con->getAddr(addr,port);
		con->setEventLoop(this);
		if(con->onCreate(this) < 0){
			NLogError << "EventLoop:" << this << ", con:" 
				<< con << ", id:" << con->getId() 
				<< ", fd:" << con->getFd() << ", " 
				<< (addr) << ":" << port << " onCreate fail!";
			con->recycle();
			return -1;
			
		}
		if(m_con_map.find(con->getId()) == m_con_map.end()){
			m_con_map.insert(con_map::value_type(con->getId(), con));
#if DETAIL_NET_LOG
			NLogTrace << "EventLoop:" << this << ", con:" 
				<< con << ", id:" << con->getId() 
				<< ", fd:" << con->getFd() << ", " 
				<< (addr) << ":" << port << " add sucess!";
#endif
		}else{
			NLogWarn << "EventLoop:" << this << ", con:" 
				<< con << ", id:" << con->getId() 
				<< ", fd:" << con->getFd() << ", " 
				<< (addr) << ":" << port << " has be added!";
		}
		return 0;
	}

	int32 EventLoop::delConnection(int32 id){
		std::string addr;
		int32 port; 
		con_map::iterator itor = m_con_map.find(id);
		Connection* con = NULL;
		if(itor != m_con_map.end()){
			con = itor->second;
			con->getAddr(addr,port);
			m_con_map.erase(itor);
#if DETAIL_NET_LOG
			NLogTrace << "EventLoop:" << this << ", con:" 
				<< con << ", id:" << con->getId() 
				<< ", fd:" << con->getFd() << ", " 
				<< (addr) << ":" << port << " del sucess!";
#endif
		}else{
			NLogError << "EventLoop:" << this
				<< ", id:" << id
				<< " del not find!";
		}
		return 0;
	}

	Connection* EventLoop::getConnection(int32 id){
		con_map::iterator itor = m_con_map.find(id);
		if(itor != m_con_map.end()){
			return (*itor).second;
		}
		return NULL;
	}

	int32 EventLoop::closeAllConnections(){
		std::list<Connection*> cons;
		con_map::iterator it = m_con_map.begin();
		for(; it != m_con_map.end(); ++it){
			cons.push_back(it->second);
		}
		std::list<Connection*>::iterator i = cons.begin();
		for(; i != cons.end(); ++i){
			(*i)->safeClose();
		}
		return 0;
	}


	int32 EventLoop::asynCloseAllConnections(){
		NetOperator* op = new CloseAllConnectionsOp();
		return addAsynOperator(op);
	}

	int32 EventLoop::stop(){
		NLogError << "EventLoop:" << std::hex << this << " stop!";
		int32 status = atomicCompareExchange32(&m_status, 
			STATUS_CLOSING, STATUS_RUNNING);
		if(status == STATUS_RUNNING){
			asynCloseAllConnections();
		} 
		return 0;
	}

	int32 EventLoop::init(){
		int32 ret = 0; 

#if defined(GPUSH_LINUX)
		m_epl = epoll_create(m_max_fds); 
		if(m_epl < 0){
			NLogError << "EventLoop:" << std::hex 
				<< this << " init epoll_create fail!";
			return -1;

		}
#else
        printf("file %s func:%s line:%d not implement!!!\n", __FILE__, __FUNCTION__, __LINE__);
#endif

		ret = startCtl();
		if(ret < 0){
			NLogError << "EventLoop:" << std::hex 
				<< this << " init startCtl fail!";
			return -4;
		}

		if(m_init){
			m_init(this, m_obj);
		}

		return 0;
	}

	size_t EventLoop::ConnectionsCount(){
		size_t i = 0;
		i = m_con_map.size();
		return i;
	}

	int32 EventLoop::run(){
		int32 ret = 0;
		int32 nfds = 0;
		m_status = STATUS_RUNNING;
		NLogError << "EventLoop:" << std::hex << this << " run start!";

#if defined(GPUSH_LINUX)
		struct epoll_event events[MAX_PROC_EVENT_CNT];
		int32 events_on_loop = MAX_PROC_EVENT_CNT < NetSettings::procEventCnt ? 
				MAX_PROC_EVENT_CNT : NetSettings::procEventCnt;

		time_tv tv;
		while(1){
			//process Op and timer at first
			processOps();
			int64 t = processTimers();
			if(m_status != STATUS_RUNNING&& !ConnectionsCount()){
				m_status = STATUS_CLOSED;
				break;
			}
			if(t > 0){
				nfds = epoll_wait(m_epl,events, events_on_loop, t);
			}else{
				nfds = epoll_wait(m_epl,events, events_on_loop, -1);
			}
			if(nfds < 0){
				NLogTrace << "EventLoop:" << std::hex << this 
					<< ", epoll error,errno:" << errno;   
				if(errno != SOCK_EINTR){ 
					break;
				}
			}
			for(int32 i = 0; i < nfds && i < events_on_loop; ++i){
				if(events[i].data.fd == m_ctlfd){
					//std::cout << "epoll m_ctlfd"<< std::endl;
					continue;
				}
				std::string addr;
				int32 port;
				Device* con = (Device*)events[i].data.ptr;
				con->getAddr(addr,port); 
				if(events[i].events & EPOLLIN){
					ret = con->handleRead(this);
				}else if(events[i].events & EPOLLOUT){
					ret = con->handleWrite(this);
				}else{
					ret = -1;
				}

				if(ret < 0){
					NLogWarn << "EventLoop:" << std::hex << this 
							<< ", con:" << std::hex << con 
							<< std::dec << ", fd:" << con->getFd() 
							<< ", " << (addr) << ":" << port
							<< " handle event:" << events[i].events 
							<< " fail, recycle!";
					con->onFail();
					continue;
				}
#if DETAIL_NET_LOG
				else{
					NLogTrace << "EventLoop:" << std::hex << this 
						<< ", con:" << std::hex << con
						<< std::dec << ", fd:" << con->getFd() 
						<< ", " << (addr) << ":" << port
						<< " handle event success!";
				}
#endif
			}
		}
#else
        printf("file %s func:%s line:%d not implement!!!\n", __FILE__, __FUNCTION__, __LINE__);
#endif// GPUSH_LINUX
		NLogError << "EventLoop:" << std::hex << this << " run stop!";
		return 0;
	}

	int32 EventLoop::processOps(){
		NetOperator *op = NULL;
		char buf[1024 * 16];
		int32 loop = sizeof(buf);
		atomicExchange32(&m_ops_flag, 0);
		loop = recv(m_ctlfd, buf, sizeof(buf), 0);
		int32 cnt = 0;
		while(m_ops.size()){
			mutexTake(&m_opcs);
			if(m_ops.size() >= (int32)sizeof(op)){
				 m_ops.read((uint8*)&op, sizeof(op));
				mutexGive(&m_opcs);
			}else{
				mutexGive(&m_opcs);
				break;
			}
			//recv notify
			assert(op);
			op->process(this);
			delete op;
			++cnt;
		}

		return 0;
	}

	int32 EventLoop::delAllOp(){

		NetOperator *op = NULL;
		while(1){
			mutexTake(&m_opcs);
			if(m_ops.size() >= (int32)sizeof(op)){
				m_ops.read((uint8*)&op, sizeof(op));
				mutexGive(&m_opcs);
			}else{
				mutexGive(&m_opcs);
				break;
			}
			//recv notify
			assert(op);
			delete op;
		}

		return 0;
	}

	int32 EventLoop::addAsynOperator(NetOperator* op){

		int32 ret = 0;
		
		if(op){
			mutexTake(&m_opcs);
			m_ops.autoResizeWrite((const uint8*)&op, sizeof(op));
			mutexGive(&m_opcs);
		}

		int32 flag = atomicCompareExchange32(&m_ops_flag, 1, 0);
		if(!flag){ 
			char ctl;
			ret = send(m_ctlfd1, &ctl, sizeof(ctl), 0);
			if(ret < 0){
				NLogError << "EventLoop:" << std::hex 
					<< this << ", send ctl msg fail";
				return -1;
			}else{
	#if DETAIL_NET_LOG
				NLogTrace << "EventLoop:" << std::hex 
					<< this << ", send ctl msg";
	#endif
			}
		}
		return ret;
	}

	int64 EventLoop::processTimers(){
		int32 max_timer_one_loop 
			= MAX_PROC_TIMER_CNT < NetSettings::procTimerCnt ?
			MAX_PROC_TIMER_CNT : NetSettings::procTimerCnt;
		timeval t;
		gettimeofday(&t, NULL);
		time_tv tv(t);
		timer_map::iterator itor = m_timer_map.begin();
		int32 i = 0;
		int64 ret = 0;
		int32 ods = m_timer_map.size();
		while(itor != m_timer_map.end() && i < max_timer_one_loop){
			timeval t;
			gettimeofday(&t, NULL);
			time_tv tv(t);
			timer_map::value_type tm = (*itor);
			time_tv tv1 = tm->getTimeoutTime();
			int64 dif = tv_diff_ms(tv1, tv);
			if(dif > 0){
				ret = dif;
				break;
			}

			//NOTO: please not change the after 2 line position 
			m_timer_map.erase(itor);

#if DETAIL_NET_LOG
			int32 s = m_timer_map.size();
			NLogTrace << "EventLoop:" << std::hex 
				<< this << ", timeout, timer:" << tm
				<< std::dec
				<< ", sec:" << tm->getTimeoutTime().m_sec
				<< ", usec:" << tm->getTimeoutTime().m_usec
				<< ", old_size:" << ods 		
				<< ", map.size:" << s;

#endif/*DETAIL_NET_LOG*/
			//when connection.Timer timeout ,the timer would be clear
			//so we erase it before
			tm->timeout(this);
			itor = m_timer_map.begin();
			++i;
		}
#if DETAIL_NET_LOG
		NLogTrace << "EventLoop:" << std::hex 
				<< this << std::dec
				<< ", sec:" << tv.m_sec
				<< ", usec:" << tv.m_usec				
				<< ", diff_ms:" << ret
				<< ", old_size:" << ods 		
				<< ", map.size:" << m_timer_map.size();		
#endif
		return ret;
	}


	int32 EventLoop::addNotify(Device *con, int32 noti){
		assert(con);
		std::string addr;
		int32 port;
		con->getAddr(addr,port);

		int32 ret = 0;
#if defined(GPUSH_LINUX)
		struct epoll_event ev;
		ev.events = 0;
		//ev.events = EPOLLET;
		if(noti & READ_EVENT){
			ev.events |= EPOLLIN;
		}

		if(noti & WRITE_EVENT){
			ev.events |= EPOLLOUT;
		}

		ev.data.ptr = con;
		ret = epoll_ctl(m_epl, EPOLL_CTL_ADD, con->getFd(), &ev);

		if(ret >= 0){ 
#if DETAIL_NET_LOG 
			NLogTrace << "EventLoop:" << std::hex << this 
				<< ", con:" << std::hex << con 
				<< std::dec << ", fd:" << con->getFd()
				<< ", epoll_fd:" << m_epl << " " << (addr) 
				<< ":" << port << " setnoti:" << std::hex 
				<< noti << ", events:" << ev.events
				<< " success!";
#endif
		}else{
			NLogError << "EventLoop:" << std::hex << this 
				<< ", con:" << std::hex << con 
				<< std::dec << ", fd:" << con->getFd()
				<< ", epoll_fd:" << m_epl << " " << (addr) 
				<< ":" << port << " setnoti:" << std::hex 
				<< noti << ", events:" << " fail, errno:"
				<< errno;
		}
#else
        printf("file %s func:%s line:%d not implement!!!\n", __FILE__, __FUNCTION__, __LINE__);
#endif//GPUSH_LINUX
		return	ret;  
	}

	int32 EventLoop::modifyNotify(Device *con, int32 noti){
		assert(con);
		std::string addr;
		int32 port;
		con->getAddr(addr,port);

		int32 ret = 0;
#if defined(GPUSH_LINUX)
		struct epoll_event ev;
		ev.events = 0;
		//ev.events = EPOLLET;
		if(noti & READ_EVENT){
			ev.events |= EPOLLIN;
		}

		if(noti & WRITE_EVENT){
			ev.events |= EPOLLOUT;
		}
		
		ev.data.ptr = con;
		ret = epoll_ctl(m_epl, EPOLL_CTL_MOD, con->getFd(), &ev);

		if(ret >= 0){  
#if DETAIL_NET_LOG
			NLogTrace << "EventLoop:" << std::hex << this 
				<< ", con:" << std::hex << con 
				<< std::dec << ", fd:" << con->getFd()
				<< ", epoll_fd:" << m_epl << " " << (addr) 
				<< ":" << port << " modifyNoti:" << std::hex 
				<< noti << ", events:" << ev.events
				<< " success!";
#endif
		}else{
			NLogError << "EventLoop:" << std::hex << this 
				<< ", con:" << std::hex << con 
				<< std::dec << ", fd:" << con->getFd()
				<< ", epoll_fd:" << m_epl << " " << (addr) 
				<< ":" << port << " modifyNoti:" << std::hex 
				<< noti << ", events:" << ev.events
				<< " fail, errno:" << errno;
		}
#else
        printf("file %s func:%s line:%d not implement!!!\n", __FILE__, __FUNCTION__, __LINE__);
#endif//GPUSH_LINUX
		return	ret;  
	}


	int32 EventLoop::clearNotify(Device *con){
		assert(con);
		std::string addr;
		int32 port;
		con->getAddr(addr,port);

		int32 ret = 0;
#if defined(GPUSH_LINUX)
		struct epoll_event ev;
		ev.events = 0;

		ev.data.ptr = con;

		ret = epoll_ctl(m_epl, EPOLL_CTL_DEL, con->getFd(), &ev);

		if(ret >= 0){ 
#if DETAIL_NET_LOG 
			NLogTrace << "EventLoop:" << std::hex << this 
				<< ", con:" << std::hex << con 
				<< std::dec << ", fd:" << con->getFd()
				<< ", epoll_fd:" << m_epl
				<< " " << (addr) << ":"
				<< port << " claerNoti success";
#endif
		}else{
			NLogError << "EventLoop:" << std::hex << this 
				<< ", con:" << std::hex << con 
				<< std::dec << ", fd:" << con->getFd()
				<< ", epoll_fd:" << m_epl << " " << (addr) 
				<< ":" << port << " claerNoti fail, errno:"
				<< errno;
		}
#else
        printf("file %s func:%s line:%d not implement!!!\n", __FILE__, __FUNCTION__, __LINE__);
#endif//GPUSH_LINUX
		return	ret;  
	}

	int32 EventLoop::addTimer(Timer* tm){
		int32 ods = m_timer_map.size();
		m_timer_map.insert(tm);

#if DETAIL_NET_LOG
		int32 s = 0;
		s = m_timer_map.size();
		NLogTrace << "EventLoop:" << std::hex << this 
				<< ", add timer:" << tm << std::dec
				<< ", sec:" << tm->getTimeoutTime().m_sec
				<< ", usec:" << tm->getTimeoutTime().m_usec
				<< ", old_size:" << ods
				<< ", map.size:" << s;
#endif
		return 0;
	}

	int32 EventLoop::delTimer(Timer* tm){

		int32 ods = m_timer_map.size();
		m_timer_map.erase(tm);
#if DETAIL_NET_LOG
		int32 s = 0;
		s = m_timer_map.size();
		NLogTrace << "EventLoop:" << std::hex << this 
				<< " del timer:" << tm << std::dec
				<< ", sec:" << tm->getTimeoutTime().m_sec
				<< ", usec:" << tm->getTimeoutTime().m_usec				
				<< ", old_size:" << ods
				<< ", map.size:" << s;
#endif
		return 0;
	}

	int32 EventLoop::asynAddConnection(Connection* con){
		NetOperator *op = new AddConnectionOp(con);

		addAsynOperator(op);
#if DETAIL_NET_LOG
		NLogTrace << "EventLoop:" << std::hex << this
			<< ", con:" << std::hex << con << std::dec
			<< ", asynAddConnection, id:" << con->getId()
			<< ", fd:," << (con ? con->getFd() : -1)
			<< " " << (con ? (con->getIp()) : "0")
			<< ":" << (con ? con->getPort() : -1);
#endif
		return 0;

	}

	int32 EventLoop::asynCloseConnection(int32 id){
		NetOperator *op = new CloseConnectionOp(id);

		addAsynOperator(op);
#if DETAIL_NET_LOG
		NLogTrace << "EventLoop:" << std::hex << this
			<< std::dec
			<< ", asynCloseConnection, id:" << id;
#endif

		return 0;

	}

	int32 EventLoop::closeConnection(int32 id){
		int32 ret = 0;
		Connection *con = getConnection(id);
		if(con){
			ret = con->safeClose();
		}

		return ret;
	}

	int32 EventLoop::asynSendMessage(int32 id, const std::string &msg){
		NetOperator *op = new SendMessageOp(id, msg);
		addAsynOperator(op);

#if DETAIL_NET_LOG
		NLogTrace << "EventLoop:" << std::hex << this
			<< std::dec << ", asynSendMessage, id:" << id
			<< ", size:" << msg.size();
#endif
		return 0;
	}

	int32 EventLoop::sendMessage(int32 id, const std::string &msg){
		int32 ret = 0;
		Connection *con = getConnection(id);
		if(con){
			ret = con->sendMessage(msg);
			if(ret < 0){
				con->recycle();
			}
		}else{
			
			NLogError << "EventLoop:" << std::hex << this 
				<< ", con_id:" << std::dec << id 
				<< ", sendMesage not found!";
		}
		return 0;
	}
}

