//
//  RTMeetingRoom.h
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMeeting__RTMeetingRoom__
#define __dyncRTMeeting__RTMeetingRoom__

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <list>
#include "LinkedList.h"
#include "refcount.h"
#include "scoped_ptr.h"
#include "scoped_ref_ptr.h"
#include "RTRoomSession.h"
#include "OSMutex.h"

class RTMeetingRoom : public rtc::RefCountedObject< rtc::scoped_ptr<RTMeetingRoom> >{
public:
    RTMeetingRoom(const std::string mid, const std::string ownerid);
    ~RTMeetingRoom();
    
    typedef struct _notify_msgs{
        int seq;
        long long pubTime;
        std::string notifyMsg;
        std::string publisher;
        _notify_msgs() {
            seq = 0;
            pubTime = 0;
            notifyMsg = "";
            publisher = "";
        }
    }NotifyMsg;
    
    typedef enum _get_members_status {
        GMS_NIL = 0,
        GMS_WAITTING,
        GMS_DONE
    }GetMembersStatus;
    
    typedef std::map<std::string, NotifyMsg*>  RoomNotifyMsgs;
    
    bool AddMemberToRoom(const std::string uid);
    bool IsMemberInRoom(const std::string uid);
    bool DelMemberFmRoom(const std::string uid);
    bool AddMemberToSession(const std::string sid, const std::string uid);
    bool DelMemberFmSession(const std::string sid, const std::string uid);
    int  GetRoomMemberNumber() { return (int)m_roomMemList.count; }
    int  GetSesssionMemberNumber() {
        if (m_pRoomSession)
            return m_pRoomSession->GetSessionMemberNumber();
        else
            return 0;
    }
    const std::string& GetSessionId() { return m_sessionId; }
    void CreateSession();
    void DestroySession();
    void SetGetMembersStatus(GetMembersStatus status){m_eGetMembersStatus = status;};
    void UpdateUserList(std::list<const std::string>& ulist);
    void AddUserToList(const std::string& userid);
    
    int GetRoomMemberInJson(const std::string from, std::string& users);
    int GetSessionMemberInJson(const std::string from, std::string& users) { if(m_pRoomSession) return m_pRoomSession->GetMembersInJson(from, users); else return 0; }
    void ClearLostMember(const std::string& userid) { if (m_pRoomSession) m_pRoomSession->ClearLostMember(userid); }
    
    int AddNotifyMsg(const std::string pubsher, const std::string msg);
    int DelNotifyMsg(const std::string pubsher);
    RoomNotifyMsgs& GetRoomNotifyMsgsMap() { return m_roomNotifyMsgs; }
    
    const std::string& GetOwnerId() { return m_ownerId; }
    
    
public:
    void CheckMembers();
    
private:
    int GenericNotifySeq();
    void GenericMeetingSessionId(std::string& strId);
    
    
    OSMutex                         m_mutex;
    OSMutex                         m_notifyMutex;
    const std::string               m_roomId;
    const std::string               m_ownerId;
    std::string                     m_sessionId;
    RTRoomSession                   *m_pRoomSession;
    GetMembersStatus                m_eGetMembersStatus;
    List                            m_roomMemList;
    int                             m_maxRoomMem;
    RoomNotifyMsgs                  m_roomNotifyMsgs;
    
};
#endif /* defined(__dyncRTMeeting__RTMeetingRoom__) */
