//
//  PRTConnManager.h
//  MsgServerPusher
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerPusher__PRTConnManager__
#define __MsgServerPusher__PRTConnManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "OSMutex.h"
#include "RTSingleton.h"
#include "RTType.h"
#include "PRTConnDispatcher.h"
#include "RTEventTimer.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class PRTTransferSession;

class PRTConnManager : public RTSingleton< PRTConnManager >{
    friend class RTSingleton< PRTConnManager >;
public:
    typedef struct _ModuleInfo{
        int             flag;
        pms::ETransferModule  othModuleType;
        std::string     othModuleId;
        PRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = pms::ETransferModule::MPUSHER;
            othModuleId = "";
            pModule = NULL;
        }
    }ModuleInfo;

    //store  module usage information
    //meet meet123456  session123456
    //msgqueue queue123456  session234567
    typedef struct _TypeModuleSessionInfo{
        pms::ETransferModule moduleType;
        std::string moduleId;
        std::set<std::string> sessionIds;
        _TypeModuleSessionInfo() {
            moduleType = pms::ETransferModule::MPUSHER;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;

    typedef std::unordered_map< std::string, ModuleInfo* >      ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator                            ModuleInfoMapsIt;

    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*> TypeModuleSessionInfoLists;

    typedef std::unordered_multimap<std::string, std::string>        UserConnectorMaps;
    typedef UserConnectorMaps::iterator UserConnectorMapsIt;

    typedef std::list< PRTTransferSession* > ConnectingSessList;

    static std::string      s_cohttpIp;
    static unsigned short   s_cohttpPort;
    static std::string      s_cohttpHost;

    ModuleInfo*       findConnectorInfo(const std::string& userid);
    ModuleInfo*       findModuleInfo(const std::string& userid, pms::ETransferModule module);
    ModuleInfo*       findModuleInfoBySid(const std::string& sid);
    ModuleInfo*       findConnectorInfoById(const std::string& userid, const std::string& connector);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid, EventData& data);
    bool AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    void TransferSessionLostNotify(const std::string& sid);

    bool    ConnectConnector();
    bool TryConnectConnector(const std::string ip, unsigned short port);


    std::list<std::string>* GetConnectorAddrList() { return &m_connectorAddrList; }

    void    RefreshConnection();
    void    SendTransferData(const std::string mid, const std::string uid, const std::string msg);

    void SetModuleId(const std::string& mid) { m_logicalId = mid; }
    std::string& ModuleId() { return m_logicalId; }
    bool    SignalKill();
    bool    ClearAll();

    void OnTLogin(const std::string& uid, const std::string& token, const std::string& connector);
    void OnTLogout(const std::string& uid, const std::string& token, const std::string& connector);

    void ProcessRecvEvent(const char*pData, int nLen);
    void ProcessTickEvent(const char*pData, int nLen);
    void PostDataStatic(const char* pData, int nLen);

    // for RTEventTimer
    static int DispTimerCallback(const char*pData, int nLen);
protected:
    PRTConnManager():m_pConnDispatcher(NULL) { }
    ~PRTConnManager() { }
private:
    bool DoConnectConnector(const std::string ip, unsigned short port);
    std::list<std::string>    m_connectorAddrList;
    std::string               m_logicalId;
    OSMutex                   m_mutexMembers;
    PRTConnDispatcher*        m_pConnDispatcher;
    UserConnectorMaps         m_userConnectors;
    ConnectingSessList        m_connectingSessList;
};

#endif /* defined(__MsgServerPusher__PRTConnManager__) */
