//
//  RTMeetingRoom.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTMeetingRoom.h"
#include "rtklog.h"
#include "RTRoomManager.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"

RTMeetingRoom::RTMeetingRoom(const std::string mid, const std::string ownerid)
: m_roomId(mid)
, m_sessionId("")
, m_ownerId(ownerid)
, m_pRoomSession(NULL)
{
    ListZero(&m_roomMemList);
}

RTMeetingRoom::~RTMeetingRoom()
{
    if (m_pRoomSession) {
        delete m_pRoomSession;
        m_pRoomSession = NULL;
    }
    ListEmpty(&m_roomMemList);
}

bool RTMeetingRoom::AddMemberToRoom(const std::string uid)
{
    OSMutexLocker locker(&m_mutex);
    if (IsMemberInRoom(uid)) {
        LI("user: %s ALREADY in room: %s\n", uid.c_str(), m_roomId.c_str());
        return false;
    } else {
        const char* str = (const char*)strdup(uid.c_str());
        ListAppend(&m_roomMemList, (void*)str, (int)strlen(str));
        LI("user: %s ADDOK room: %s\n", uid.c_str(), m_roomId.c_str());
        return true;
    }
}

bool RTMeetingRoom::IsMemberInRoom(const std::string uid)
{
    if (m_roomMemList.count==0) {
        return false;
    }
    return (!!ListFindItem(&m_roomMemList, (void*)uid.c_str(), stringcompare));
}


bool RTMeetingRoom::DelMemberFmRomm(const std::string uid)
{
    OSMutexLocker locker(&m_mutex);
    if (IsMemberInRoom(uid)) {
        if (ListRemoveItem(&m_roomMemList, (void*)uid.c_str(), stringcompare)) {
            LI("user: %s DELOK room: %s\n", uid.c_str(), m_roomId.c_str());
            return true;
        } else {
            LI("user: %s DEL FAILED room: %s\n", uid.c_str(), m_roomId.c_str());
            return false;
        }
    } else {
        LI("user: %s NOT in room: %s\n", uid.c_str(), m_roomId.c_str());
        return false;
    }
}

bool RTMeetingRoom::AddMemberToSession(const std::string sid, const std::string uid)
{
    return m_pRoomSession->AddMemberToSession(uid);
}

bool RTMeetingRoom::DelMemberFmSession(const std::string sid, const std::string uid)
{
    return m_pRoomSession->DelMemberFmSession(uid);
}

void RTMeetingRoom::CreateSession()
{
    OSMutexLocker  locker(&m_mutex);
    if (m_pRoomSession) {
        return;
    }
    if (m_pRoomSession == NULL) {
        RTRoomManager::Instance()->GenericMeetingSessionId(m_sessionId);
        m_pRoomSession = new RTRoomSession(m_sessionId);
    }
}

void RTMeetingRoom::DestroySession()
{
    OSMutexLocker  locker(&m_mutex);
    if (!m_pRoomSession) {
        return;
    }
    if (m_pRoomSession->GetSessionMemberNumber() == 0) {
        delete m_pRoomSession;
        m_pRoomSession = NULL;
    }
}

void RTMeetingRoom::CheckMembers()
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMemList.count==0) {
        return;
    }
    ListElement* elem = NULL;
    LI("MeetingRoom members:%d\n", m_roomMemList.count);
    while (ListNextElement(&m_roomMemList, &elem)!=NULL) {
        LI("\t-->MeetingRoom members :%s\n", (char*)elem->content);
    }
}