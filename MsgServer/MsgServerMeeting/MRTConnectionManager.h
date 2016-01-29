//
//  MRTConnectionManager.h
//  MsgServerMeeting
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerMeeting__MRTConnectionManager__
#define __MsgServerMeeting__MRTConnectionManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "RTMessage.h"

class MRTTransferSession;

class MRTConnectionManager {
public:
    typedef struct _ModuleInfo{
        int             flag;
        TRANSFERMODULE  othModuleType;
        std::string     othModuleId;
        MRTTransferSession*     pModule;
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

    typedef std::unordered_map< std::string, ModuleInfo* >     ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator ModuleInfoMapsIt;

    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*> TypeModuleSessionInfoLists;

    //check list and map which is better
    typedef std::list<UserSessionInfo*> UserSessionInfoLists;

    typedef std::unordered_map<std::string, std::list<TypeSessionInfo*> > UserSessionInfoMaps;
    typedef UserSessionInfoMaps::iterator UserSessionInfoMapsIt;


    static MRTConnectionManager* Instance() {
        static MRTConnectionManager s_manager;
        return &s_manager;
    }

    ModuleInfo*  findModuleInfo(const std::string& userid, TRANSFERMODULE module);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid);
    bool AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    void TransferSessionLostNotify(const std::string& sid);

    bool    ConnectConnector();
    void    RefreshConnection();

    void SetMeetingId(const std::string& mid) { m_meetingId = mid; }
    std::string& MeetingId() { return m_meetingId; }
    std::list<std::string>* GetAddrsList() { return &m_ipList; }
private:
    MRTConnectionManager() {}
    ~MRTConnectionManager() {}
    bool DoConnectConnector(const std::string ip, unsigned short port);
    std::list<std::string>    m_ipList;
    std::string               m_meetingId;

};

#endif /* defined(__MsgServerMeeting__MRTConnectionManager__) */
