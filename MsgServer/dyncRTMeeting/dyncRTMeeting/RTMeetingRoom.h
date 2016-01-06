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
    bool AddMemberToRoom(const std::string uid);
    bool IsMemberInRoom(const std::string uid);
    bool DelMemberFmRomm(const std::string uid);
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
    
    int GetSessionMemberInJson(const std::string from, std::string& users) { if(m_pRoomSession) return m_pRoomSession->GetMembersInJson(from, users); else return 0; }
    void ClearLostMember(const std::string& userid) { if (m_pRoomSession) m_pRoomSession->ClearLostMember(userid); }
    
    const std::string& GetOwnerId() { return m_ownerId; }
public:
    void CheckMembers();
private:
    OSMutex                         m_mutex;
    const std::string               m_roomId;
    const std::string               m_ownerId;
    std::string                     m_sessionId;
    RTRoomSession                   *m_pRoomSession;
    List                            m_roomMemList;
    
};
#endif /* defined(__dyncRTMeeting__RTMeetingRoom__) */
