#ifndef __MsgServerLogical_LRTLOGICAL_H__
#define __MsgServerLogical_LRTLOGICAL_H__
#include "TCPListenerSocket.h"
#include "LRTLogicalListener.h"
#include "MsConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

class LRTLogical
{
protected:
	LRTLogical(void);
public:

	virtual ~LRTLogical(void);

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
	static LRTLogical* Inst();

public:
	int		Start(const MsConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    LRTLogicalListener    *m_pLogicalListener;

};

#endif	// __MsgServerLogical_LRTLOGICAL_H__
