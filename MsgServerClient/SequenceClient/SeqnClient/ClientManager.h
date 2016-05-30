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
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "ClientSession.h"
#include "RTTcp.h"
#include "RTType.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "MsgServer/proto/sequence_msg.pb.h"

#define HR_USERID       "hr_userid"
#define HR_CONNECTORID  "hr_connectorid"

class ClientManager : public RTSingleton< ClientManager >{
    friend class RTSingleton< ClientManager >;
public:

    void InitClient(const char* pUserid, const char* pIp, unsigned int port);
    void UninClient();
    void RequestLoop();
    void GenericMsg(const std::string& userid);
    int  GenericMsgId(std::string& strMsgId);
    void DoPackage();
    void ProcessRecvMessage(const char*data, int nLen);
    void Keepalive();

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
    FILE*           pSendFile;
    FILE*           pRecvFile;
	std::list<pms::SequenceMsg*> m_wait2SndList;
    long long       mSendCounter;
    long long       mRecvCounter;
};

#endif /* defined(__SeqnClient__ClientManager__) */
