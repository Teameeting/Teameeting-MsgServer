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

std::string     RTRoomManager::s_msgQueueIp;
unsigned short  RTRoomManager::s_msgQueuePort;
std::string     RTRoomManager::s_httpIp;
unsigned short  RTRoomManager::s_httpPort;
std::string     RTRoomManager::s_httpHost;

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
        default:
            LE("HandleOptRoom cmd [%d] not handle now\n", mmsg._cmd);
            break;
    }
}


void RTRoomManager::HandleOptRoomWithData(HTTPCMD cmd, TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    switch (cmd) {
        case HTTPCMD::_get_meeting_info:
        {
            
        }
            break;
        case HTTPCMD::_get_member_list:
        {
            OnGetMemberList(tmsg, mmsg, data);
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
        GenericResponse(tmsg, mmsg, MESSAGETYPE::response, SIGNALTYPE::sndmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    }
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("Room not exists\n");
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, MESSAGETYPE::response, SIGNALTYPE::sndmsg, RTCommCode::_nexistroom, users, GetRTCommStatus(RTCommCode::_nexistroom), resp);
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
                            //@Eric
                            //* 1, Check GetMembersStatus is Waitting
                            // Need to save msg to msgList for other member(not in room) when Got members form http.
                            rtc::scoped_refptr<RTMeetingRoom> meetingRoom = it->second;
                            if (meetingRoom->GetGetMembersStatus()!=RTMeetingRoom::GetMembersStatus::GMS_DONE) {
                                //store message
                                meetingRoom->AddMsgToWaiting(mmsg);
                                return;
                            }
                            if (!meetingRoom->GetRoomMemberInJson(mmsg._from, users)) {
                                GenericResponse(tmsg, mmsg, MESSAGETYPE::request, SIGNALTYPE::sndmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
                                SendTransferData(resp, (int)resp.length());
                                if (m_pHttpSvrConn) {
                                    char mseq[8] = {0};
                                    sprintf(mseq, "%lld", mmsg._mseq);
                                    m_pHttpSvrConn->HttpInsertMeetingMsg(mmsg._pass.c_str(), mmsg._room.c_str(), mseq, "0", it->second->GetSessionId().c_str(), mmsg._cont.c_str(), mmsg._from.c_str());
                                }
                                return;
                            } else {
                                // no member in room
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
                            if (!meetingRoom->GetRoomMemberMeetingInJson(mmsg._from, users)) {
                                LI("SENDTAGS::notify send to others:%s\n", users.c_str());
                                mmsg._tags = SENDTAGS::notify;//send to room other members;
                                GenericResponse(tmsg, mmsg, MESSAGETYPE::request, SIGNALTYPE::sndmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
                                SendTransferData(resp, (int)resp.length());
                                return;
                            } else {
                                //no member in meeting
                            }
                        } else if (mmsg._to.at(0)=='u') {
                            //to userlist
                        }
                    }
                        break;
                    default:
                    {
                        ChangeToJson(mmsg._from, users);
                        GenericResponse(tmsg, mmsg, MESSAGETYPE::response, SIGNALTYPE::sndmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
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
                GenericResponse(tmsg, mmsg, MESSAGETYPE::response, SIGNALTYPE::sndmsg, RTCommCode::_invparams, users, GetRTCommStatus(RTCommCode::_invparams), resp);
                SendTransferData(resp, (int)resp.length());
                return;
            }
                break;
        }
    }
}

void RTRoomManager::EnterRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    std::string res, users, resp;
    //@Eric
    //* 1, Find Room
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("% Room %s not exists, new it\n", mmsg._room.c_str(), __FUNCTION__);
        std::string sid;
        GenericMeetingSessionId(sid);
        m_meetingRoomMap.insert(make_pair(mmsg._room, new rtc::RefCountedObject<RTMeetingRoom>(sid, mmsg._room, "")));
        it = m_meetingRoomMap.find(mmsg._room);
        
        //@Eric
        //* Get room member list from httpSvr.
        if (m_pHttpSvrConn) {
            m_pHttpSvrConn->HttpGetMeetingMemberList(tmsg, mmsg);
        } else {
            LE("EnterRoom m_pHttpSvrConn is NULL\n");
        }
        //@Eric
        //* Set GetMembersStatus to Waitting.
        it->second->SetGetMembersStatus(RTMeetingRoom::GMS_WAITTING);
    }
    
    //@Eric
    //* 2, Add member to RoomList.
    if (!it->second->AddMemberToRoom(mmsg._from)) {
        assert(false);
    }
    //m_pHttpSvrConn->HttpInsertUserMeetingRoom(sign, meetingid);
    

    //@Eric
    //* 3, Notify Other members, i'm online.
    if (!it->second->GetRoomMemberInJson(mmsg._from, users)) {
        if (users.length()>0) {
            LI("notify users :%s i am in room\n", users.c_str());
            mmsg._cont = mmsg._from + " enter room!";
            GenericResponse(tmsg, mmsg, MESSAGETYPE::request, SIGNALTYPE::sndmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
            SendTransferData(resp, (int)resp.length());
        }
    }
    //@Eric
    //* 4, Notify myself
    users.assign("");
    res.assign("");
    resp.assign("");
    RTMeetingRoom::RoomNotifyMsgs msgMaps(it->second->GetRoomNotifyMsgsMap());
    LI("EnterRoom msgMaps size:%d\n", (int)msgMaps.size());
    RTMeetingRoom::RoomNotifyMsgs::iterator mit = msgMaps.begin();
    for (; mit!=msgMaps.end(); mit++) {
        if (mit->second->publisher.compare(mmsg._from)!=0) {
            LI("other %s send to me!!!\n", mit->second->publisher.c_str());
            mmsg._cont = mit->second->notifyMsg;
            mmsg._tags = SENDTAGS::notify;
            ChangeToJson(mmsg._from, users);
            GenericResponse(tmsg, mmsg, MESSAGETYPE::request, SIGNALTYPE::sndmsg, RTCommCode::_ok, users, res, resp);
            SendTransferData(resp, (int)resp.length());
        } else {
            LI("%s NOT send to myself!!!\n", mit->second->publisher.c_str());
        }
    }
    
    //@Eric
    //* 5, Enter room done!
    // Need send back event to client?
    //////////////////////////
    users.assign("");
    res.assign("");
    resp.assign("");
    mmsg._cont = "you enter room";
    ChangeToJson(mmsg._from, users);
    GenericResponse(tmsg, mmsg, MESSAGETYPE::response, SIGNALTYPE::sndmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
    SendTransferData(resp, (int)resp.length());
}

void RTRoomManager::LeaveRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    std::string res, users, resp;
    //@Eric
    //* 1, Remove myself in Room MemberList;
    //
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("% Room %s not exists, you can not leave\n", mmsg._room.c_str(), __FUNCTION__);
        ChangeToJson(mmsg._from, users);
        GenericResponse(tmsg, mmsg, MESSAGETYPE::response, SIGNALTYPE::sndmsg, RTCommCode::_nexistroom, users, GetRTCommStatus(RTCommCode::_nexistroom), resp);
        SendTransferData(resp, (int)resp.length());
        return;
    } else {
        if (!it->second->DelMemberFmRoom(mmsg._from)) {
            assert(false);
        }
    }
    //@Eric
    //* 2, Notify Other members
    //
    users.assign("");
    res.assign("");
    resp.assign("");
    if (!it->second->GetRoomMemberInJson(mmsg._from, users)) {
        if (users.length()>0) {
            LI("notify users :%s i am out room\n", users.c_str());
            mmsg._cont = mmsg._from + " leave room!";
            GenericResponse(tmsg, mmsg, MESSAGETYPE::request, SIGNALTYPE::sndmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
            SendTransferData(resp, (int)resp.length());
        }
    }
    
    //@Eric
    //* 3, Check size of Room MemberList is 0?
    // If 0, maybe release this room?
    // If GetMembersStatus is Waitting, need waitting for http response!!!
    if (it->second->GetGetMembersStatus()==RTMeetingRoom::GetMembersStatus::GMS_WAITTING) {
        //send wait event
    }
    
    //@Eric
    //* 4, Leave room done!
    // Need send back event to client?
    users.assign("");
    ChangeToJson(mmsg._from, users);
    mmsg._cont = "you leave room!";
    GenericResponse(tmsg, mmsg, MESSAGETYPE::response, SIGNALTYPE::sndmsg, RTCommCode::_ok, users, GetRTCommStatus(RTCommCode::_ok), resp);
    SendTransferData(resp, (int)resp.length());
    it->second->DelNotifyMsg(mmsg._from);
}

