//
//  RTRoomManager.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTRoomManager.h"
#include <assert.h>
#include <algorithm>
#include "atomic.h"
#include "md5.h"
#include "md5digest.h"
#include "OSMutex.h"
#include "OS.h"
#include "rtklog.h"
#include "RTMeetingRoom.h"
#include "StatusCode.h"
#include "RTHiredis.h"
#include "RTHttpMsg.h"

static char          s_curMicroSecStr[32];
static unsigned char s_digest[16];
static OSMutex       s_mutex;
static unsigned int	 g_trans_id = 0;

std::string RTRoomManager::s_msgQueueIp;
unsigned short RTRoomManager::s_msgQueuePort;
std::string RTRoomManager::s_httpIp;
unsigned short RTRoomManager::s_httpPort;
std::string RTRoomManager::s_httpHost;

void RTRoomManager::HandleOptRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    switch (mmsg._cmd) {
        case MEETCMD::enter:
        {
            EnterRoom(tmsg, mmsg);
        }
            break;
        case MEETCMD::leave:
        {
            LeaveRoom(tmsg, mmsg);
        }
            break;
        case MEETCMD::create:
        {
            CreateRoom(tmsg, mmsg);
        }
            break;
        case MEETCMD::destroy:
        {
            DestroyRoom(tmsg, mmsg);
        }
            break;
        default:
            LE("HandleOptRoom cmd [%d] not handle now\n", mmsg._cmd);
            break;
    }
}


void RTRoomManager::HandleOptRoomWithData(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    switch (mmsg._cmd) {
        case MEETCMD::enter:
        {
            OnEnterRoom(tmsg, mmsg, data);
        }
            break;
        case MEETCMD::leave:
        {
            OnLeaveRoom(tmsg, mmsg, data);
        }
            break;
        case MEETCMD::create:
        {
            OnCreateRoom(tmsg, mmsg, data);
        }
            break;
        case MEETCMD::destroy:
        {
            OnDestroyRoom(tmsg, mmsg, data);
        }
            break;
        default:
            LE("HandleOptRoomWithData cmd [%d] not handle now\n", mmsg._cmd);
            break;
    }
}


void RTRoomManager::HandleDcommRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    std::string res, users, resp;
    mmsg._ntime = OS::Milliseconds();
    if (mmsg._from.length()==0 || mmsg._room.length()==0 || mmsg._to.length()==0) {
        LE("invalid params error\n");
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("Room not exists\n");
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_nexistroom, users, GetRTCommStatus(RTCommCode::_nexistroom), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    } else { // meeting has exists
        switch (mmsg._action) {
            case DCOMMACTION::msend:
            {
                switch (mmsg._tags) {
                    case SENDTAGS::talk:
                    {
                        if (mmsg._to.at(0)=='a') {
                            //to all
                            rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
                            if (!meetingRoom->GetRoomMemberInJson(mmsg._from, users)) {
                                GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
                                SendTransferData(resp, (int)resp.length());
                                return;
                            } else {
                                ChangeToJson(mmsg._from, users);
                                GenericResponse(tmsg, mmsg, RTCommCode::_errtojson, users, GetRTCommStatus(RTCommCode::_errtojson), resp);
                                SendTransferData(resp, (int)resp.length());
                                return;
                            }
                        } else if (mmsg._to.at(0)=='u') {
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
                    case SENDTAGS::notify:
                    {
                        if (mmsg._to.at(0)=='a') {
                            //to all
                            rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
                            meetingRoom->AddNotifyMsg(mmsg._from, mmsg._cont);
                            if (!meetingRoom->GetRoomMemberInJson(mmsg._from, users)) {
                                GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
                                SendTransferData(resp, (int)resp.length());
                                return;
                            } else {
                                ChangeToJson(mmsg._from, users);
                                GenericResponse(tmsg, mmsg, RTCommCode::_errtojson, users, GetRTCommStatus(RTCommCode::_errtojson), resp);
                                SendTransferData(resp, (int)resp.length());
                                return;
                            }
                        } else if (mmsg._to.at(0)=='u') {
                            //to userlist
                        }
                    }
                        break;
                    default:
                    {
                        ChangeToJson(mmsg._from, users);
                        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
                        SendTransferData(resp, (int)resp.length());
                        return;
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
                ChangeToJson(mmsg._from, users);
                GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
                SendTransferData(resp, (int)resp.length());
                return;
            }
                break;
        }
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
}

void RTRoomManager::EnterRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    if (m_pHttpSvrConn) {
        m_pHttpSvrConn->HttpGetMeetingMemberList(tmsg, mmsg);
    } else {
        LE("EnterRoom m_pHttpSvrConn is NULL\n");
    }
}

void RTRoomManager::LeaveRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    if (m_pHttpSvrConn) {
        m_pHttpSvrConn->HttpGetMeetingMemberList(tmsg, mmsg);
    } else {
        LE("EnterRoom m_pHttpSvrConn is NULL\n");
    }
}

void RTRoomManager::CreateRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    if (m_pHttpSvrConn) {
        m_pHttpSvrConn->HttpGetMeetingMemberList(tmsg, mmsg);
    } else {
        LE("EnterRoom m_pHttpSvrConn is NULL\n");
    }
}

void RTRoomManager::DestroyRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    if (m_pHttpSvrConn) {
        m_pHttpSvrConn->HttpGetMeetingMemberList(tmsg, mmsg);
    } else {
        LE("EnterRoom m_pHttpSvrConn is NULL\n");
    }
}

