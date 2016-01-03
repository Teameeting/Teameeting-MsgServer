//
//  RTRoomManager.h
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMeeting__RTRoomManager__
#define __dyncRTMeeting__RTRoomManager__

#include <stdio.h>
#include <iostream>
#include <map>
#include "refcount.h"
#include "scoped_ref_ptr.h"
#include "RTMeetMsg.h"
#include "RTTransferSession.h"
#include "RTMeetingRoom.h"
#include "RTHttpSvrConn.h"

class RTRoomManager{
public:
    static RTRoomManager* Instance() {
        static RTRoomManager s_manager;
        return &s_manager;
    }
    static std::string      s_msgQueueIp;
    static unsigned short   s_msgQueuePort;
    static std::string      s_httpIp;
    static unsigned short   s_httpPort;
    static std::string      s_httpHost;
public:
    int HandleOptRoom(MEETMSG& msg, std::string& tos, std::string& res);
    int HandleDcommRoom(MEETMSG& msg, std::string& tos, std::string& res);
    
    void GenericMeetingSessionId(std::string& strId);
    bool Init();
    bool ConnectMsgQueue();
    bool ConnectHttpSvrConn();
    void SendTransferData(const std::string strData, int nLen);
    void RefreshConnection();
    
    void CheckMembers();
    void SyncHttpRequest();
    void ClearSessionLost(const std::string& uid);
    void ClearMsgQueueSession(const std::string& sid);
private:
    int EnterRoom(MEETMSG& msg, std::string& tos, std::string& res);
    int LeaveRoom(MEETMSG& msg, std::string& tos, std::string& res);
    int CreateRoom(MEETMSG& msg, std::string& tos, std::string& res);
    int DestroyRoom(MEETMSG& msg, std::string& tos, std::string& res);
    int RefreshRoom(MEETMSG& msg, std::string& tos, std::string& res);
    
    typedef std::map<const std::string, rtc::scoped_refptr<RTMeetingRoom> > MeetingRoomMap;
    RTRoomManager():m_pMsgQueueSession(NULL)
                , m_pHttpSvrConn(NULL){
        
    }
    ~RTRoomManager(){
        if (m_pHttpSvrConn) {
            delete m_pHttpSvrConn;
            m_pHttpSvrConn = NULL;
        }
        if (m_pMsgQueueSession) {
            delete m_pMsgQueueSession;
            m_pMsgQueueSession = NULL;
        }
        //TODO:
        //delete map members;
    }
    
    int ChangeToJson(const std::string from, std::string& users);
    
    RTTransferSession    *m_pMsgQueueSession;
    MeetingRoomMap       m_meetingRoomMap;
    OSMutex                   m_mutexHttp;
    RTHttpSvrConn        *m_pHttpSvrConn;
};

#endif /* defined(__dyncRTMeeting__RTRoomManager__) */
