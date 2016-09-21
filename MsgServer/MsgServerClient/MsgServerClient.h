#ifndef __MsgServerClient_SEQN_CLIENT_H__
#define __MsgServerClient_SEQN_CLIENT_H__

#include <iostream>
#include <map>

class MsgServerClient
{
protected:
	MsgServerClient(void);
public:

	virtual ~MsgServerClient(void);

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
	static MsgServerClient* Inst();

public:
	int		Start(const char* pUserid, const char*pIp, unsigned short usPort);
	void	DoTick();
	void	Stop();

private:

};

#endif	// __MsgServerClient_SEQN_CLIENT_H__
