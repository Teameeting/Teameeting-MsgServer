#ifndef __XTCP_CLIENT_IMPL_H__
#define __XTCP_CLIENT_IMPL_H__

#include "core/XTcpClient.h"
#include "core/XTcpTick.h"

#include "webrtc/base/nethelpers.h"
#include "webrtc/base/physicalsocketserver.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/signalthread.h"
#include "webrtc/base/sigslot.h"

class XTcpClientImpl
	: public sigslot::has_slots < >
	, public rtc::MessageHandler
	, public XTcpClient
	, public XTcpTick
{
public:
	XTcpClientImpl(XTcpClientCallback &rCallback);
	virtual ~XTcpClientImpl();

public:
	//* For XTcpClient
	virtual TcpState Status(){ return m_nState; };
	virtual void Connect(const std::string& server, int port, bool bAutoConnect);
	virtual void Disconnect();
	// Must implement in rCallback.OnTick()
	virtual int SendMessageX(std::string&strMsg);
	virtual int SendMessageX(const char*pMsg, int nLen);

	//* For XTcpTick
	virtual void OnTick(void);
	virtual void OnFinish(void);

	//* Implements the MessageHandler interface
	void OnMessage(rtc::Message* msg);
protected:
	void DoResolver();
	void DoConnect();
	void Close();
	void InitSocketSignals();
	bool ConnectControlSocket();

	//* For sigslot
	void OnConnect(rtc::AsyncSocket* socket);
	void OnRead(rtc::AsyncSocket* socket);
	void OnClose(rtc::AsyncSocket* socket, int err);
	void OnResolveResult(rtc::AsyncResolverInterface* resolver);

public:
	XTcpClientCallback	&m_rCallback;
	rtc::SocketAddress	m_svrSockAddr;
	rtc::AsyncResolver	*m_asynResolver;
	rtc::scoped_ptr<rtc::AsyncSocket> m_asynSock;

	TcpState				m_nState;
	bool				m_bAutoConnect;

	rtc::CriticalSection	m_csBuf;
	char				*m_pBuffer;
	int					m_nBufLen;
	int					m_nBufOffset;
};


#endif	// __XTCP_CLIENT_IMPL_H__
