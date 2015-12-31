//
//  RTRoomManager.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTRoomManager.h"
#include <assert.h>
#include "md5.h"
#include "md5digest.h"
#include "OSMutex.h"
#include "OS.h"
#include "rtklog.h"
#include "RTMeetingRoom.h"

static char          s_curMicroSecStr[32];
static unsigned char s_digest[16];
static OSMutex       s_mutex;

std::string RTRoomManager::s_msgQueueIp;
unsigned short RTRoomManager::s_msgQueuePort;
std::string RTRoomManager::s_httpIp;
unsigned short RTRoomManager::s_httpPort;
std::string RTRoomManager::s_httpHost;

int RTRoomManager::EnterRoom(MEETMSG& msg, std::string& tos, std::string& res)
{
    if (msg._from.length()==0 || msg._room.length()==0) {
        LE("invalid params error\n");
        res.assign("params error");
        return -1;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(msg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("Room not exists\n");
        res.assign("not exists");
        return -1;
    } else { // meeting has exists
        int rMemNum = 0, sMemNum = 0;
        rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
        meetingRoom->CreateSession();
        meetingRoom->AddMemberToRoom(msg._from);
        meetingRoom->AddMemberToSession("", msg._from);
        rMemNum = meetingRoom->GetRoomMemberNumber();
        sMemNum = meetingRoom->GetSesssionMemberNumber();

        std::string users;
        if (!ChangeToJson(msg._from, users)) {
            tos = users;
            res.assign("ok");
        } else {
            res.assign("error");
        }
        // send to http
        LI("RoomManager::EnterRoom roomMemNum:%d, sessMemNum:%d, from:%s, pass:%s, roomid:%s\n", rMemNum, sMemNum, msg._from.c_str(), msg._pass.c_str(), msg._room.c_str());
        const char* sign = msg._pass.c_str();
        const char* meetingid = msg._room.c_str();
        char meetingMemNumber[4] = {0};
        const char* sessionid = meetingRoom->GetSessionId().c_str();
        const char* sessionstatus = "1";
        const char* sessiontype = "1";
        char sessionnumber[4] = {0};
        sprintf(meetingMemNumber, "%d", rMemNum);
        sprintf(sessionnumber, "%d", sMemNum);

        m_pHttpSvrConn->HttpInsertUserMeetingRoom(sign, meetingid);
        m_pHttpSvrConn->HttpInsertSessionMeetingInfo(sign, meetingid, sessionid, sessionstatus, sessiontype, sessionnumber);
        m_pHttpSvrConn->HttpUpdateRoomMemNumber(sign, meetingid, meetingMemNumber);
        m_pHttpSvrConn->HttpUpdateSessionMeetingNumber(sign, sessionid, sessionnumber);
        m_pHttpSvrConn->HttpUpdateUserMeetingJointime(sign, meetingid);
        res.assign("ok");
        return 0;
    }
}

int RTRoomManager::DcommRoom(MEETMSG& msg, std::string& tos, std::string& res)
{
    if (msg._from.length()==0 || msg._room.length()==0 || msg._to.length()==0) {
        LE("invalid params error\n");
        res.assign("params error");
        return -1;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(msg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("Room not exists\n");
        res.assign("not exists");
        return -1;
    } else { // meeting has exists
        switch (msg._action) {
            case DCOMMACTION::msend:
            {
                switch (msg._tags) {
                    case SENDTAGS::talk:
                    {
                        if (msg._to.at(0)=='a') {
                            //to all
                            rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
                            std::string users;
                            if (!meetingRoom->GetSessionMemberInJson(msg._from, users)) {
                                tos = users;
                                res.assign("ok");
                            } else {
                                res.assign("error");
                            }
                        } else if (msg._to.at(0)=='u') {
                            //to userlist
                        }
                    }
                        break;
                    case SENDTAGS::chat:
                    {
                    
                    }
                        break;
                    case SENDTAGS::lvmsg:
                    {
                    
                    }
                        break;
                    default:
                    {
                        res.assign("invalid tags");
                        return -1;
                    }
                        break;
                }
            }
                break;
            case DCOMMACTION::dsett:
            {
            
            }
                break;
            case DCOMMACTION::share:
            {
            
            }
                break;
                
            default:
            {
                res.assign("invalid actions\n");
                return -1;
            }
                break;
        }
        res.assign("ok");
        return 0;
    }
}

int RTRoomManager::LeaveRoom(MEETMSG& msg, std::string& tos, std::string& res)
{
    if (msg._from.length()==0 || msg._room.length()==0) {
        LE("invalid params error\n");
        res.assign("params error");
        return -1;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(msg._room);
    if (it == m_meetingRoomMap.end()) {
        LE("room:%s not exists, ERROR CANNOT LEAVE!!!\n", msg._room.c_str());
        res.assign("not exists");
        return -1;
    } else {
        int rMemNum = 0, sMemNum = 0;
        rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
        meetingRoom->DelMemberFmSession("", msg._from);
        meetingRoom->DelMemberFmRomm(msg._from);
        
        rMemNum = meetingRoom->GetRoomMemberNumber();
        sMemNum = meetingRoom->GetSesssionMemberNumber();
        std::string users;
        if (!ChangeToJson(msg._from, users)) {
            tos = users;
            res.assign("ok");
        } else {
            res.assign("error");
        }
        const char* sign = msg._pass.c_str();
        const char* meetingid = msg._room.c_str();
        const char* sessionid = meetingRoom->GetSessionId().c_str();
        LI("RoomManager::LeaveRoom roomMemNum:%d, sessMemNum:%d, meetingid:%s, sign:%s, sessionid:%s\n", rMemNum, sMemNum, meetingid, sign, sessionid);
        char meetingMemNumber[4] = {0};
        char sessionnumber[4] = {0};
        sprintf(meetingMemNumber, "%d", rMemNum);
        sprintf(sessionnumber, "%d", sMemNum);

        m_pHttpSvrConn->HttpUpdateRoomMemNumber(sign, meetingid, meetingMemNumber);
        m_pHttpSvrConn->HttpUpdateSessionMeetingNumber(sign, sessionid, sessionnumber);
        m_pHttpSvrConn->HttpUpdateSessionMeetingEndtime(sign, sessionid);
        meetingRoom->DestroySession();
        res.assign("ok");
        return 0;
    }
}

int RTRoomManager::CreateRoom(MEETMSG& msg, std::string& tos, std::string& res)
{
    if (msg._from.length()==0 || msg._room.length()==0) {
        LE("invalid params error\n");
        res.assign("params error");
        return -1;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(msg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        m_meetingRoomMap.insert(make_pair(msg._room, new rtc::RefCountedObject<RTMeetingRoom>(msg._room, msg._from)));
        std::string users;
        if (!ChangeToJson(msg._from, users)) {
            tos = users;
            res.assign("ok");
        } else {
            res.assign("error");
        }
        LI("CreateRoom for roomid:%s\n", msg._room.c_str());
        return 0;
    } else { // meeting has exists
        LE("the room has already exists roomid:%s\n", msg._room.c_str());
        res.assign("exists");
        return -1;
    }
}

int RTRoomManager::DestroyRoom(MEETMSG& msg, std::string& tos, std::string& res)
{
    if (msg._from.length()==0 || msg._room.length()==0) {
        LE("invalid params error\n");
        res.assign("params error");
        return -1;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(msg._room);
    if (it == m_meetingRoomMap.end()) {
        LE("uid:%s NOT in this room:%s, ERROR CANNOT LEAVE!!!\n");
        res.assign("not exists");
        return -1;
    } else {
        it->second->reset();
        m_meetingRoomMap.erase(it);
        std::string users;
        if (!ChangeToJson(msg._from, users)) {
            tos = users;
            res.assign("ok");
        } else {
            res.assign("error");
        }
        res.assign("ok");
        LI("DestroyRoom for roomid:%s\n", msg._room.c_str());
        return 0;
    }
}

/////////////////////////////////////////////////////////////

void RTRoomManager::GenericMeetingSessionId(std::string& strId)
{
    SInt64 curTime = OS::Microseconds();
    MD5_CTX context;
    StrPtrLen hashStr;
    Assert(s_msgQueueIp.length()>0);
    memset(s_curMicroSecStr, 0, 128);
    memset(s_digest, 0, 16);
    qtss_sprintf(s_curMicroSecStr, "%lld", curTime);
    MD5_Init(&context);
    MD5_Update(&context, (unsigned char*)s_curMicroSecStr, (unsigned int)strlen((const char*)s_curMicroSecStr));
    MD5_Update(&context, (unsigned char*)s_msgQueueIp.c_str(), (unsigned int)s_msgQueueIp.length());
    MD5_Final(s_digest, &context);
    HashToString(s_digest, &hashStr);
    strId = hashStr.GetAsCString();
}

bool RTRoomManager::Init()
{
    return ConnectMsgQueue() && ConnectHttpSvrConn();
}

bool RTRoomManager::ConnectMsgQueue()
{
    m_pMsgQueueSession = new RTTransferSession();
    m_pMsgQueueSession->Init();
    // conn to msgqueue
    while(!m_pMsgQueueSession->Connect(s_msgQueueIp, s_msgQueuePort)) {
        LI("connecting to msgqueue server %s:%u waiting...\n", s_msgQueueIp.c_str(), s_msgQueuePort);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, s_msgQueuePort,  m_pMsgQueueSession->GetSocket()->GetSocketFD());
    m_pMsgQueueSession->EstablishConnection();
    return true;
}

bool RTRoomManager::ConnectHttpSvrConn()
{
    if (!m_pHttpSvrConn) {
        m_pHttpSvrConn = new RTHttpSvrConn();
        m_pHttpSvrConn->SetHttpHost(s_httpIp, s_httpPort, s_httpHost);
    }
    return true;
}

void RTRoomManager::SendTransferData(const std::string strData, int nLen)
{
    if (m_pMsgQueueSession) {
        m_pMsgQueueSession->SendTransferData(strData.c_str(), nLen);
    } else {
        LE("%s m_pMsgQueueSession %d NULL error\n", __FUNCTION__, __LINE__);
    }
}

void RTRoomManager::RefreshConnection()
{
    if (m_pMsgQueueSession) {
       if (m_pMsgQueueSession->RefreshTime())
           m_pMsgQueueSession->KeepAlive();
    } else {
        LE("%s m_pMsgQueueSession %d NULL error\n", __FUNCTION__, __LINE__);
    }
}

int RTRoomManager::ChangeToJson(const std::string from, std::string& users)
{
    TOJSONUSER touser;
    touser._us.push_front(from);
    users = touser.ToJson();
    return 0;
}

void RTRoomManager::CheckMembers()
{
#if 1
    if (m_meetingRoomMap.size()==0) {
        return;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.begin();
    for (; it!=m_meetingRoomMap.end(); it++) {
        rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
        LI("meetingRoom member.number:%d\n", meetingRoom->GetSesssionMemberNumber());
    }
#endif
}

void RTRoomManager::SyncHttpRequest()
{
    
}

void RTRoomManager::ClearSessionLost(const std::string& uid)
{
    if (m_meetingRoomMap.size()==0) {
        return;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.begin();
    for (; it!=m_meetingRoomMap.end(); it++) {
        rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
        meetingRoom->ClearLostMember(uid);
        LI("meetingRoom clear member, member.number:%d\n", meetingRoom->GetSesssionMemberNumber());
    }
}

void RTRoomManager::ClearMsgQueueSession(const std::string& sid)
{
    // the object MsgQueue RTTransferSession will be delete in Task
    // here just make the pointer to NULL
    m_pMsgQueueSession = NULL;
    LI("RTRoomManager::ClearMsgQueueSession m_pMsgQueueSession=NULL sid:%s\n", sid.c_str());
}