void RTRoomManager::OnEnterRoom(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    std::string res, users, resp;
    mmsg._ntime = OS::Milliseconds();
    if (mmsg._from.length()==0 || mmsg._room.length()==0) {
        LE("% invalid params error\n", __FUNCTION__);
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    MEETINGMEMBERLIST memList;
    std::string err;
    memList.GetMsg(data, err);
    if (err.length()>0) {
        LE("%s json parse meeting member list error\n", __FUNCTION__);
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    std::list<const std::string>::iterator remoteFound = std::find(memList._uslist.begin(), memList._uslist.end(), mmsg._from);
    bool localFound = false;
    int rMemNum = 0, sMemNum = 0;
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("% Room %s not exists, new it\n", mmsg._room.c_str(), __FUNCTION__);
        //rtc::scoped_ptr<RTMeetingRoom> mRoom(new rtc::RefCountedObject<RTMeetingRoom>(mmsg._room, ""));
        //m_meetingRoomMap.insert(make_pair(mmsg._room, mRoom.get()));
        m_meetingRoomMap.insert(make_pair(mmsg._room, new rtc::RefCountedObject<RTMeetingRoom>(mmsg._room, "")));
        it = m_meetingRoomMap.find(mmsg._room);
    } else { // meeting has exists
        localFound = it->second->IsMemberInRoom(mmsg._from);
    }
    
    if (it==m_meetingRoomMap.end()) {
        LE("m_meetingRoomMap not found Room %s ERRORRRRRRRRR\n", mmsg._room.c_str());
        return;
    }
    
    if (remoteFound!=memList._uslist.end() && !localFound) {
        // this means user is in remote and not in local: after create room, owner enter
        LI("[C][C]OnEnterRoom 1, after create room, owner enter\n");
        //it->second->UpdateUserList(memList._uslist);
        it->second->AddMemberToRoom(mmsg._from);
    } else if (remoteFound==memList._uslist.end() && !localFound) {
        // this means user is not in remote and not in local: invite others join room first
        LI("[C][C]OnEnterRoom 2, invite others join room first\n");
        it->second->AddMemberToRoom(mmsg._from);
    } else if (remoteFound!=memList._uslist.end() && localFound) {
        // this means user is in remote and also in local: just enter room again
        LI("[C][C]OnEnterRoom 3, just enter room again\n");
    } else {
        // this is error, user is not in remote, and it should not in local
        LE("[C][C]OnEnterRoom 4, ENTER ERROR\n");
    }
    
    // begin session or join session
    it->second->CreateSession();
    it->second->AddMemberToSession("", mmsg._from);
    rMemNum = it->second->GetRoomMemberNumber();
    sMemNum = it->second->GetSesssionMemberNumber();
    mmsg._nmem = sMemNum;

    // send to http
    LI("RoomManager::OnEnterRoom roomMemNum:%d, sessMemNum:%d, from:%s, pass:%s, roomid:%s\n", rMemNum, sMemNum, mmsg._from.c_str(), mmsg._pass.c_str(), mmsg._room.c_str());
    const char* sign = mmsg._pass.c_str();
    const char* meetingid = mmsg._room.c_str();
    char meetingMemNumber[4] = {0};
    char sessionnumber[4] = {0};
    sprintf(meetingMemNumber, "%d", rMemNum);
    sprintf(sessionnumber, "%d", sMemNum);
    const char* sessionid = it->second->GetSessionId().c_str();
    const char* sessionstatus = "1";
    const char* sessiontype = "1";

    if (remoteFound==memList._uslist.end()) { //user NOT in room
        LI("[C][C]OnEnterRoom 2, InsertUserMeetingRoom\n");
        m_pHttpSvrConn->HttpInsertUserMeetingRoom(sign, meetingid);
    }
    
    if (rMemNum>1) {
        m_pHttpSvrConn->HttpUpdateSessionMeetingNumber(sign, sessionid, sessionnumber);
        m_pHttpSvrConn->HttpUpdateUserMeetingJointime(sign, meetingid);
        //m_pHttpSvrConn->HttpUpdateRoomMemNumber(sign, meetingid, meetingMemNumber);
        
        TOJSONUSER touser;
        //std::list<const std::string>::iterator  memIt = memList._uslist.begin();
        //for (; memIt!=memList._uslist.end(); memIt++) {
        //    touser._us.push_front((*memIt));
        //}
        //users = touser.ToJson();
        users.assign("");
        it->second->GetRoomMemberInJson(mmsg._from, users);
        mmsg._cont = mmsg._from + "Enter Room!";
        GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, res, resp);
        SendTransferData(resp, (int)resp.length());
    } else if (rMemNum==1) {
        m_pHttpSvrConn->HttpInsertSessionMeetingInfo(sign, meetingid, sessionid, sessionstatus, sessiontype, sessionnumber);
        ChangeToJson(mmsg._from, users);
        mmsg._cont = "You Enter Room!";
        GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, res, resp);
        SendTransferData(resp, (int)resp.length());
    }
    std::string msgNo, msgPub;
    int msgSeq;
    if ((msgSeq=it->second->ShouldNotify(mmsg._from, msgNo, msgPub))>0) {
        users.assign("");
        std::string content = msgPub + " publish " + msgNo;
        mmsg._cont = content;
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, res, resp);
        SendTransferData(resp, (int)resp.length());
        it->second->NotifyDone(mmsg._from, msgPub, msgSeq);
    } else {
        LI("There is no notify msg for user:%s\n", mmsg._from.c_str());
        return;
    }
    
}

