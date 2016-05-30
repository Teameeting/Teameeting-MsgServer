#ifndef __MsgServerRTRedis_RRTRTREDIS_H__
#define __MsgServerRTRedis_RRTRTREDIS_H__
#include "TCPListenerSocket.h"
#include "RRTRTRedisListener.h"

#include <iostream>
#include <map>

class RRTRTRedis
{
protected:
	RRTRTRedis(void);
public:

	virtual ~RRTRTRedis(void);

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
	static RRTRTRedis* Inst();

public:
	int		Start(const char*pRTRedisIp, unsigned short usRTRedisPort);
	void	DoTick();
	void	Stop();

private:

    RRTRTRedisListener    *m_pRTRedisListener;

};

#endif	// __MsgServerRTRedis_RRTRTREDIS_H__
