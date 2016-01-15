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
#include "md5.h"
#include "md5digest.h"
#include "OSMutex.h"
#include "OS.h"

static char          s_curMicroSecStr[32];
static unsigned char s_digest[16];
static OSMutex       s_mutex;

static unsigned int	 s_notify_msg_id = 0;

int RTMeetingRoom::GenericNotifySeq()
{
    return atomic_add(&s_notify_msg_id, 1);
}

RTMeetingRoom::RTMeetingRoom(const std::string mid, const std::string ownerid)
: m_roomId(mid)
, m_sessionId("")
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
    if (GetRoomMemberOnline()==0) {
        m_sessionId.assign("");
        GenericMeetingSessionId(m_sessionId);
    }
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
    if (GetRoomMemberOnline()==0) {
        m_sessionId.assign("");
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
    OSMutexLocker locker(&m_mutex);
    int online = 0;
    RoomMembers::iterator rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        if (rit->second && rit->second->_memStatus==MemberStatus::MS_INMEETING) {
            online++;
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
    if (GetRoomMemberOnline()==0) {
        m_sessionId.assign("");
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

int RTMeetingRoom::AddNotifyMsg(const std::string pubsher, const std::string pubid)
{
    OSMutexLocker locker(&m_notifyMutex);
    NotifyMsg* notifyMsg = new NotifyMsg();
    notifyMsg->seq = GenericNotifySeq();
    notifyMsg->pubTime = OS::Milliseconds();
    notifyMsg->notifyMsg = pubid;
    notifyMsg->publisher = pubsher;
    m_roomNotifyMsgs.insert(make_pair(pubsher, notifyMsg));
    return 0;
}

int RTMeetingRoom::DelNotifyMsg(const std::string pubsher, std::string& pubid)
{
    OSMutexLocker locker(&m_notifyMutex);
    RoomNotifyMsgs::iterator mit = m_roomNotifyMsgs.find(pubsher);
    if (mit!=m_roomNotifyMsgs.end()) {
        LI("%s leave RTMeetingRoom::DelNotifyMsg NotifyMsg....", pubsher.c_str());
        pubid = mit->second->notifyMsg;
        delete mit->second;
        mit->second = NULL;
        m_roomNotifyMsgs.erase(pubsher);
    }
    return 0;
}


void RTMeetingRoom::GenericMeetingSessionId(std::string& strId)
{
    SInt64 curTime = OS::Microseconds();
    MD5_CTX context;
    StrPtrLen hashStr;
    Assert(RTRoomManager::s_msgQueueIp.length()>0);
    memset(s_curMicroSecStr, 0, 128);
    memset(s_digest, 0, 16);
    qtss_sprintf(s_curMicroSecStr, "%lld", curTime);
    MD5_Init(&context);
    MD5_Update(&context, (unsigned char*)s_curMicroSecStr, (unsigned int)strlen((const char*)s_curMicroSecStr));
    MD5_Update(&context, (unsigned char*)RTRoomManager::s_msgQueueIp.c_str(), (unsigned int)RTRoomManager::s_msgQueueIp.length());
    MD5_Final(s_digest, &context);
    HashToString(s_digest, &hashStr);
    strId = hashStr.GetAsCString();
}