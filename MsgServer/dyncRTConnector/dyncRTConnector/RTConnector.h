#ifndef __RT_CONNECTOR_H__
#define __RT_CONNECTOR_H__
#include "TCPListenerSocket.h"
#include "RTConnListener.h"
#include "RTModuleListener.h"
#include "RTConnTcpListener.h"

#include <iostream>
#include <map>

class RTConnector
{
protected:
	RTConnector(void);
public:
    
	virtual ~RTConnector(void);

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
	static RTConnector* Inst();
    
public:
	int		Start(const char*pLanIP, unsigned short usConnPort
                  , const char*pModuleAddr, unsigned short usModulePort
                  , const char*pConnTcpAddr, unsigned short usConnTcpPort
                  , const char*pHttpAddr, unsigned short usHttpPort);
	void	DoTick();
	void	Stop();

private:
    
	RTConnListener		*m_pConnListener;
    RTModuleListener    *m_pModuleListener;
    RTConnTcpListener   *m_pConnTcpListener;

	OSMutex				m_MutexRoom;
	OSMutex				m_MutexSession;

};

#endif	// __RT_CONNECTOR_H__