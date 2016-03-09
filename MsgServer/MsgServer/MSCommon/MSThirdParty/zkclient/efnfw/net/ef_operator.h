#ifndef __EF_OPERATOR_H__
#define __EF_OPERATOR_H__

#include "ef_common.h"
#include <string>

namespace ef{
	class EventLoop;
	class Connection;

	class NetOperator
	{
	public:
		virtual ~NetOperator(){

		}

		virtual int32 process(EventLoop *thr){
			return 0;
		}

	};

	class CloseConnectionOp:public NetOperator
	{
	public:
		CloseConnectionOp(int32 conid);

		virtual int32 process(EventLoop *thr);

	protected:
	private:
		int32 m_conid;
	};

	class CloseAllConnectionsOp:public NetOperator
	{
	public:
		virtual int32 process(EventLoop *thr);
	protected:
	private:
	};


	class SendMessageOp:public NetOperator
	{
	public:
		SendMessageOp(int32 conid, const std::string& msg);
		virtual int32 process(EventLoop *thr);

	protected:
	private:
		int32 m_conid;
		std::string m_msg;
	};


	class AddConnectionOp:public NetOperator
	{
	public:
		AddConnectionOp(Connection* c)
			:m_c(c){
		}
		virtual int32 process(EventLoop *thr);
	private:
		Connection* m_c;
	};


};

#endif/*EF_OPERATOR_H*/

