//
//  RTRoomSession.h
//  dyncRTMeeting
//
//  Created by hp on 12/14/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMeeting__RTRoomSession__
#define __dyncRTMeeting__RTRoomSession__

#include <stdio.h>
#include <iostream>
#include <list>
#include "LinkedList.h"
#include "OSMutex.h"

class RTRoomSession{
public:
    RTRoomSession(const std::string sid);
    ~RTRoomSession();
    
    bool AddMemberToSession(const std::string uid);
    bool IsMemberInSession(const std::string uid);
    bool DelMemberFmSession(const std::string uid);
    const List* GetMemberList() { return &m_sessMemList; }
    int  GetSessionMemberNumber() { return (int)m_sessMemList.count; }
    int GetMembersInJson(const std::string from, std::string& users);
    void ClearLostMember(const std::string& userid);
public:
    void CheckMembers();
private:
    OSMutex                        m_mutex;
    std::string                    m_sessionId;
    List                           m_sessMemList;
};

#endif /* defined(__dyncRTMeeting__RTRoomSession__) */
