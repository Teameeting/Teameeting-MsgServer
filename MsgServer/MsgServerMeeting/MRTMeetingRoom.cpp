//
//  MRTMeetingRoom.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <algorithm>
#include "rtklog.h"
#include "atomic.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "MRTMeetingRoom.h"
#include "MRTRoomManager.h"
#include "RTUtils.hpp"

MRTMeetingRoom::MRTMeetingRoom(const std::string mid, const std::string ownerid)
: m_roomId(mid)
, m_sessionId("")
, m_ownerId(ownerid)
, m_eGetMembersStatus(GMS_NIL)
{
    m_roomMembers.clear();
    m_meetingMembers.clear();
}

MRTMeetingRoom::~MRTMeetingRoom()
{
    m_meetingMembers.clear();
    m_roomMembers.clear();
}

void MRTMeetingRoom::AddMemberToRoom(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_memberMutex);
    if (m_meetingMembers.empty()) {
        m_sessionId = "";
        GenericSessionId(m_sessionId);
    }
    m_roomMembers.insert(uid);
    if (MS_INMEETING==status) {
        m_meetingMembers.insert(uid);
        LI("AddMemberToRoom userid: %s to meeting\n", uid.c_str());
    }
}

bool MRTMeetingRoom::IsMemberInRoom(const std::string& uid)
{
    bool found = false;
    {
        OSMutexLocker locker(&m_memberMutex);
        found = !m_roomMembers.empty() && m_roomMembers.count(uid);
    }
    return found;
}


void MRTMeetingRoom::DelMemberFmRoom(const std::string& uid)
{
    OSMutexLocker locker(&m_memberMutex);
    m_meetingMembers.erase(uid);
    m_roomMembers.erase(uid);
}

void MRTMeetingRoom::AddMemberToMeeting(const std::string& uid)
{
    OSMutexLocker locker(&m_memberMutex);
    m_meetingMembers.insert(uid);
}

bool MRTMeetingRoom::IsMemberInMeeting(const std::string& uid)
{
    bool found = false;
    {
        OSMutexLocker locker(&m_memberMutex);
        found = !m_meetingMembers.empty() && m_meetingMembers.count(uid);
    }
    return found;
}

void MRTMeetingRoom::DelMemberFmMeeting(const std::string& uid)
{
    OSMutexLocker locker(&m_memberMutex);
    m_meetingMembers.erase(uid);
}

int MRTMeetingRoom::GetRoomMemberJson(const std::string from, pms::ToUser* users)
{
#if DEF_PROTO
    if (!users) return -1;
    {
        OSMutexLocker locker(&m_memberMutex);
        if (m_roomMembers.empty()) {
            return -1;
        }
        RoomMembersIt rit = m_roomMembers.begin();
        for (int i=0; rit!=m_roomMembers.end();i++, rit++) {
            if ((*rit).compare(from)!=0) {
                users->add_users(*rit);
            }
        }
    }
#else
    LI("not define DEF_PROTO\n");
#endif
    return 0;
}

int MRTMeetingRoom::GetAllRoomMemberJson(pms::ToUser* users)
{
#if DEF_PROTO
    if (!users) return -1;
    {
        OSMutexLocker locker(&m_memberMutex);
        if (m_roomMembers.empty()) {
            return -1;
        }
        RoomMembersIt rit = m_roomMembers.begin();
        for (int i=0; rit!=m_roomMembers.end(); i++, rit++) {
            users->add_users(*rit);
        }
    }
#else
    LI("not define DEF_PROTO\n");
#endif
    return 0;
}

int MRTMeetingRoom::GetMeetingMemberJson(const std::string from, pms::ToUser* users)
{
#if DEF_PROTO
    if (!users) return -1;
    {
        OSMutexLocker locker(&m_memberMutex);
        if (m_meetingMembers.empty()) {
            LE("GetMeetingMemberJson member is 0\n");
            return -1;
        }
        LI("meeting members:%d\n", m_roomMembers.size());
        MeetingMembersIt mit = m_meetingMembers.begin();
        for (int i=0; mit!=m_meetingMembers.end(); i++, mit++) {
            if ((*mit).compare(from)!=0) {
                users->add_users(*mit);
            }
        }
    }
#else
    LI("not define DEF_PROTO\n");
#endif
    return 0;
}
int MRTMeetingRoom::GetAllMeetingMemberJson(pms::ToUser* users)
{
#if DEF_PROTO
    if (!users) return -1;
    {
        OSMutexLocker locker(&m_memberMutex);
        if (m_meetingMembers.empty()) {
            LE("GetAllMeetingMemberJson member is 0\n");
            return -1;
        }
        LI("all meeting members:%d\n", m_roomMembers.size());
        MeetingMembersIt mit = m_meetingMembers.begin();
        for (int i=0; mit!=m_meetingMembers.end(); i++, mit++) {
            users->add_users(*mit);
        }
    }
#else
    LI("not define DEF_PROTO\n");
#endif
    return 0;

}


MRTMeetingRoom::MemberStatus MRTMeetingRoom::GetRoomMemberStatus(const std::string& uid)
{
    OSMutexLocker locker(&m_memberMutex);
    if (m_meetingMembers.count(uid)) {
        return MS_INMEETING;
    } else {
        if (m_roomMembers.count(uid)) {
            return MS_OUTMEETING;
        }
        return MS_NIL;
    }
}

void MRTMeetingRoom::UpdateMemberList(std::list<std::string>& ulist)
{
    OSMutexLocker locker(&m_memberMutex);
    LI("before UpdateMemberList size:%d\n", m_roomMembers.size());
    if (ulist.size()>=m_roomMembers.size()) {
        std::list<std::string>::iterator ait = ulist.begin();
        for (; ait!=ulist.end(); ait++) {
            m_roomMembers.insert((*ait));
        }
        LI("UpdateMemberList add member count:%d\n", m_roomMembers.size());
    } else if (ulist.size()<m_roomMembers.size()) {
        std::list<std::string> tmpList;
        RoomMembersIt rit = m_roomMembers.begin();
        for (; rit!=m_roomMembers.end(); rit++) {
            std::list<std::string>::iterator t = std::find(ulist.begin(), ulist.end(), (*rit));
            if (t==ulist.end()) {
                tmpList.push_back(*rit);
            }
        }
        std::list<std::string>::iterator dit = tmpList.begin();
        for (; dit!=tmpList.end(); dit++) {
            m_meetingMembers.erase((*dit));
            m_roomMembers.erase((*dit));
        }
        LI("UpdateMemberList del member count:%d\n", m_roomMembers.size());
    }
}


void MRTMeetingRoom::CheckMembers()
{

}

void MRTMeetingRoom::AddWaitingMsgToList(int type, int tag, pms::RelayMsg& rmsg, pms::MeetMsg& mmsg)
{
    LI("===>>>MRTMeetingRoom::AddWaitingMsgToList size:%d\n", (int)m_waitingMsgsList.size());
    OSMutexLocker locker(&m_wmsgMutex);
    m_waitingMsgsList.push_back(WaitingMsg(type, tag, rmsg, mmsg));
}
