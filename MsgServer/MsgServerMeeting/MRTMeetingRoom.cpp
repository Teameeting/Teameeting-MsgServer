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
    OSMutexLocker locker(&m_mutex);
    if (m_meetingMembers.empty()) {
        m_sessionId.assign("");
        GenericMeetingSessionId(m_sessionId);
    }
    m_roomMembers.insert(uid);
    if (MS_INMEETING==status) {
        m_meetingMembers.insert(uid);
        LI("AddMemberToRoom userid: %s to meeting\n", uid.c_str());
    }
}

void MRTMeetingRoom::SyncRoomMember(const std::string& uid, MemberStatus status)
{
    OSMutexLocker locker(&m_mutex);
    m_roomMembers.insert(uid);
    if (MS_INMEETING==status) {
        m_meetingMembers.insert(uid);
        LI("SyncRoomMember userid: %s to meeting\n", uid.c_str());
    }
}

bool MRTMeetingRoom::IsMemberInRoom(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.empty()) {
        return false;
    }
    return (m_roomMembers.count(uid)?true:false);
}


void MRTMeetingRoom::DelMemberFmRoom(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    m_meetingMembers.erase(uid);
    m_roomMembers.erase(uid);
}

void MRTMeetingRoom::AddMemberToMeeting(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    m_meetingMembers.insert(uid);
}

void MRTMeetingRoom::DelMemberFmMeeting(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    m_meetingMembers.erase(uid);
}

int MRTMeetingRoom::GetRoomMemberJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.empty()) {
        return -1;
    }

    TOJSONUSER touser;
    LI("room members:%d\n", m_roomMembers.size());
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        if ((*rit).compare(from)!=0) {
            touser._us.push_front(*rit);
        }
    }
    users = touser.ToJson();
    return 0;
}

int MRTMeetingRoom::GetAllRoomMemberJson(std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMembers.empty()) {
        return -1;
    }

    TOJSONUSER touser;
    LI("all room members:%d\n", m_roomMembers.size());
    RoomMembersIt rit = m_roomMembers.begin();
    for (; rit!=m_roomMembers.end(); rit++) {
        touser._us.push_front(*rit);
    }
    users = touser.ToJson();
    return 0;
}

int MRTMeetingRoom::GetMeetingMemberJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_meetingMembers.empty()) {
        LE("GetMeetingMemberJson member is 0\n");
        return -1;
    }

    TOJSONUSER touser;
    LI("meeting members:%d\n", m_roomMembers.size());
    MeetingMembersIt mit = m_meetingMembers.begin();
    for (; mit!=m_meetingMembers.end(); mit++) {
        if ((*mit).compare(from)!=0) {
            touser._us.push_front(*mit);
        }
    }
    users = touser.ToJson();
    return 0;
}
int MRTMeetingRoom::GetAllMeetingMemberJson(std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_meetingMembers.empty()) {
        LE("GetAllMeetingMemberJson member is 0\n");
        return -1;
    }

    TOJSONUSER touser;
    LI("all meeting members:%d\n", m_roomMembers.size());
    MeetingMembersIt mit = m_meetingMembers.begin();
    for (; mit!=m_meetingMembers.end(); mit++) {
        touser._us.push_front(*mit);
    }
    users = touser.ToJson();
    return 0;

}

int MRTMeetingRoom::GetRoomMemberOnline()
{
    OSMutexLocker locker(&m_mutex);
    return (int)m_meetingMembers.size();
}

bool MRTMeetingRoom::IsMemberInMeeting(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
    if (m_meetingMembers.empty()) {
        return false;
    }
    return (m_meetingMembers.count(uid)?true:false);
}

MRTMeetingRoom::MemberStatus MRTMeetingRoom::GetRoomMemberStatus(const std::string& uid)
{
    OSMutexLocker locker(&m_mutex);
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
    OSMutexLocker locker(&m_mutex);
    LI("before UpdateMemberList size:%d\n", m_roomMembers.size());
    if (ulist.size()>=m_roomMembers.size()) {
        std::list<std::string>::iterator ait = ulist.begin();
        for (; ait!=ulist.end(); ait++) {
            SyncRoomMember((*ait), MemberStatus::MS_OUTMEETING);
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
            DelMemberFmRoom((*dit));
        }
        LI("UpdateMemberList del member count:%d\n", m_roomMembers.size());
    }
}


void MRTMeetingRoom::CheckMembers()
{

}

