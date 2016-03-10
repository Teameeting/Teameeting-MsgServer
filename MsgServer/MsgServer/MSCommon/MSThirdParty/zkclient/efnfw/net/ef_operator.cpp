#include "ef_operator.h"
#include "ef_connection.h"
#include "ef_event_loop.h"

namespace ef{


	CloseConnectionOp::CloseConnectionOp(int32 conid)
		:m_conid(conid){

	}

	int32 CloseConnectionOp::process(EventLoop *thr){
		return thr->closeConnection(m_conid);
	}

	int32 CloseAllConnectionsOp::process(EventLoop *thr){
		return thr->closeAllConnections();
	}

	SendMessageOp::SendMessageOp(int32 conid, const std::string& msg)
		:m_conid(conid), m_msg(msg){

	}

	int32 SendMessageOp::process(EventLoop *thr){
		return thr->sendMessage(m_conid, m_msg);
	}


	int32 AddConnectionOp::process(EventLoop *thr){
		return thr->addConnection(m_c);
	}

};