void RTRoomManager::OnGetMemberList(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    //@Eric
    //* 1, Update room member list.
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        assert(false);  // Maybe has some error!!!
    } else {
        MEETINGMEMBERLIST memList;
        std::string err;
        memList.GetMsg(data, err);
        if (err.length()==0) {
            it->second->UpdateMemberList(memList._uslist);
        } else {
            LE("OnGetMemberList error:%s\n", err.c_str());
        }
    }
    
    //@Eric
    //* 2, Set GetMembersStatus to Done, and notify other members(not in room) meeting is opened.
    it->second->SetGetMembersStatus(RTMeetingRoom::GMS_DONE);
    
    //@Eric
    //* 3, Check msgs list, and send to notify other members(not in room)
    
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
        LI("meetingRoom roomMember:%d\n", meetingRoom->GetRoomMemberNumber());
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
    
}

void RTRoomManager::ClearMsgQueueSession(const std::string& sid)
{
    // the object MsgQueue RTTransferSession will be delete in Task
    // here just make the pointer to NULL
    m_pMsgQueueSession = NULL;
    LI("RTRoomManager::ClearMsgQueueSession m_pMsgQueueSession=NULL sid:%s\n", sid.c_str());
}


void RTRoomManager::GenericResponse(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, const std::string& res, std::string& response)
{
    switch (mmsg._cmd) {
        case MEETCMD::enter:
        case MEETCMD::leave:
            ResponseNotDcomm(tmsg, mmsg, msgtype, stype, code, tos, res, response);
            break;
        case MEETCMD::dcomm:
            ResponseDcomm(tmsg, mmsg, msgtype, stype, code, tos, res, response);
            break;
        default:
            LE("mmsg._cmd error:%d !!!\n", mmsg._cmd);
            break;
    }
}

