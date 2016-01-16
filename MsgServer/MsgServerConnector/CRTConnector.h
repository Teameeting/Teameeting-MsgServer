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
	int		Start(const char*pLanIP, unsigned short usConnPort
                  , const char*pModuleAddr, unsigned short usModulePort
                  , const char*pConnTcpAddr, unsigned short usConnTcpPort
                  , const char*pHttpAddr, unsigned short usHttpPort);
	void	DoTick();
	void	Stop();

private:
    
	CRTConnListener		*m_pConnListener;
    CRTModuleListener    *m_pModuleListener;
    CRTConnTcpListener   *m_pConnTcpListener;

	OSMutex				m_MutexRoom;
	OSMutex				m_MutexSession;

};

#endif	// __MsgServerConnector_CRTCONNECTOR_H__