#include <cassert>
#include <errno.h>
#include "ef_connection.h"
#include "ef_event_loop.h"
#include "ef_net_log.h"
#include "base/ef_utility.h"


namespace ef{

Connection::Connection()
	:m_id(0),m_buf(),
	m_flag(FLAG_NULL),
	m_evlp(NULL)
{
	static int id = 0;
	m_id = atomicIncrement32(&id);
	atomicCompareExchange32(&id, 1, 0X7FFFFFF);
}

Connection::~Connection(){
	clear();
}

int32	Connection::clearTimer(){

	if(m_evlp){
		std::map<int32, ConnectionTimer*>::iterator itor = m_timers.begin();
		for(;itor != m_timers.end(); ++itor){
			m_evlp->delTimer((itor->second));
			delete itor->second;
		}
		m_timers.clear();
	}

	return 0;
}

int32	Connection::clear(){
	EventLoop* thr = getEventLoop();
	std::string ip;
	int32 port;
	getAddr(ip, port);
	NLogTrace << "con:" << std::hex << this 
		<< std::dec << ", id:" 
		<< m_id << ", fd:" << getFd() 
		<< ", from " << ip << ":" << port
		<< " clear!";
	if(thr){
		clearNotify(m_evlp);
		clearTimer();
		thr->delConnection(m_id);
		thr = NULL;
	}
	m_id = 0;

	return 0;
}

int32	Connection::safeClose(){
	//if has no to send
	std::string ip;
	int32 port;
	getAddr(ip, port);
	NLogTrace << "con:" << std::hex << this << ", id:" 
		<< m_id << ", fd:" << getFd()
		<< ", from " << ip << ":" << port
		<< " will be safe_close!";
	if(!m_send_buf.size()){
		recycle(); 
		return 0;
	}
	EventLoop* thr = getEventLoop();
	if(thr){
		modifyNotify(m_evlp, WRITE_EVENT);
		clearTimer();
		shutDownRead();
	}
	m_flag = FLAG_SAFE_CLOSE;
	return doSend();
}

int32	Connection::handleRead(EventLoop* l){
	int32	ret = 0;
	char*	tmpbuf = NULL;
	int32	wantlen = 0;
	int32	actrcv = 0;
	int32	totalrcv = 0;
	std::string ip;
	int32 port;
	getAddr(ip, port);
	//std::cout << "Connection::handleRead\n";
#ifdef READ_ALL
	while(true){
#else
	do{
#endif
		actrcv = 0;
		ret = 0;
		wantlen = m_buf.freeFrameLen();
		if(wantlen <= 0){
			m_buf.extend(16 * 1024);
			wantlen = m_buf.freeFrameLen();
		}
		tmpbuf = (char*)m_buf.freeFrameBuf();
		ret = tcpNbReceive(getFd(), tmpbuf, wantlen, &actrcv);
		NLogTrace << "con:" << std::hex << this 
			<< std::dec << ", id:" << m_id 
			<< ", fd:" << getFd() 
			<< ", from " << ip << ":" << port
			<< ", wantlen:" << wantlen
			<< ", recv ret:" << ret 
			<< ", actrcv:" << actrcv;
		if(ret < 0 || actrcv == 0){
			break;
		}
		totalrcv += actrcv;
		m_buf.write(NULL, actrcv);
		while(1){
			int32	len = checkPackLen();
			if(len < 0){
				NLogError << "con:" << std::hex << this
					<< std::dec << ", id:"
					<< m_id << ", fd:" << getFd()
					<< ", from " << ip << ":" << port
					<< " checkPackLen fail, len:"
					<< len;	
				return	len;
			}

			if(!len){
				break;
			}
			std::string	req;
			req.resize(len);
			readBuf((char*)req.data(), len);	
			ret = handlePacket(req);
			if(ret < 0){
				NLogError<< "con:" << std::hex << this 
					<< std::dec << ", id:" 
					<< m_id << ", fd:" << getFd() 
					<< ", from " << ip << ":" << port
					<< ", buf_size:"
					<< m_buf.size() << ", pack_len:" 
					<< len << ", handle_pack fail!";

				return	ret;
			}
		}
		//all data has been recv
		if(actrcv < wantlen)
			break;
#ifdef READ_ALL
	}
#else
	} while(0);
#endif
	//get read event, but recv no data
	if(totalrcv <= 0){
		NLogError<< "con:" << std::hex << this
			<< std::dec << ", id:"
			<< m_id << ", fd:" << getFd()
			<< ", from " << ip << ":" << port
			<< ", close";
			
		return -1;
	}	

	return	ret;
}

int32	Connection::handleWrite(EventLoop* l){

	std::string ip;
	int32 port;
	getAddr(ip, port);
	int32	ret = doSend();
	if(ret < 0)
		return	ret;
	if(!m_send_buf.size()){
		if(m_flag == FLAG_SAFE_CLOSE){
			NLogTrace << "con:" << std::hex << this 
				<< std::dec << ", id:" 
				<< getId() << ", fd:" << getFd()
				<< ", from " << ip << ":" << port
				<< ", safe close!";
			return	-2;
		}
		modifyNotify(m_evlp, READ_EVENT);
	}
	return ret;
}


ConnectionTimer* Connection::getTimer(int32 id){
	std::map<int32, ConnectionTimer*>::iterator itor = m_timers.find(id);
	if(itor != m_timers.end()){
		return itor->second;
	}
	return	NULL;
}

int32	Connection::startTimer(int32 id, int32 timeout){
	ConnectionTimer* tm = NULL;
	EventLoop* thr = getEventLoop();

	if(!thr){
		assert(thr);
		return -1;
	}

	tm = getTimer(id);
	if(tm){
		if(tm->status() == ConnectionTimer::STATUS_START){
			thr->delTimer(tm);
		}
	}else{
		tm = new ConnectionTimer(this, id);	
		m_timers[id] = tm;
	}

	tm->setTimeOutAfter(timeout);
	tm->setStatus(ConnectionTimer::STATUS_START);
	thr->addTimer(tm);
	
	return	0;
}

int32	Connection::stopTimer(int32 id){
	ConnectionTimer* tm = NULL;
	EventLoop* thr = getEventLoop();
	if(!thr){	
		assert(thr);
		return -1;
	}
	tm = getTimer(id);
	if(tm){
		if(tm->status() == ConnectionTimer::STATUS_START){
			thr->delTimer(tm);
		}
		tm->setStatus(ConnectionTimer::STATUS_STOP);
	}
	return	0;
}


int32	Connection::handleTimer(EventLoop* l, int32 id){
	return 0;	
}

int32	Connection::sendMessage(const std::string &msg){
	int32	ret = 0;
	bool	wnoti = false;
	std::string ip;
	int32 port;
	getAddr(ip, port);
	wnoti = (getNotify() & WRITE_EVENT);
	if(wnoti){
		m_send_buf.autoResizeWrite((const uint8*)msg.data(), msg.size());
		return	ret;
	}
	ret = send_(msg.data(), msg.size());
	NLogTrace << "con:" << std::hex << this 
		<< std::dec << ", id:" << getId() << ", fd:" 
		<< getFd() << ", from " << ip << ":" 
		<< port << ", req size:" << msg.size()
		<< ", send ret:" << ret << ", send_buf_size:"
		<< m_send_buf.size() << ", noti:" 
		<< getNotify();
	if(ret < 0){
		//recycle it after handle read/write event
		return	ret;
	}
	if(ret >= (int32)msg.size()){
		return	ret;
	}
	m_send_buf.autoResizeWrite((const uint8*)msg.data() + ret, 
		msg.size() - ret);	
	modifyNotify(m_evlp, READ_EVENT|WRITE_EVENT);
	return	ret;
}



int32	Connection::send_(const char* buf, int32 len){
	int32	errcode = 0;
	int32	ret = 0;
	std::string ip;
	int32 port;
	getAddr(ip, port);
do_:
	ret = send(getFd(), buf, (int32)len, 0);
	if(ret > 0){
		return	ret;
	}else	if(ret <= 0){
		errcode = errno;
		if (errcode ==  SOCK_EINTR) {
			//fprintf(stdout, "[TRACE] tcp_send : signal EINTR\n");
			goto	do_;
		}else if (errcode == SOCK_EAGAIN 
			|| errcode == SOCK_EWOULDBLOCK) {
			return	0;
		}else{
			NLogTrace << "con:" << std::hex << this 
				<< std::dec << ", id:" 
				<< getId() << ", fd:" << getFd() 
				<< ", from " << ip << ":" << port
				<< ", send ret:" << ret << ", errno:"
				<< errcode;
			return	-1;
		}
	}
#if 0
	//if no space in send buf, send return 0, errno: 11, but socket not close 
	}else{
		errcode = errno;
		NLogTrace << "con:" << std::hex << this 
			<< std::dec << ", id:" 
			<< getId() << ", fd:" << getFd() 
			<< ", from " << ip << ":" << port
			<< ", send ret:" << ret << ", errno:"
			<< errcode;
		return	-1;
	}
#endif
	return	ret;
}

//send the msg list
int32	Connection::doSend(){

	std::string ip;
	int32 port;
	getAddr(ip, port);
	int32 sendlen = 0;
	int32 len = m_send_buf.firstFrameLen();
	int32 ret = len;
	while(ret > 0){
		len = m_send_buf.firstFrameLen();
		ret = send_((const char*)m_send_buf.firstFrameData(), 
			m_send_buf.firstFrameLen());
		NLogTrace << "con:" << std::hex << this 
			<< std::dec << ", id:" 
			<< getId() << ", fd:" << getFd() 
			<< ", from " << ip << ":" << port
			<< ", send ret:" << ret << ", send_buf_size:"
			<< m_send_buf.size() << ", noti:" 
			<< getNotify() << ", errno:" << errno;
		if(ret < 0){
			return	ret;
		}
		sendlen += ret;
		m_send_buf.read(NULL, ret);
		if(ret < len){
			break;
		}
	}	
	return	sendlen;
}

};


