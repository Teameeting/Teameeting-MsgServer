//
//  CRTConnectionManager.h
//  MsgServerConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTConnectionManager__
#define __MsgServerConnector__CRTConnectionManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "CRTTransferSession.h"
#include "RTMessage.h"
#include "RTTcp.h"
#include "RTType.h"

#define HR_USERID       "hr_userid"
#define HR_CONNECTORID  "hr_connectorid"

class CRTConnectionManager {
public:
    //store the session's module type and transfer_session_id and transfer_session
    typedef struct _ModuleInfo{
        int             flag;
        TRANSFERMODULE  othModuleType;
        std::string     othModuleId;
        CRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = transfermodule_invalid;
            othModuleId = "";
            pModule = NULL;
        }
    }ModuleInfo;

    //store the user id and connection
    typedef struct _ConnectionInfo{
        std::string     _connId;
        std::string     _connAddr;
        std::string     _connPort;
        std::string     _userId;
        std::string     _token;
        RTTcp*          _pConn;
        CONNECTIONTYPE  _connType;
        int             _flag;
        _ConnectionInfo():_connId("")
                        , _connAddr("")
                        , _connPort("")
                        , _userId("")
                        , _token("")
                        , _pConn(NULL)
                        , _connType(connectiontype_invalid)
                        , _flag(0){
        }
    }ConnectionInfo;

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
    typedef std::unordered_map< std::string, ModuleInfo* >      ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator                            ModuleInfoMapsIt;
    //<user_id, connection_info>
    typedef std::unordered_map< std::string, ConnectionInfo* >  ConnectionInfoMaps;
    typedef ConnectionInfoMaps::iterator                        ConnectionInfoMapsIt;
    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*>                   TypeModuleSessionInfoLists;

    //check list and map which is better
    typedef std::list<UserSessionInfo*>                         UserSessionInfoLists;

    typedef std::unordered_map<std::string, std::list<TypeSessionInfo*> >   UserSessionInfoMaps;
    typedef UserSessionInfoMaps::iterator                                   UserSessionInfoMapsIt;


    /**
     *
     */
    static CRTConnectionManager* Instance() {
        static CRTConnectionManager s_manager;
        return &s_manager;
    }
    static std::string      s_cohttpIp;
    static unsigned short   s_cohttpPort;
    static std::string      s_cohttpHost;

    void    SetConnectorInfo(const char* Ip, unsigned short port, const char* Id) { m_connectorIp = Ip;
        m_connectorPort = port;
        m_connectorId = Id;
    }

    ModuleInfo*       findModuleInfo(const std::string& userid, TRANSFERMODULE module);
    ConnectionInfo*   findConnectionInfoById(const std::string& uid);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid);
    bool AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid);

    //if session lost, del session
    //if programer lost, del module
    bool DelTypeModuleSession(const std::string& sid);
    bool AddUser(CONNECTIONTYPE type, const std::string& uid, ConnectionInfo* pInfo);
    // return true means delete one
    // return false means not delete
    bool DelUser(CONNECTIONTYPE type, const std::string& uid, std::string& token);

    void ConnectionLostNotify(const std::string& uid, const std::string& token);
    void ConnectionConnNotify(const std::string& uid, const std::string& token);
    void TransferSessionLostNotify(const std::string& sid);
    void TransferMsg(MSGTYPE mType, long long mseq, const std::string& uid, const std::string& msg);
    
    std::string& ConnectorIp() { return m_connectorIp; }
    std::string& ConnectorPort() { return m_connectorPort; }
    std::string& ConnectorId() { return m_connectorId; }
public:
    void ShowConnectionInfo();
private:
    CRTConnectionManager() : m_connectorIp(""),
                            m_connectorPort(""),
                            m_connectorId("") { }
    ~CRTConnectionManager() { }
    std::string m_connectorIp;
    std::string m_connectorPort;
    std::string m_connectorId;
};

#endif /* defined(__MsgServerConnector__CRTConnectionManager__) */