int MRTMeetingRoom::AddPublishIdMsg(const std::string pubsher, SENDTAGS tags, const std::string content)
{
    OSMutexLocker locker(&m_notifyMutex);
    if (m_publishIdMsgs.size()==0) {
        NotifyMsg* notifyMsg = new NotifyMsg();
        notifyMsg->tags = tags;
        notifyMsg->notifyMsg = content;
        notifyMsg->publisher = pubsher;
        m_publishIdMsgs.insert(make_pair(pubsher, notifyMsg));
        return 0;
    }
    PublishIdMsgs::iterator mit = m_publishIdMsgs.find(pubsher);
    if (mit!=m_publishIdMsgs.end()) {
        LI("%s RTMeetingRoom::AddPublishIdMsg NotifyMsg....", pubsher.c_str());
        mit->second->notifyMsg = content;
    } else {
        NotifyMsg* notifyMsg = new NotifyMsg();
        notifyMsg->tags = tags;
        notifyMsg->notifyMsg = content;
        notifyMsg->publisher = pubsher;
        m_publishIdMsgs.insert(make_pair(pubsher, notifyMsg));
    }

    return 0;
}

int MRTMeetingRoom::DelPublishIdMsg(const std::string pubsher, std::string& pubid)
{
    OSMutexLocker locker(&m_notifyMutex);
    if (m_publishIdMsgs.size()==0) {
        return 0;
    }
    PublishIdMsgs::iterator mit = m_publishIdMsgs.find(pubsher);
    if (mit!=m_publishIdMsgs.end()) {
        LI("%s leave RTMeetingRoom::DelPublishIdMsg NotifyMsg....", pubsher.c_str());
        pubid = mit->second->notifyMsg;
        delete mit->second;
        mit->second = NULL;
        m_publishIdMsgs.erase(pubsher);
    }
    return 0;
}

int MRTMeetingRoom::AddAudioSetMsg(const std::string pubsher, SENDTAGS tags, const std::string content)
{
    OSMutexLocker locker(&m_notifyMutex);
    if (m_audioSetMsgs.size()==0) {
        NotifyMsg* notifyMsg = new NotifyMsg();
        notifyMsg->tags = tags;
        notifyMsg->notifyMsg = content;
        notifyMsg->publisher = pubsher;
        m_audioSetMsgs.insert(make_pair(pubsher, notifyMsg));
        return 0;
    }
    AudioSetMsgs::iterator mit = m_audioSetMsgs.find(pubsher);
    if (mit!=m_audioSetMsgs.end()) {
        LI("%s RTMeetingRoom::AddAudioSetMsg NotifyMsg....", pubsher.c_str());
        mit->second->notifyMsg = content;
    } else {
        NotifyMsg* notifyMsg = new NotifyMsg();
        notifyMsg->tags = tags;
        notifyMsg->notifyMsg = content;
        notifyMsg->publisher = pubsher;
        m_audioSetMsgs.insert(make_pair(pubsher, notifyMsg));
    }
    return 0;
}

int MRTMeetingRoom::DelAudioSetMsg(const std::string pubsher)
{
    OSMutexLocker locker(&m_notifyMutex);
    if (m_audioSetMsgs.size()==0) {
        return 0;
    }
    AudioSetMsgs::iterator mit = m_audioSetMsgs.find(pubsher);
    if (mit!=m_audioSetMsgs.end()) {
        LI("%s leave RTMeetingRoom::DelAudioSetMsg NotifyMsg....", pubsher.c_str());
        delete mit->second;
        mit->second = NULL;
        m_audioSetMsgs.erase(pubsher);
    }
    return 0;
}

int MRTMeetingRoom::AddVideoSetMsg(const std::string pubsher, SENDTAGS tags, const std::string content)
{
    OSMutexLocker locker(&m_notifyMutex);
    if (m_videoSetMsgs.size()==0) {
        NotifyMsg* notifyMsg = new NotifyMsg();
        notifyMsg->tags = tags;
        notifyMsg->notifyMsg = content;
        notifyMsg->publisher = pubsher;
        m_videoSetMsgs.insert(make_pair(pubsher, notifyMsg));
        return 0;
    }
    VideoSetMsgs::iterator mit = m_videoSetMsgs.find(pubsher);
    if (mit!=m_videoSetMsgs.end()) {
        LI("%s RTMeetingRoom::AddVideoSetMsg NotifyMsg....", pubsher.c_str());
        mit->second->notifyMsg = content;
    } else {
        NotifyMsg* notifyMsg = new NotifyMsg();
        notifyMsg->tags = tags;
        notifyMsg->notifyMsg = content;
        notifyMsg->publisher = pubsher;
        m_videoSetMsgs.insert(make_pair(pubsher, notifyMsg));
    }
    return 0;
}

int MRTMeetingRoom::DelVideoSetMsg(const std::string pubsher)
{
    OSMutexLocker locker(&m_notifyMutex);
    if (m_videoSetMsgs.size()==0) {
        return 0;
    }
    VideoSetMsgs::iterator mit = m_videoSetMsgs.find(pubsher);
    if (mit!=m_videoSetMsgs.end()) {
        LI("%s leave RTMeetingRoom::DelVideoSetMsg NotifyMsg....", pubsher.c_str());
        delete mit->second;
        mit->second = NULL;
        m_videoSetMsgs.erase(pubsher);
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
    char* p = hashStr.GetAsCString();
    strId = p;
    delete p;
    p = NULL;
    hashStr.Delete();
}