void RTRoomManager::OnLeaveRoom(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    std::string res, users, resp;
    mmsg._ntime = OS::Milliseconds();
    if (mmsg._from.length()==0 || mmsg._room.length()==0) {
        LE("% invalid params error\n", __FUNCTION__);
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) {
        LE("OnLeaveRoom room:%s not exists, ERROR CANNOT LEAVE!!!\n", mmsg._room.c_str());
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_nexistroom, users, GetRTCommStatus(RTCommCode::_nexistroom), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    } else {
        int rMemNum = 0, sMemNum = 0;
        rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
        meetingRoom->DelMemberFmSession("", mmsg._from);
        
        rMemNum = meetingRoom->GetRoomMemberNumber();
        sMemNum = meetingRoom->GetSesssionMemberNumber();
        mmsg._nmem = sMemNum;
        
        const char* sign = mmsg._pass.c_str();
        const char* meetingid = mmsg._room.c_str();
        const char* sessionid = meetingRoom->GetSessionId().c_str();
        LI("RoomManager::OnLeaveRoom roomMemNum:%d, sessMemNum:%d, meetingid:%s, sign:%s, sessionid:%s\n", rMemNum, sMemNum, meetingid, sign, sessionid);
        char meetingMemNumber[4] = {0};
        char sessionnumber[4] = {0};
        sprintf(meetingMemNumber, "%d", rMemNum);
        sprintf(sessionnumber, "%d", sMemNum);

        m_pHttpSvrConn->HttpUpdateSessionMeetingNumber(sign, sessionid, sessionnumber);
        m_pHttpSvrConn->HttpUpdateSessionMeetingEndtime(sign, sessionid);
        meetingRoom->DestroySession();
        users.assign("");
        it->second->GetRoomMemberInJson(mmsg._from, users);
        mmsg._cont = mmsg._from + "Leave Room!";
        GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
}

void RTRoomManager::OnCreateRoom(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    std::string res, users, resp;
    mmsg._ntime = OS::Milliseconds();
    if (mmsg._from.length()==0 || mmsg._room.length()==0) {
        LE("% invalid params error\n", __FUNCTION__);
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        rtc::scoped_ptr<RTMeetingRoom> mRoom(new rtc::RefCountedObject<RTMeetingRoom>(mmsg._room, ""));
        m_meetingRoomMap.insert(make_pair(mmsg._room, mRoom.release()));
        ChangeToJson(mmsg._from, users);
        mmsg._cont = "You create Room!";
        GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    } else { // meeting has exists
        LE("% the room has already exists roomid:%s\n", __FUNCTION__, mmsg._room.c_str());
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_existroom, users, GetRTCommStatus(RTCommCode::_existroom), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
}

void RTRoomManager::OnDestroyRoom(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    std::string res, users, resp;
    mmsg._ntime = OS::Milliseconds();
    if (mmsg._from.length()==0 || mmsg._room.length()==0) {
        LE("% invalid params error\n", __FUNCTION__);
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) {
        LE("% room: %s ERROR NOT EXIST!!!\n", __FUNCTION__, mmsg._room.c_str());
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_nexistroom, users, GetRTCommStatus(RTCommCode::_nexistroom), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    } else {
        MEETINGMEMBERLIST memList;
        std::string err;
        memList.GetMsg(data, err);
        if (err.length()>0) {
            LE("%s json parse meeting member list error\n", __FUNCTION__);
            ChangeToJson(mmsg._from, users);
            GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
            SendTransferData(resp, (int)resp.length());
            return;
        }
        it->second->UpdateUserList(memList._uslist);
        if (memList._uslist.size() == 0) {
            it->second->reset();
            m_meetingRoomMap.erase(it);
        }
        
        LI("OnDestroyRoom for roomid:%s\n", mmsg._room.c_str());
        ChangeToJson(mmsg._from, users);
        mmsg._cont = "You destroy Room!";
        GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
}

void RTRoomManager::OnStartMeeting(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    LI("OnStartMeeting return\n");
}

void RTRoomManager::OnStopMeeting(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    LI("OnStopMeeting return\n");
}

void RTRoomManager::OnRefreshRoom(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    LI("OnRefreshRoom return\n");
    /*
    std::string res, users, resp;
    mmsg._ntime = OS::Milliseconds();
    if (mmsg._from.length()==0 || mmsg._room.length()==0) {
        LE("invalid params error\n");
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    ChangeToJson(mmsg._from, users);
    GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
    SendTransferData(resp, (int)resp.length());
    return;
     */
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

int RTRoomManager::GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
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
        m_pHttpSvrConn = new rtc::RefCountedObject<RTHttpSvrConn>();
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
        LI("meetingRoom roomMember:%d, sessMember:%d\n", meetingRoom->GetRoomMemberNumber(), meetingRoom->GetSesssionMemberNumber());
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
        it->second->ClearLostMember(uid);
        LI("meetingRoom clear member, member.number:%d\n", it->second->GetSesssionMemberNumber());
    }
}

void RTRoomManager::ClearMsgQueueSession(const std::string& sid)
{
    // the object MsgQueue RTTransferSession will be delete in Task
    // here just make the pointer to NULL
    m_pMsgQueueSession = NULL;
    LI("RTRoomManager::ClearMsgQueueSession m_pMsgQueueSession=NULL sid:%s\n", sid.c_str());
}


void RTRoomManager::GenericResponse(TRANSMSG tmsg, MEETMSG mmsg, int code, const std::string& tos, const std::string& res, std::string& response)
{
    switch (mmsg._cmd) {
        case MEETCMD::enter:
        case MEETCMD::leave:
        case MEETCMD::create:
        case MEETCMD::destroy:
        case MEETCMD::refresh:
            ResponseNotDcomm(tmsg, mmsg, code, tos, res, response);
            break;
        case MEETCMD::dcomm:
            ResponseDcomm(tmsg, mmsg, code, tos, res, response);
            break;
        default:
            LE("mmsg._cmd error:%d !!!\n", mmsg._cmd);
            break;
    }
}

void RTRoomManager::ResponseNotDcomm(TRANSMSG tmsg, MEETMSG mmsg, int code, const std::string& tos, const std::string& res, std::string& response)
{
    MEETMSG meetmsg;
    long long seq = GenericTransSeq();
    meetmsg._mtype = mmsg._mtype;
    meetmsg._cmd = mmsg._cmd;
    meetmsg._action = mmsg._action;
    meetmsg._tags = mmsg._tags;
    meetmsg._type = mmsg._type;
    meetmsg._mseq = mmsg._mseq;
    meetmsg._from = mmsg._from;
    meetmsg._room = mmsg._room;
    meetmsg._sess = mmsg._sess;
    meetmsg._to = mmsg._from;
    meetmsg._cont = mmsg._cont;
    meetmsg._pass = mmsg._pass;
    meetmsg._code = code;
    meetmsg._status = res;
    meetmsg._nmem = mmsg._nmem;
    meetmsg._ntime = mmsg._ntime;
    
    QUEUEMSG qmsg;
    qmsg._flag = 0;
    qmsg._touser = tos;
    qmsg._connector = tmsg._connector;//which connector come from
    qmsg._content = meetmsg.ToJson();
    
    TRANSFERMSG trmsg;
    trmsg._action = TRANSFERACTION::req;
    trmsg._fmodule = TRANSFERMODULE::mmeeting;
    trmsg._type = TRANSFERTYPE::queue;
    trmsg._trans_seq = seq;
    trmsg._trans_seq_ack = 0;
    trmsg._valid = 1;
    trmsg._content = qmsg.ToJson();
    response = trmsg.ToJson();
}

void RTRoomManager::ResponseDcomm(TRANSMSG tmsg, MEETMSG mmsg, int code, const std::string& tos, const std::string& res, std::string& response)
{
    MEETMSG meetmsg;
    int seq = GenericTransSeq();
    meetmsg._mtype = mmsg._mtype;
    meetmsg._cmd = mmsg._cmd;
    meetmsg._action = mmsg._action;
    meetmsg._tags = mmsg._tags;
    meetmsg._type = mmsg._type;
    meetmsg._mseq = mmsg._mseq;
    meetmsg._from = mmsg._from;
    meetmsg._room = mmsg._room;
    meetmsg._sess = mmsg._sess;
    meetmsg._to = "";
    meetmsg._cont = mmsg._cont;
    meetmsg._pass = mmsg._pass;
    meetmsg._code = code;
    meetmsg._status = res;
    meetmsg._nmem = mmsg._nmem;
    meetmsg._ntime = mmsg._ntime;
    
    QUEUEMSG qmsg;
    qmsg._flag = 0;
    qmsg._touser = tos;
    qmsg._connector = tmsg._connector;//which connector come from
    qmsg._content = meetmsg.ToJson();
    
    TRANSFERMSG trmsg;
    trmsg._action = TRANSFERACTION::req;
    trmsg._fmodule = TRANSFERMODULE::mmeeting;
    trmsg._type = TRANSFERTYPE::queue;
    trmsg._trans_seq = seq;
    trmsg._trans_seq_ack = 0;
    trmsg._valid = 1;
    trmsg._content = qmsg.ToJson();
    response = trmsg.ToJson();
}
