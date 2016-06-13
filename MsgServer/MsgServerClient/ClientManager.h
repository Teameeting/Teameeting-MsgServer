//
//  ClientManager.h
//  SeqnClient
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __SeqnClient__ClientManager__
#define __SeqnClient__ClientManager__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "ClientSession.h"
#include "RTTcp.h"
#include "RTType.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/storage_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#include "MsgServer/proto/common_msg.pb.h"

class ClientManager : public RTSingleton< ClientManager >{
    friend class RTSingleton< ClientManager >;
public:

    void InitClient(const char* pUserid, const char* pIp, unsigned int port);
    void UninClient();
    void RequestLoop();
    void GenericMsg(int type, const std::string& userid);
    int  GenericMsgId(std::string& strMsgId);
    int  GenericTempMsgId(std::string& strMsgId);
    void DoPackage(int type);
    void ProcessRecvMessage(const char*data, int nLen);
    void Keepalive(int type);

    bool SignalKill();
    bool ClearAll();
    void SetUserId(std::string userid) { mUserId = userid; }

protected:
    ClientManager() { }
    ~ClientManager() { }
private:
    ClientSession*   mClientSession;
    std::string     mUserId;
    bool            mIsRun;
	std::ifstream   mIfs;
	std::list<pms::StorageMsg*> m_sequeWait2SndList;
	std::list<pms::StorageMsg*> m_storeWait2SndList;
	std::list<pms::StorageMsg*> m_logicW2SndList;
	std::list<pms::StorageMsg*> m_logicR2SndList;
    long long       mSendCounter;
    long long       mRecvCounter;
};

#endif /* defined(__SeqnClient__ClientManager__) */
