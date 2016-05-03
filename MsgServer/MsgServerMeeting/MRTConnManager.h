//
//  MRTConnManager.h
//  MsgServerMeeting
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerMeeting__MRTConnManager__
#define __MsgServerMeeting__MRTConnManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "RTMessage.h"
#include "MRTConnDispatcher.h"
#include "RTEventTimer.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/meet_msg.pb.h"
#include "MsgServer/proto/meet_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"

class MRTTransferSession;

class MRTConnManager : public RTSingleton< MRTConnManager >{
    friend class RTSingleton< MRTConnManager >;
public:
    typedef struct _ModuleInfo{
        int             flag;
        pms::ETransferModule  othModuleType;
        std::string     othModuleId;
        MRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = pms::ETransferModule::MMEETING;
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
            moduleType = pms::ETransferModule::MMEETING;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;

    //match the module type and session id
    typedef struct _TypeSessionInfo{
        pms::ETransferModule moduleType;
        std::string sessionId;
        _TypeSessionInfo() {
            moduleType = pms::ETransferModule::MMEETING;
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

    typedef std::list< MRTTransferSession* > ConnectingSessList;


    ModuleInfo*  findModuleInfo(const std::string& userid, pms::ETransferModule module);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid, EventData& data);
    bool AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    void TransferSessionLostNotify(const std::string& sid);

    bool    ConnectConnector();
    void    RefreshConnection();
    bool    SignalKill();
    bool    ClearAll();

    void SetMeetingId(const std::string& mid) { m_meetingId = mid; }
    std::string& MeetingId() { return m_meetingId; }
    std::list<std::string>* GetAddrsList() { return &m_ipList; }

    void ProcessRecvEvent(const char*pData, int nLen);
    void ProcessTickEvent(const char*pData, int nLen);
    void PostDataStatic(const char* pData, int nLen);

    // for RTEventTimer
    static int ConnTimerCallback(const char*pData, int nLen);
protected:
    MRTConnManager():m_pConnDispatcher(NULL){}
    ~MRTConnManager(){}

private:
    bool DoConnectConnector(const std::string ip, unsigned short port);
    bool TryConnectConnector(const std::string ip, unsigned short port);
    std::list<std::string>    m_ipList;
    std::string               m_meetingId;
    ConnectingSessList        m_connectingSessList;
    MRTConnDispatcher*        m_pConnDispatcher;

};

#endif /* defined(__MsgServerMeeting__MRTConnManager__) */
