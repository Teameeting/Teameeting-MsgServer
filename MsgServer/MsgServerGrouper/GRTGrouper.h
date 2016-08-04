#ifndef __MsgServerGrouper_GRTGROUPER_H__
#define __MsgServerGrouper_GRTGROUPER_H__
#include "TCPListenerSocket.h"
#include "GRTGrouperListener.h"

#include <iostream>
#include <map>
#include <vector>

class GRTGrouper
{
protected:
	GRTGrouper(void);
public:

	virtual ~GRTGrouper(void);

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
	static GRTGrouper* Inst();

public:
	int		Start(const char*pGrouperIp, unsigned short usGrouperPort, const char*pGroupMgrIp, unsigned short usGroupMgrPort, const char* pRedisIp, unsigned short usRedisPort);
	void	DoTick();
	void	Stop();

private:
    GRTGrouperListener    *m_pGrouperListener;
};

#endif	// __MsgServerGrouper_GRTGROUPER_H__
