//
//  RTConnectionManager.h
//  dyncRTConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTConnector__RTConnectionManager__
#define __dyncRTConnector__RTConnectionManager__

#include <stdio.h>
#include <iostream>
#include <map>
#include <list>
#include "RTTransferSession.h"
#include "RTMessage.h"
#include "RTTcp.h"
#include "RTType.h"

#define HR_USERID       "hr_userid"
#define HR_CONNECTORID  "hr_connectorid"

class RTConnectionManager {
public:
    //store the session's module type and transfer_session_id and transfer_session
    typedef struct _ModuleInfo{
        int             flag;
        TRANSFERMODULE  othModuleType;
        std::string     othModuleId;
        RTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = transfermodule_invalid;
            othModuleId = "";
            pModule = NULL;
        }
    }ModuleInfo;
    
    //store the user id and connection
    typedef struct _ConnectionInfo{
        std::string     connId;
        std::string     connAddr;
        std::string     connPort;
        std::string     userId;
        RTTcp*          pConn;
        CONNECTIONTYPE  connType;
        int             flag;
        _ConnectionInfo() {
            connId = "";
            connAddr = "";
            connPort = "";
            userId = "";
            pConn = NULL;
            connType = connectiontype_invalid;
            flag = 0;
        }
    }ConnectionInfo;
    
    //store  module usage information
    //meet meet123456  session123456
    //msgqueue queue123456  session234567
    typedef struct _TypeModuleSessionInfo{
        TRANSFERMODULE moduleType;
        std::string moduleId;
        std::list<std::string> sessionIds;
        _TypeModuleSessionInfo() {
            moduleType = transfermodule_invalid;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;
    
    //match the module type and session id
    typedef struct _TypeSessionInfo{
        TRANSFERMODULE moduleType;
        std::string sessionId;
        _TypeSessionInfo() {
            moduleType = transfermodule_invalid;
            sessionId = "";
        }
    }TypeSessionInfo;
    
    //store which session the user use most
    typedef struct _UserSessionInfo{
        std::string userId;
        std::list<TypeSessionInfo> typeSessionInfo;
        _UserSessionInfo() {
            userId = "";
            typeSessionInfo.clear();
        }
    }UserSessionInfo;
    
    //<transfer_session_id, module_info>
    typedef std::map< std::string, ModuleInfo* >     ModuleInfoMaps;
    //<user_id, connection_info>
    typedef std::map< std::string, ConnectionInfo* > ConnectionInfoMaps;
    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*> TypeModuleSessionInfoLists;
    
    //check list and map which is better
    typedef std::list<UserSessionInfo*> UserSessionInfoLists;
    
    typedef std::map<std::string, std::list<TypeSessionInfo*> > UserSessionInfoMaps;
    
    
    static ModuleInfoMaps*  GetModuleInfo() {
        static ModuleInfoMaps  s_ModuleInfoMap;
        return &s_ModuleInfoMap;
    }
    
    static ConnectionInfoMaps* GetConnectionInfo() {
        static ConnectionInfoMaps s_ConnectionInfoMap;
        return &s_ConnectionInfoMap;
    }
    
    static TypeModuleSessionInfoLists* GetTypeModuleSessionInfo() {
        static TypeModuleSessionInfoLists s_TypeModuleSessionInfoList;
        return &s_TypeModuleSessionInfoList;
    }
    
    static UserSessionInfoLists* GetUserSessionInfoList() {
        static UserSessionInfoLists s_UserSessionInfoList;
        return &s_UserSessionInfoList;
    }
    
    static UserSessionInfoMaps* GetUserSessionInfoMap() {
        static UserSessionInfoMaps s_UserSessionInfoMap;
        return &s_UserSessionInfoMap;
    }
    
    /**
     *
     */
    static RTConnectionManager* Instance() {
        static RTConnectionManager s_manager;
        return &s_manager;
    }
    
    void    GenericSessionId(std::string& strId);
    void    SetConnectorInfo(const char* Ip, unsigned short port, const char* Id) { m_connectorIp = Ip;
        m_connectorPort = port;
        m_connectorId = Id;
    }
    
    ModuleInfo*       findModuleInfo(const std::string& userid, TRANSFERMODULE module);
    ConnectionInfo*   findConnectionInfoById(const std::string& cid);
    
    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid);
    bool AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);
    bool AddUser(CONNECTIONTYPE type, const std::string& uid, ConnectionInfo* pInfo);
    bool DelUser(CONNECTIONTYPE type, const std::string& uid);
    
    void ConnectionLostNotify(const std::string& uid, const std::string& token);
    void TransferSessionLostNotify(const std::string& sid);
    
    std::string& ConnectorIp() { return m_connectorIp; }
    std::string& ConnectorPort() { return m_connectorPort; }
    std::string& ConnectorId() { return m_connectorId; }
private:
    RTConnectionManager() : m_connectorIp(""),
                            m_connectorPort(""),
                            m_connectorId(""),
                            m_lastUpdateTime("RTConnectionManager") { }
    ~RTConnectionManager() { }
    std::string m_connectorIp;
    std::string m_connectorPort;
    std::string m_connectorId;
    std::string m_lastUpdateTime;
};

#endif /* defined(__dyncRTConnector__RTConnectionManager__) */
