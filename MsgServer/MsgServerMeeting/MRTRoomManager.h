//
//  MRTRoomManager.h
//  MsgServerMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerMeeting__MRTRoomManager__
#define __MsgServerMeeting__MRTRoomManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include "refcount.h"
#include "scoped_ref_ptr.h"
#include "MRTTransferSession.h"
#include "MRTMeetingRoom.h"
#include "MRTHttpSvrConn.h"
#include "MRTRoomDispatcher.h"
#include "RTEventTimer.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class MRTRoomManager : public RTSingleton< MRTRoomManager >{
    friend class RTSingleton< MRTRoomManager >;
public:

    void HandleOptRoom(pms::RelayMsg& rmsg, pms::Entity& emsg);
    void HandleOptRoomWithData(int cmd, pms::RelayMsg& rmsg, pms::Entity& emsg, std::string& data);
    void HandleDcommRoom(pms::RelayMsg& rmsg, pms::Entity& emsg);

    void EnterRoom(pms::RelayMsg& rmsg, pms::Entity& emsg);
    void LeaveRoom(pms::RelayMsg& rmsg, pms::Entity& emsg);

    bool Init(const std::string& msgQueueIp, unsigned short msgQueuePort, const std::string& httpIp, unsigned short httpPort, const std::string& httpHost);
    bool ConnectMsgQueue(const std::string& msgQueueIp, unsigned short msgQueuePort);
    bool TryConnectMsgQueue(const std::string& msgQueueIp, unsigned short msgQueuePort);
    bool ConnectHttpSvrConn(const std::string& httpIp, unsigned short httpPort, const std::string& httpHost);
    void SendTransferData(const std::string strData, int nLen);

    void CheckMembers();
    void SyncHttpRequest();
    void ClearSessionLost(const std::string& uid, const std::string& token, const std::string& connector);
    void ClearMsgQueueSession(const std::string& sid);
    bool ClearAll();

    void ProcessTickEvent(const char*pData, int nLen);
protected:
    MRTRoomManager()
                : m_pMsgQueueSession(NULL)
                , m_pHttpSvrConn(NULL)
                , m_pRoomDispatcher(NULL){}
    ~MRTRoomManager(){
        if (m_pHttpSvrConn) {
            delete m_pHttpSvrConn;
            m_pHttpSvrConn = NULL;
        }
    }


private:
    // <roomid, MeetingRoom> all the rooms map
    typedef std::unordered_map<std::string, rtc::scoped_refptr<MRTMeetingRoom> > MeetingRoomMap;
    typedef MeetingRoomMap::iterator MeetingRoomMapIt;

    // <userid, MeetingRoomId> in meeting user-roomid map
    typedef std::unordered_map<std::string, const std::string> UserMeetingRoomIdMap;
    typedef UserMeetingRoomIdMap::iterator UserMeetingRoomIdMapIt;

    int ChangeToJson(const std::string from, std::string& users);
    void SendWaitingMsgs(MeetingRoomMapIt mit);
    void OnGetMemberList(pms::RelayMsg& rmsg, pms::Entity& emsg, std::string& data);
    void GenericResponse(pms::EServerCmd cmd, const pms::RelayMsg& rmsg, const pms::Entity& emsg, const pms::ToUser* tos, std::string& response);
    void GenericConnLostResponse(const std::string& uid, const std::string& token, const std::string& roomid, const std::string& connector, int nmem, const std::string& cont, const pms::ToUser* tos, std::string& response);
    void ResponseSndMsg(pms::EServerCmd cmd, const pms::RelayMsg& rmsg, const pms::Entity& emsg, const pms::ToUser* tos, std::string& response);


    void AddUserMeetingRoomId(const std::string& uid, const std::string& roomid) {
        OSMutexLocker locker(&m_mutexUser);
        UserMeetingRoomIdMapIt uit = m_userMeetingRoomIdMap.find(uid);
        if (uit!=m_userMeetingRoomIdMap.end()) {
            m_userMeetingRoomIdMap.erase(uid);
        }
        m_userMeetingRoomIdMap.insert(make_pair(uid, roomid));
    }
    void DelUserMeetingRoomId(const std::string& uid) {
        OSMutexLocker locker(&m_mutexUser);
        UserMeetingRoomIdMapIt uit = m_userMeetingRoomIdMap.find(uid);
        if (uit!=m_userMeetingRoomIdMap.end()) {
            m_userMeetingRoomIdMap.erase(uid);
        }
    }
    const std::string GetUserMeetingRoomId(const std::string& uid) {
        OSMutexLocker locker(&m_mutexUser);
        UserMeetingRoomIdMapIt uit = m_userMeetingRoomIdMap.find(uid);
        if (uit!=m_userMeetingRoomIdMap.end()) {
            return uit->second;
        }
        return "";
    }

    OSMutex                     m_mutexUser;
    MRTHttpSvrConn*             m_pHttpSvrConn;
    MRTTransferSession*         m_pMsgQueueSession;
    MRTRoomDispatcher*          m_pRoomDispatcher;
    MeetingRoomMap              m_meetingRoomMap;
    UserMeetingRoomIdMap        m_userMeetingRoomIdMap;

};

#endif /* defined(__MsgServerMeeting__MRTRoomManager__) */
