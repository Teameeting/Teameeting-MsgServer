//
//  DRTConnectionManager.h
//  MsgServerDispatcher
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerDispatcher__DRTConnectionManager__
#define __MsgServerDispatcher__DRTConnectionManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "RTMessage.h"
#include "OSMutex.h"
#include "DRTHttpSvrConn.h"

class DRTTransferSession;

class DRTConnectionManager {
public:
    typedef struct _ModuleInfo{
        int             flag;
        TRANSFERMODULE  othModuleType;
        std::string     othModuleId;
        DRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = transfermodule_invalid;
            othModuleId = "";
            pModule = NULL;
        }
    }ModuleInfo;

    //store  module usage information
    //meet meet123456  session123456
    //msgqueue queue123456  session234567
    typedef struct _TypeModuleSessionInfo{
        TRANSFERMODULE moduleType;
        std::string moduleId;
        std::set<std::string> sessionIds;
        _TypeModuleSessionInfo() {
            moduleType = transfermodule_invalid;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;

    //1 match the module type and session id
    typedef struct _TypeSessionInfo{
        TRANSFERMODULE moduleType;
        std::string sessionId;
        _TypeSessionInfo() {
            moduleType = transfermodule_invalid;
            sessionId = "";
        }
    }TypeSessionInfo;

    //2 store which session the user use most
    typedef struct _UserSessionInfo{
        std::string userId;
        std::list<TypeSessionInfo> typeSessionInfo;
        _UserSessionInfo() {
            userId = "";
            typeSessionInfo.clear();
        }
    }UserSessionInfo;

    typedef std::unordered_map< std::string, ModuleInfo* >      ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator                            ModuleInfoMapsIt;

    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*> TypeModuleSessionInfoLists;

    //check list and map which is better
    typedef std::list<UserSessionInfo*> UserSessionInfoLists;

    typedef std::unordered_map<std::string, std::list<TypeSessionInfo*> > UserSessionInfoMaps;
    typedef UserSessionInfoMaps::iterator UserSessionInfoMapsIt;

    typedef std::set<std::string>    OnlineMembers;//all the members online
    typedef OnlineMembers::iterator      OnlineMembersIt;
    typedef std::set<std::string>    OfflineMembers;//all the members offline
    typedef OfflineMembers::iterator     OfflineMembersIt;

    static DRTConnectionManager* Instance() {
        static DRTConnectionManager s_manager;
        return &s_manager;
    }
    static std::string      s_cohttpIp;
    static unsigned short   s_cohttpPort;
    static std::string      s_cohttpHost;

    ModuleInfo*       findConnectorInfo(const std::string& userid);
    ModuleInfo*       findModuleInfo(const std::string& userid, TRANSFERMODULE module);
    ModuleInfo*       findModuleInfoBySid(const std::string& sid);
    ModuleInfo*       findConnectorInfoById(const std::string& userid, const std::string& connector);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid);
    bool AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    void TransferSessionLostNotify(const std::string& sid);

    bool    ConnectConnector();
    std::list<std::string>* GetAddrsList() { return &m_ipList; }
    void    RefreshConnection();
    void    SendTransferData(const std::string mid, const std::string uid, const std::string msg);
    void SetMsgQueueId(const std::string& mid) { m_msgQueueId = mid; }
    std::string& MsgQueueId() { return m_msgQueueId; }

    void AddMemberToOnline(const std::string& uid);
    bool IsMemberInOnline(const std::string& uid);
    void DelMemberFmOnline(const std::string& uid);
    int  GetOnlineNumber() { return (int)m_onlineMembers.size(); }
    
    void AddMemberToOffline(const std::string& uid);
    bool IsMemberInOffline(const std::string& uid);
    void DelMemberFmOffline(const std::string& uid);
    int  GetOfflineNumber() { return (int)m_offlineMembers.size(); }
    
    void OnTLogin(const std::string& uid, const std::string& token);
    void OnTLogout(const std::string& uid, const std::string& token);
    
    bool ConnectHttpSvrConn();
    void PushMeetingMsg(const std::string& sign, const std::string& meetingid, const std::string& pushMsg, const std::string& notification, const std::string& extra);
    void PushCommonMsg(const std::string& sign, const std::string& targetid, const std::string& pushMsg, const std::string& notification, const std::string& extra);
    
private:
    DRTConnectionManager()
        : m_pHttpSvrConn(NULL) { }
    ~DRTConnectionManager() {}
    bool DoConnectConnector(const std::string ip, unsigned short port);
    std::list<std::string>    m_ipList;
    std::string               m_msgQueueId;
    OnlineMembers             m_onlineMembers;
    OfflineMembers            m_offlineMembers;
    OSMutex                   m_mutexMembers;
    DRTHttpSvrConn*           m_pHttpSvrConn;

};

#endif /* defined(__MsgServerDispatcher__DRTConnectionManager__) */
