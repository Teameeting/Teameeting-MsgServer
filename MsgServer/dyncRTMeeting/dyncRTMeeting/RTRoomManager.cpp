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
            OnGetMemberList(tmsg, mmsg, data);
        }
            break;
        case MEETCMD::leave:
        {
            OnLeaveRoom(tmsg, mmsg, data);
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
                            
                            //@Eric
                            //* 1, Check GetMembersStatus is Waitting
                            // Need to save msg to msgList for other member(not in room) when Got members form http.
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
                            if (!meetingRoom->GetSessionMemberInJson(mmsg._from, users)) {
                                LI("SENDTAGS::notify send to others:%s\n", users.c_str());
                                mmsg._tags = SENDTAGS::notify;//send to room other members;
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
    //@Eric
    //* 1, Find Room
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("% Room %s not exists, new it\n", mmsg._room.c_str(), __FUNCTION__);
        m_meetingRoomMap.insert(make_pair(mmsg._room, new rtc::RefCountedObject<RTMeetingRoom>(mmsg._room, "")));
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

    //@Eric
    //* 3, Notify Other members, i'm online.
    
    //@Eric
    //* 4, Notify myself
    std::string res, users, resp;
    RTMeetingRoom::RoomNotifyMsgs msgMaps(it->second->GetRoomNotifyMsgsMap());
    LI("EnterRoom msgMaps size:%d\n", (int)msgMaps.size());
    RTMeetingRoom::RoomNotifyMsgs::iterator mit = msgMaps.begin();
    for (; mit!=msgMaps.end(); mit++) {
        if (mit->second->publisher.compare(mmsg._from)!=0) {
            LI("other %s send to me!!!\n", mit->second->publisher.c_str());
            mmsg._cont = mit->second->notifyMsg;
            mmsg._tags = SENDTAGS::notify;
            ChangeToJson(mmsg._from, users);
            GenericResponse(tmsg, mmsg, RTCommCode::_ok, users, res, resp);
            SendTransferData(resp, (int)resp.length());
        } else {
            LI("%s NOT send to myself!!!\n", mit->second->publisher.c_str());
        }
    }
    
    //@Eric
    //* 5, Enter room done!
    // Need send back event to client?
    //////////////////////////
    
}

void RTRoomManager::LeaveRoom(TRANSMSG& tmsg, MEETMSG& mmsg)
{
    //@Eric
    //* 1, Remove myself in Room MemberList;
    //
    
    //@Eric
    //* 2, Notify Other members
    //
    
    //@Eric
    //* 3, Check size of Room MemberList is 0?
    // If 0, maybe release this room?
    // If GetMembersStatus is Waitting, need waitting for http response!!!
    
    //@Eric
    //* 4, Leave room done!
    // Need send back event to client?
}

void RTRoomManager::OnGetMemberList(TRANSMSG& tmsg, MEETMSG& mmsg, std::string& data)
{
    //@Eric
    //* 1, Update room member list.
    MeetingRoomMap::iterator it = m_meetingRoomMap.find(mmsg._room);
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        assert(false);  // Maybe has some error!!!
    }
    
    //@Eric
    //* 2, Set GetMembersStatus to Done, and notify other members(not in room) meeting is opened.
    it->second->SetGetMembersStatus(RTMeetingRoom::GMS_DONE);
    
    //@Eric
    //* 3, Check msgs list, and send to notify other members(not in room)
    
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
        it->second->DelNotifyMsg(mmsg._from);
        return;
    }
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
