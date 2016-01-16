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
#include <map>
#include "refcount.h"
#include "scoped_ref_ptr.h"
#include "RTMeetMsg.h"
#include "RTSignalMsg.h"
#include "MRTTransferSession.h"
#include "MRTMeetingRoom.h"
#include "MRTHttpSvrConn.h"
#include "MRTHttpCmd.h"

class MRTRoomManager{
public:
    static MRTRoomManager* Instance() {
        static MRTRoomManager s_manager;
        return &s_manager;
    }
    static std::string      s_msgQueueIp;
    static unsigned short   s_msgQueuePort;
    static std::string      s_httpIp;
    static unsigned short   s_httpPort;
    static std::string      s_httpHost;
public:
    
    void HandleOptRoom(TRANSMSG& tmsg, MEETMSG& mmsg);
    void HandleOptRoomWithData(HTTPCMD cmd, TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data);
    void HandleDcommRoom(TRANSMSG& tmsg, MEETMSG& mmsg);

    void EnterRoom(TRANSMSG& tmsg, MEETMSG& mmsg);
    void LeaveRoom(TRANSMSG& tmsg, MEETMSG& mmsg);
    
    bool Init();
    bool ConnectMsgQueue();
    bool ConnectHttpSvrConn();
    void SendTransferData(const std::string strData, int nLen);
    void RefreshConnection();
    const MRTHttpSvrConn* GetHttpSvrConn() { return m_pHttpSvrConn; }
    
    void CheckMembers();
    void SyncHttpRequest();
    void ClearSessionLost(const std::string& uid, const std::string& token, const std::string& connector);
    void ClearMsgQueueSession(const std::string& sid);
private:
    void OnGetMemberList(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data);
    
    int GenericTransSeq();
    void GenericResponse(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, const std::string& res,  std::string& response);
    void GenericConnLostResponse(const std::string& uid, const std::string& token, const std::string& roomid, const std::string& connector, SENDTAGS tags, int nmem, const std::string& cont, const std::string& tos, std::string& response);
    void ResponseNotDcomm(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, const std::string& res, std::string& response);
    void ResponseDcomm(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, const std::string& res, std::string& response);
    
    typedef std::map<const std::string, rtc::scoped_refptr<MRTMeetingRoom> > MeetingRoomMap;
    MRTRoomManager():m_pMsgQueueSession(NULL)
                , m_pHttpSvrConn(NULL){
        
    }
    ~MRTRoomManager(){
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
    
    MRTTransferSession    *m_pMsgQueueSession;
    MeetingRoomMap       m_meetingRoomMap;
    OSMutex                   m_mutexHttp;
    MRTHttpSvrConn        *m_pHttpSvrConn;
};

#endif /* defined(__MsgServerMeeting__MRTRoomManager__) */
