//
//  GRTGrouperManager.h
//  MsgServerGrouper
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerGrouper__GRTGrouperManager__
#define __MsgServerGrouper__GRTGrouperManager__

#include <stdio.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include "RTSingleton.h"
#include <hiredis/hiredis.h>
#include "xRedisClient.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class GRTTransferSession;

class GRTGrouperManager : public RTSingleton< GRTGrouperManager >{
    friend class RTSingleton< GRTGrouperManager >;
public:
    enum GrpMemStatus{ADD=1, DEL=2};

    //<userid, MemStatus>
    typedef std::unordered_map<std::string, enum GrpMemStatus>      GrpMembersStatusMap;
    typedef GrpMembersStatusMap::iterator                           GrpMembersStatusMapIt;

    //<groupid, user list>
    typedef std::unordered_map<std::string, std::unordered_set<std::string>* >         GroupMembersMap;
    typedef GroupMembersMap::iterator                                                  GroupMembersMapIt;

    // <groupid, msg sequence>
    typedef std::unordered_map<std::string, int64>        TmpGroupMsgMap;
    typedef TmpGroupMsgMap::iterator                      TmpGroupMsgMapIt;

    //<groupid>
    typedef std::list<std::string>                        TmpGroupList;

    bool RecvRequestCounter();
    bool SendResponseCounter();

    bool InitManager(const std::string& ip, int port);
    bool UninManager();

    bool GetGroupMembersRedis(const std::string& groupid);

    void UpdateGroupMembers(const std::string& groupid, const GrpMembersStatusMap&  members);
    void GetGroupMembersLocal(const std::string& groupid, std::unordered_set<std::string>** uset);

    bool FindTmpGroupId(const std::string& groupid);
    void TmpStoreGroupMsg(const std::string& groupid, int64 seqn, bool exist);
    void RemoveGroupMsg(const std::string& groupid);
    bool ProcessTmpGroupMsg(GRTTransferSession* pSess);



    bool IsServerReady() { return m_isServerReady; }
    void SetIsServerReady(bool ready) { m_isServerReady = ready; }


    bool    SignalKill();
    bool    ClearAll();
protected:
    GRTGrouperManager():m_isServerReady(false)
    {

    }

    ~GRTGrouperManager()
    {

    }
private:
    bool                        m_isServerReady;
    GroupMembersMap             m_MapGroupMembers;
    TmpGroupMsgMap              m_MapTmpGroupMsg;
    TmpGroupList                m_ListTmpGroup;

    OSMutex                     m_MutexGroupMembers;
    OSMutex                     m_MutexTmpGroupMsg;

    std::string                      m_Ip;
    int                              m_Port;
    int                              m_RedisNodeNum;
    xRedisClient                     m_xRedisClient;
    RedisDBIdx*                      m_RedisDBIdx;
    RedisNode*                       m_RedisList;
};

#endif /* defined(__MsgServerGrouper__GRTGrouperManager__) */