void RTRoomManager::ResponseNotDcomm(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, const std::string& res, std::string& response)
{
    LI("ResponseNotDcomm res:%s\n", res.c_str());
    MEETMSG meetmsg;
    long long seq = GenericTransSeq();
    meetmsg._mtype = mmsg._mtype;
    meetmsg._messagetype = msgtype;
    meetmsg._signaltype = stype;
    meetmsg._cmd = mmsg._cmd;
    meetmsg._action = mmsg._action;
    meetmsg._tags = mmsg._tags;
    meetmsg._type = mmsg._type;
    meetmsg._mseq = mmsg._mseq;
    meetmsg._from = mmsg._from;
    meetmsg._room = mmsg._room;
    meetmsg._to = mmsg._from;
    meetmsg._cont = mmsg._cont;
    meetmsg._pass = "";
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

void RTRoomManager::ResponseDcomm(TRANSMSG tmsg, MEETMSG mmsg, MESSAGETYPE msgtype, SIGNALTYPE stype, int code, const std::string& tos, const std::string& res, std::string& response)
{
    LI("ResponseDcomm res:%s\n", res.c_str());
    MEETMSG meetmsg;
    int seq = GenericTransSeq();
    meetmsg._mtype = mmsg._mtype;
    meetmsg._messagetype = msgtype;
    meetmsg._signaltype = stype;
    meetmsg._cmd = mmsg._cmd;
    meetmsg._action = mmsg._action;
    meetmsg._tags = mmsg._tags;
    meetmsg._type = mmsg._type;
    meetmsg._mseq = mmsg._mseq;
    meetmsg._from = mmsg._from;
    meetmsg._room = mmsg._room;
    meetmsg._to = "";
    meetmsg._cont = mmsg._cont;
    meetmsg._pass = "";
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
