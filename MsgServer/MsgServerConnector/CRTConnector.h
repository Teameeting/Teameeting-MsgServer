#ifndef __MsgServerConnector_CRTCONNECTOR_H__
#define __MsgServerConnector_CRTCONNECTOR_H__
#include "TCPListenerSocket.h"
#include "CRTConnListener.h"
#include "CRTModuleListener.h"
#include "CRTConnTcpListener.h"

#include <iostream>
#include <map>

class CRTConnector
{
protected:
	CRTConnector(void);
public:

	virtual ~CRTConnector(void);

	/* 打印服务版本号
	 * 参数:	无
	 */
	static void PrintVersion();
	/* 系统初始化
	 * 参数:	evTbSize - 系统的Hash表大小，根据系统的设计容量而变化
	 */
	static void Initialize(int evTbSize);
	/* 系统注销
	 * 参数:	无
	 */
	static void DeInitialize();

	/*
	 *
	 */
	static CRTConnector* Inst();

public:
	int		Start(const char*pWebConIp, unsigned short usWebConPort
                  , const char*pModuleIp, unsigned short usModulePort
                  , const char*pCliConIp, unsigned short usCliConPort
                  , const char*pHttpIp, unsigned short usHttpPort);
	void	DoTick();
	void	Stop();

private:

	CRTConnListener		*m_pConnListener;
    CRTModuleListener    *m_pModuleListener;
    CRTConnTcpListener   *m_pConnTcpListener;

};

#endif	// __MsgServerConnector_CRTCONNECTOR_H__
