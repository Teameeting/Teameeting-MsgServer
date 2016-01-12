//
//  RTMeetingRoom.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <algorithm>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "RTMeetingRoom.h"
#include "rtklog.h"
#include "atomic.h"
#include "RTRoomManager.h"

static unsigned int	 s_notify_msg_id = 0;

int RTMeetingRoom::GenericNotifySeq()
{
    return atomic_add(&s_notify_msg_id, 1);
}

RTMeetingRoom::RTMeetingRoom(const std::string sid, const std::string mid, const std::string ownerid)
: m_roomId(mid)
, m_sessionId(sid)
, m_ownerId(ownerid)
, m_eGetMembersStatus(GMS_NIL)
{
    m_roomMembers.clear();
}

RTMeetingRoom::~RTMeetingRoom()
{
    RoomMembers::iterator rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        if (rit->second) {
            delete rit->second;
            rit->second = NULL;
        }
    }
    m_roomMembers.clear();
}

void RTMeetingRoom::AddMemberToRoom(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembers::iterator rit = m_roomMembers.find(uid);
    if (rit!=m_roomMembers.end()) {
        LI("AddMemberToRoom %s to room status:%d\n", uid.c_str(), status);
        rit->second->_memStatus = status;
    } else {
        m_roomMembers.insert(make_pair(uid, new RoomMember(uid, status)));
        LI("user: %s ADDOK room: %s\n", uid.c_str(), m_roomId.c_str());
    }
}

void RTMeetingRoom::SyncRoomMember(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembers::iterator rit = m_roomMembers.find(uid);
    if (rit==m_roomMembers.end()) {
        m_roomMembers.insert(make_pair(uid, new RoomMember(uid, status)));
        LI("offline user: %s ADDOK room: %s\n", uid.c_str(), m_roomId.c_str());
    }
}

bool RTMeetingRoom::IsMemberInRoom(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return false;
    }
    return (m_roomMembers.find(uid)!=m_roomMembers.end());
}


void RTMeetingRoom::DelMemberFmRoom(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembers::iterator rit = m_roomMembers.find(uid);
    if (rit!=m_roomMembers.end()) {
        if (rit->second) {
            delete rit->second;
            rit->second = NULL;
        }
        m_roomMembers.erase(rit);
    }
}

int RTMeetingRoom::GetRoomMemberJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return -1;
    }

    TOJSONUSER touser;
    LI("room members:%d\n", m_roomMembers.size());
    RoomMembers::iterator rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        if (rit->first.compare(from)!=0) {
            touser._us.push_front(rit->first);
        }
    }
    users = touser.ToJson();
    return 0;
}

int RTMeetingRoom::GetRoomMemberMeetingJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        LE("GetRoomMemberMeetingJson room member is 0\n");
        return -1;
    }
    
    TOJSONUSER touser;
    
    RoomMembers::iterator rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) { //_uid.compare(from)==0--->from, !=0--->others
        LI("[][][][][][][]RTMeetingRoom::GetRoomMemberMeetingJson meeting uid:%s, from:%s\n", rit->second->_uid.c_str(), from.c_str());
        if (rit->second && rit->second->_uid.compare(from) && rit->second->_memStatus==MemberStatus::MS_INMEETING) {
            touser._us.push_front(rit->first);
        }
    }
    users = touser.ToJson();
    LI("---->>>room members:%d, users:%s\n", m_roomMembers.size(), users.c_str());
    return 0;
}

void RTMeetingRoom::UpdateMemberStatus(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembers::iterator rit = m_roomMembers.find(uid);
    if (rit!=m_roomMembers.end()) {
        rit->second->_memStatus = status;
        LI("UpdateMemberStatus member %s status:%d\n", uid.c_str(), status);
    }
}

int RTMeetingRoom::GetRoomMemberOnline()
{
    int online = 0;
    RoomMembers::iterator rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        LI("RTMeetingRoom::GetRoomMemberOnline find!!!\n");
        if (rit->second && rit->second->_memStatus==MemberStatus::MS_INMEETING) {
            online++;
            LI("RTMeetingRoom::GetRoomMemberOnline find!!! online:%d\n", online);
        }
    }
    LI("room members:%d, online member:%d\n", m_roomMembers.size(), online);
    return online;
}

void RTMeetingRoom::UpdateMemberList(std::list<const std::string>& ulist)
{
    OSMutexLocker locker(&m_mutex);
    LI("before UpdateMemberList size:%d\n", m_roomMembers.size());
    if (ulist.size()>=m_roomMembers.size()) {
        std::list<const std::string>::iterator ait = ulist.begin();
        for (; ait!=ulist.end(); ait++) {
            SyncRoomMember((*ait), MemberStatus::MS_OUTMEETING);
        }
        LI("UpdateMemberList add mem count:%d\n", m_roomMembers.size());
    } else if (ulist.size()<m_roomMembers.size()) {
        std::list<const std::string> tmpList;
        RoomMembers::iterator rit = m_roomMembers.begin();
        for (; rit!=m_roomMembers.end(); rit++) {
            std::list<const std::string>::iterator t = std::find(ulist.begin(), ulist.end(), rit->first);
            if (t==ulist.end()) {
                tmpList.push_back(rit->first);
            }
        }
        
        std::list<const std::string>::iterator dit = tmpList.begin();
        for (; dit!=tmpList.end(); dit++) {
            DelMemberFmRoom((*dit));
        }
        LI("UpdateMemberList del mem count:%d\n", m_roomMembers.size());
    }
}

void RTMeetingRoom::AddMsgToWaiting(MEETMSG msg)
{
    OSMutexLocker locker(&m_waitingMutex);
    m_waitingMsgs.push_front(msg);
}

void RTMeetingRoom::SendWaitingMsgs()
{
    // move to thread process
    OSMutexLocker locker(&m_waitingMutex);
    
}

void RTMeetingRoom::CheckMembers()
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return;
    }
    RoomMembers::iterator rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        LI("\t-->MeetingRoom members :%s\n", rit->first.c_str());
    }
}

int RTMeetingRoom::AddNotifyMsg(const std::string pubsher, const std::string msg)
{
    OSMutexLocker locker(&m_notifyMutex);
    NotifyMsg* notifyMsg = new NotifyMsg();
    notifyMsg->seq = GenericNotifySeq();
    notifyMsg->pubTime = OS::Milliseconds();
    notifyMsg->notifyMsg = msg;
    notifyMsg->publisher = pubsher;
    m_roomNotifyMsgs.insert(make_pair(pubsher, notifyMsg));
    return 0;
}

int RTMeetingRoom::DelNotifyMsg(const std::string pubsher)
{
    OSMutexLocker locker(&m_notifyMutex);
    RoomNotifyMsgs::iterator mit = m_roomNotifyMsgs.find(pubsher);
    if (mit!=m_roomNotifyMsgs.end()) {
        delete mit->second;
        mit->second = NULL;
        m_roomNotifyMsgs.erase(pubsher);
    }
    return 0;
}