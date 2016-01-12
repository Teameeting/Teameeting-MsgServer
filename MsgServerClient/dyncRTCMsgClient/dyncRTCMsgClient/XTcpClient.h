#ifndef __XTCP_CLIENT_H__
#define __XTCP_CLIENT_H__
#include "RTC_Export.h"
#include <string>

enum TcpState {
	NOT_CONNECTED,
	RESOLVING,
	CONNECTTING,
	CONNECTED
};

class XTcpClientCallback {
public:
	virtual void OnServerConnected() = 0;
	virtual void OnServerDisconnect() = 0;
	virtual void OnServerConnectionFailure() = 0;
    virtual void OnServerState(TcpState state) = 0;

	virtual void OnTick() = 0;
	virtual void OnMessageSent(int err) = 0;
	virtual void OnMessageRecv(const char*pData, int nLen) = 0;
protected:
	virtual ~XTcpClientCallback() {}
};

class RTC_API XTcpClient
{
public:
	virtual ~XTcpClient(void){};
	static XTcpClient* Create(XTcpClientCallback&rCallback);

public:
	virtual TcpState Status() = 0;
	virtual void Connect(const std::string& server, int port, bool bAutoConnect)=0;
	virtual void Disconnect()=0;
	// Must implement in rCallback.OnTick()
	virtual int SendMessageX(std::string&strMsg)=0;
	virtual int SendMessageX(const char*pMsg, int nLen)=0;

protected:
	XTcpClient(void){};
};

#endif	// __XTCP_CLIENT_H__