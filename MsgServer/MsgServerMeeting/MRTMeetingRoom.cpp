//
//  MRTMeetingRoom.cpp
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
#include "MRTMeetingRoom.h"
#include "rtklog.h"
#include "atomic.h"
#include "MRTRoomManager.h"
#include "md5.h"
#include "md5digest.h"
#include "OSMutex.h"
#include "OS.h"

static char          s_curMicroSecStr[32];
static unsigned char s_digest[16];
static OSMutex       s_mutex;

static unsigned int	 s_notify_msg_id = 0;

int MRTMeetingRoom::GenericNotifySeq()
{
    return atomic_add(&s_notify_msg_id, 1);
}

MRTMeetingRoom::MRTMeetingRoom(const std::string mid, const std::string ownerid)
: m_roomId(mid)
, m_sessionId("")
, m_ownerId(ownerid)
, m_eGetMembersStatus(GMS_NIL)
{
    m_roomMembers.clear();
}

MRTMeetingRoom::~MRTMeetingRoom()
{
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        if (rit->second) {
            delete rit->second;
            rit->second = NULL;
        }
    }
    m_roomMembers.clear();
}

void MRTMeetingRoom::AddMemberToRoom(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_mutex);
    if (GetRoomMemberOnline()==0) {
        m_sessionId.assign("");
        GenericMeetingSessionId(m_sessionId);
    }
    RoomMembersIt rit = m_roomMembers.find(uid);
    if (rit!=m_roomMembers.end()) {
        LI("AddMemberToRoom %s to room status:%d\n", uid.c_str(), status);
        rit->second->_memStatus = status;
    } else {
        m_roomMembers.insert(make_pair(uid, new RoomMember(status, uid)));
        LI("user: %s ADDOK room: %s\n", uid.c_str(), m_roomId.c_str());
    }
}

void MRTMeetingRoom::SyncRoomMember(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembersIt rit = m_roomMembers.find(uid);
    if (rit==m_roomMembers.end()) {
        m_roomMembers.insert(make_pair(uid, new RoomMember(status, uid)));
        LI("offline user: %s ADDOK room: %s\n", uid.c_str(), m_roomId.c_str());
    }
}

bool MRTMeetingRoom::IsMemberInRoom(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return false;
    }
    return (m_roomMembers.find(uid)!=m_roomMembers.end());
}


void MRTMeetingRoom::DelMemberFmRoom(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembersIt rit = m_roomMembers.find(uid);
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

int MRTMeetingRoom::GetRoomMemberJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return -1;
    }

    TOJSONUSER touser;
    LI("room members:%d\n", m_roomMembers.size());
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        if (rit->first.compare(from)!=0) {
            touser._us.push_front(rit->first);
        }
    }
    users = touser.ToJson();
    return 0;
}

int MRTMeetingRoom::GetAllRoomMemberJson(std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return -1;
    }
    
    TOJSONUSER touser;
    LI("all room members:%d\n", m_roomMembers.size());
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        touser._us.push_front(rit->first);
    }
    users = touser.ToJson();
    return 0;
}

int MRTMeetingRoom::GetRoomMemberMeetingJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        LE("GetRoomMemberMeetingJson room member is 0\n");
        return -1;
    }
    
    TOJSONUSER touser;
    
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) { //_uid.compare(from)==0--->from, !=0--->others
        if (rit->second && rit->second->_uid.compare(from) && rit->second->_memStatus==MemberStatus::MS_INMEETING) {
            touser._us.push_front(rit->first);
        }
    }
    users = touser.ToJson();
    LI("---->>>room members:%d, users:%s\n", m_roomMembers.size(), users.c_str());
    return 0;
}

void MRTMeetingRoom::UpdateMemberStatus(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembersIt rit = m_roomMembers.find(uid);
    if (rit!=m_roomMembers.end()) {
        rit->second->_memStatus = status;
        LI("UpdateMemberStatus member %s status:%d\n", uid.c_str(), status);
    }
}

int MRTMeetingRoom::GetRoomMemberOnline()
{
    OSMutexLocker locker(&m_mutex);
    int online = 0;
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        if (rit->second && rit->second->_memStatus==MemberStatus::MS_INMEETING) {
            online++;
        }
    }
    LI("room members:%d, online member:%d\n", m_roomMembers.size(), online);
    return online;
}

bool MRTMeetingRoom::IsMemberInMeeting(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return false;
    }
    return ((m_roomMembers.find(uid)!=m_roomMembers.end()) && (MemberStatus::MS_INMEETING == GetRoomMemberStatus(uid)));
}

MRTMeetingRoom::MemberStatus MRTMeetingRoom::GetRoomMemberStatus(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    RoomMembersIt rit = m_roomMembers.find(uid);
    if (rit!=m_roomMembers.end()) {
        return rit->second->_memStatus;
    }
    return MemberStatus::MS_NIL;
}

void MRTMeetingRoom::UpdateMemberList(std::list<const std::string>& ulist)
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
        RoomMembersIt rit = m_roomMembers.begin();
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


void MRTMeetingRoom::CheckMembers()
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.size()==0) {
        return;
    }
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        LI("\t-->MeetingRoom members :%s\n", rit->first.c_str());
    }
}

int MRTMeetingRoom::AddNotifyMsg(const std::string pubsher, const std::string pubid)
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

int MRTMeetingRoom::DelNotifyMsg(const std::string pubsher, std::string& pubid)
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

void MRTMeetingRoom::AddWaitingMsgToList(int type, int tag, TRANSMSG& tmsg, MEETMSG& mmsg)
{
    OSMutexLocker locker(&m_wmsgMutex);
    LI("===>>>MRTMeetingRoom::AddWaitingMsgToList size:%d\n", (int)m_waitingMsgsList.size());
    m_waitingMsgsList.push_back(WaitingMsg(type, tag, tmsg, mmsg));
}

void MRTMeetingRoom::GenericMeetingSessionId(std::string& strId)
{
    SInt64 curTime = OS::Microseconds();
    MD5_CTX context;
    StrPtrLen hashStr;
    Assert(MRTRoomManager::s_msgQueueIp.length()>0);
    memset(s_curMicroSecStr, 0, 128);
    memset(s_digest, 0, 16);
    qtss_sprintf(s_curMicroSecStr, "%lld", curTime);
    MD5_Init(&context);
    MD5_Update(&context, (unsigned char*)s_curMicroSecStr, (unsigned int)strlen((const char*)s_curMicroSecStr));
    MD5_Update(&context, (unsigned char*)MRTRoomManager::s_msgQueueIp.c_str(), (unsigned int)MRTRoomManager::s_msgQueueIp.length());
    MD5_Final(s_digest, &context);
    HashToString(s_digest, &hashStr);
    strId = hashStr.GetAsCString();
}