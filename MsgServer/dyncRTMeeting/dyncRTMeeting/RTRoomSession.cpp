//
//  RTRoomSession.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/14/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTRoomSession.h"
#include "rtklog.h"
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "RTMessage.h"

RTRoomSession::RTRoomSession(const std::string sid)
: m_sessionId(sid)
{
    ListZero(&m_sessMemList);
}

RTRoomSession::~RTRoomSession()
{
    ListEmpty(&m_sessMemList);
}

bool RTRoomSession::AddMemberToSession(const std::string uid)
{
    OSMutexLocker locker(&m_mutex);
    if (IsMemberInSession(uid)) {
        LI("user: %s ALREADY in session: %s\n", uid.c_str(), m_sessionId.c_str());
        return false;
    } else {
        const char* str = (const char*)strdup(uid.c_str());
        ListAppend(&m_sessMemList, (void*)str, (int)strlen(str));
        LI("user: %s ADDOK session: %s\n", uid.c_str(), m_sessionId.c_str());
        return true;
    }
}

bool RTRoomSession::IsMemberInSession(const std::string uid)
{
    if (m_sessMemList.count==0) {
        return false;
    }
    return (!!ListFindItem(&m_sessMemList, (void*)uid.c_str(), stringcompare));
}

bool RTRoomSession::DelMemberFmSession(const std::string uid)
{
    OSMutexLocker locker(&m_mutex);
    if (IsMemberInSession(uid)) {
        if (ListRemoveItem(&m_sessMemList, (void*)uid.c_str(), stringcompare)) {
            LI("user: %s DELOK session: %s\n", uid.c_str(), m_sessionId.c_str());
            return true;
        } else {
            LE("user: %s DEL FAILED session: %s\n", uid.c_str(), m_sessionId.c_str());
            return false;
        }
    } else {
        LI("user: %s NOT in session: %s\n", uid.c_str(), m_sessionId.c_str());
        return false;
    }
}

int RTRoomSession::GetMembersInJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_sessMemList.count==0) {
        return -1;
    }
    ListElement* elem = NULL;
    TOJSONUSER touser;
    LI("room session members:%d\n", m_sessMemList.count);
    while (ListNextElement(&m_sessMemList, &elem)!=NULL) {
        LI("\t-->MeetingRoom members :%s\n", (char*)elem->content);
        if (strcmp(from.c_str(), (const char*)elem->content)!=0) {
            touser._us.push_front((const char*)elem->content);
        }
    }
    users = touser.ToJson();
    return 0;
}


void RTRoomSession::CheckMembers()
{
     OSMutexLocker locker(&m_mutex);
    if (m_sessMemList.count==0) {
        return;
    }
    ListElement* elem = NULL;
    LI("RoomSession members:%d\n",m_sessMemList.count);
    while (ListNextElement(&m_sessMemList, &elem)!=NULL) {
        LI("\tRoomMember :%s\n", (char*)elem->content);
    }
}

void RTRoomSession::ClearLostMember(const std::string& userid)
{
    DelMemberFmSession(userid);
}