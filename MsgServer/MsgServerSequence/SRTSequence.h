#ifndef __MsgServerSequence_SRTSEQUENCE_H__
#define __MsgServerSequence_SRTSEQUENCE_H__
#include "TCPListenerSocket.h"
#include "SRTSequenceListener.h"
#include "MsConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

class SRTSequence
{
protected:
	SRTSequence(void);
public:

	virtual ~SRTSequence(void);

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
	static SRTSequence* Inst();

public:
	int		Start(const MsConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    SRTSequenceListener    *m_pSequenceListener;

};

#endif	// __MsgServerSequence_SRTSEQUENCE_H__
