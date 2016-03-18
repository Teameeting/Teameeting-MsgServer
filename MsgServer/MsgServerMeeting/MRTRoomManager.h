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
#include "RTMeetMsg.h"
#include "RTSignalMsg.h"
#include "MRTTransferSession.h"
#include "MRTMeetingRoom.h"
#include "MRTHttpSvrConn.h"
#include "RTDispatch.h"
#include "RTEventTimer.h"

class MRTRoomManager : public RTDispatch{
public:
    static MRTRoomManager* Instance() {
        static MRTRoomManager s_manager;
        return &s_manager;
    }
public:
    
    void HandleOptRoom(TRANSMSG& tmsg, MEETMSG& mmsg);
    void HandleOptRoomWithData(int cmd, TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data);
    void HandleDcommRoom(TRANSMSG& tmsg, MEETMSG& mmsg);

    void EnterRoom(TRANSMSG& tmsg, MEETMSG& mmsg);
    void LeaveRoom(TRANSMSG& tmsg, MEETMSG& mmsg);
    
    bool Init(const std::string& msgQueueIp, unsigned short msgQueuePort, const std::string& httpIp, unsigned short httpPort, const std::string& httpHost);
    bool ConnectMsgQueue(const std::string& msgQueueIp, unsigned short msgQueuePort);
    bool TryConnectMsgQueue(const std::string& msgQueueIp, unsigned short msgQueuePort);
    bool ConnectHttpSvrConn(const std::string& httpIp, unsigned short httpPort, const std::string& httpHost);
    void SendTransferData(const std::string strData, int nLen);
    
    void CheckMembers();
    void SyncHttpRequest();
    void ClearSessionLost(const std::string& uid, const std::string& token, const std::string& connector);
    void ClearMsgQueueSession(const std::string& sid);
    
    // for RTDispatch
    virtual void OnRecvEvent(const char*pData, int nLen) {}
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen);
    
private:
    // <roomid, MeetingRoom> all the rooms map
    typedef std::unordered_map<std::string, rtc::scoped_refptr<MRTMeetingRoom> > MeetingRoomMap;
    typedef MeetingRoomMap::iterator MeetingRoomMapIt;
    
    // <userid, MeetingRoomId> in meeting user-roomid map
    typedef std::unordered_map<std::string, const std::string> UserMeetingRoomIdMap;
    typedef UserMeetingRoomIdMap::iterator UserMeetingRoomIdMapIt;
    
    int ChangeToJson(const std::string from, std::string& users);
    void SendWaitingMsgs(MeetingRoomMapIt mit);
    void OnGetMemberList(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data);
    void GenericResponse(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, std::string& response);
    void GenericConnLostResponse(const std::string& uid, const std::string& token, const std::string& roomid, const std::string& connector, SENDTAGS tags, int nmem, const std::string& cont, const std::string& tos, std::string& response);
    void ResponseSndMsg(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, std::string& response);
    
    
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
    
    MRTRoomManager(): RTDispatch()
                , m_pMsgQueueSession(NULL)
                , m_pHttpSvrConn(NULL){}
    ~MRTRoomManager(){
        if (m_pHttpSvrConn) {
            delete m_pHttpSvrConn;
            m_pHttpSvrConn = NULL;
        }
        if (m_pMsgQueueSession) {
            delete m_pMsgQueueSession;
            m_pMsgQueueSession = NULL;
        }
#if 0
        {
            MeetingRoomMapIt mit = m_meetingRoomMap.begin();
            for (; mit!=m_meetingRoomMap.end(); mit++) {
                mit->second->Release();
                delete mit->second;
                m_meetingRoomMap.erase(mit);
            }
            m_meetingRoomMap.clear();
        }
        
        {
            OSMutexLocker locker(&m_mutexUser);
            UserMeetingRoomIdMapIt uit = m_userMeetingRoomIdMap.begin();
            for (;uit!=m_userMeetingRoomIdMap.end();uit++) {
                m_userMeetingRoomIdMap.erase(uit);
            }
            m_userMeetingRoomIdMap.clear();
        }
#endif
        //TODO:
        //delete map members;
    }
    
    OSMutex                     m_mutexUser;
    MRTHttpSvrConn*             m_pHttpSvrConn;
    MRTTransferSession*         m_pMsgQueueSession;
    MeetingRoomMap              m_meetingRoomMap;
    UserMeetingRoomIdMap        m_userMeetingRoomIdMap;
    
};

#endif /* defined(__MsgServerMeeting__MRTRoomManager__) */
