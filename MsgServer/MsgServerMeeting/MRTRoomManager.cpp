//
//  MRTRoomManager.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "MRTRoomManager.h"
#include <assert.h>
#include <algorithm>
#include <json/json.h>
#include "md5.h"
#include "md5digest.h"
#include "OS.h"
#include "rtklog.h"
#include "MRTMeetingRoom.h"
#include "StatusCode.h"
#include "RTHttpMsg.h"
#include "RTUtils.hpp"
#include "RTZKClient.hpp"


void MRTRoomManager::HandleOptRoom(pms::RelayMsg& rmsg, pms::MeetMsg& mmsg)
{
    LI("HandleOptRoom meet msg--->:\n");
    //mmsg.PrintDebugString();

    switch (mmsg.msg_tag()) {
        case pms::EMsgTag::TENTER:
        {
            EnterRoom(rmsg, mmsg);
        }
            break;
        case pms::EMsgTag::TLEAVE:
        {
            LeaveRoom(rmsg, mmsg);
        }
            break;
        default:
            LE("HandleOptRoom cmd [%d] not handle now\n", mmsg.msg_tag());
            break;
    }
}


void MRTRoomManager::HandleOptRoomWithData(int cmd, pms::RelayMsg& rmsg, pms::MeetMsg& mmsg, std::string& data)
{
    switch (cmd) {
        case M_HTTP_CMD_GET_MEETING_INFO:
        {

        }
            break;
        case M_HTTP_CMD_GET_MEMBER_LIST:
        {
            OnGetMemberList(rmsg, mmsg, data);
        }
            break;
        default:
            LE("HandleOptRoomWithData cmd [%d] not handle now\n", mmsg.msg_tag());
            break;
    }
}


void MRTRoomManager::HandleDcommRoom(pms::RelayMsg& rmsg, pms::MeetMsg& mmsg)
{
    LI("HandleDcommRoom meet msg--->:\n");
    //mmsg.PrintDebugString();

    std::string resp("");
    MeetingRoomMapIt it = m_meetingRoomMap.find(mmsg.rom_id());
    if (it != m_meetingRoomMap.end()) {
        switch (mmsg.msg_tag()) {
            case pms::EMsgTag::TCHAT:
                {
                    // Check GetMembersStatus is Waitting
                    // Need to save msg to msgList for other member(not in room) when Got members form http.
                    rtc::scoped_refptr<MRTMeetingRoom> meetingRoom = it->second;
                    if (meetingRoom->GetGetMembersStatus()!=MRTMeetingRoom::GetMembersStatus::GMS_DONE) {
                        //store message
                        meetingRoom->AddWaitingMsgToList(1, 1, rmsg, mmsg);
                        return;
                    }
                    pms::ToUser *users = new pms::ToUser;
                    if (!meetingRoom->GetRoomMemberJson(mmsg.usr_from(), users)) {
                        GenericResponse(pms::EServerCmd::CSNDMSG, rmsg, mmsg, users, resp);
                        SendTransferData(resp, (int)resp.length());
                        if (m_pHttpSvrConn) {
                            m_pHttpSvrConn->HttpInsertMeetingMsg(mmsg.usr_token().c_str(), mmsg.rom_id().c_str(), "0", it->second->GetSessionId().c_str(), mmsg.msg_cont().c_str(), mmsg.usr_from().c_str());
                        }
                        return;
                    } else {
                        // no member in room
                        delete users;
                        users = nullptr;
                        LE("HandleDcommRoom TCHAT no member in room id:%s\n", mmsg.rom_id().c_str());
                    }
                }
                break;
            case pms::EMsgTag::TNOTIFY:
                {
                    // Check GetMembersStatus is Waitting
                    // Need to save msg to msgList for other member(not in room) when Got members form http.
                    rtc::scoped_refptr<MRTMeetingRoom> meetingRoom = it->second;
                    if (meetingRoom->GetGetMembersStatus()!=MRTMeetingRoom::GetMembersStatus::GMS_DONE) {
                        //store message
                        meetingRoom->AddWaitingMsgToList(1, 1, rmsg, mmsg);
                        return;
                    }
                    pms::ToUser *users = new pms::ToUser;
                    if (!meetingRoom->GetRoomMemberJson(mmsg.usr_from(), users)) {
                        GenericResponse(pms::EServerCmd::CSNDMSG, rmsg, mmsg, users, resp);
                        SendTransferData(resp, (int)resp.length());
                        return;
                    } else {
                        // no member in room
                        delete users;
                        users = nullptr;
                        LE("==>HandleDcommRoom TNOTIFY no member in room id:%s\n", mmsg.rom_id().c_str());
                    }
                }
                break;
            default:
                {

                }
                break;
        }
    } else {

    }
}

void MRTRoomManager::EnterRoom(pms::RelayMsg& rmsg, pms::MeetMsg& mmsg)
{
    //* 1, Find Room
    MeetingRoomMapIt it = m_meetingRoomMap.find(mmsg.rom_id());
    if (it == m_meetingRoomMap.end()) { // meeting not exists
        LE("==>EnterRoom % Room %s not exists, new it\n", mmsg.rom_id().c_str(), __FUNCTION__);
        m_meetingRoomMap.insert(make_pair(mmsg.rom_id(), new rtc::RefCountedObject<MRTMeetingRoom>(mmsg.rom_id(), "")));
        it = m_meetingRoomMap.find(mmsg.rom_id());

        //* Get room member list from httpSvr.
        //* Set GetMembersStatus to Waitting.
        if (m_pHttpSvrConn) {
            m_pHttpSvrConn->HttpGetMeetingMemberList(rmsg, mmsg);
            it->second->SetGetMembersStatus(MRTMeetingRoom::GMS_WAITTING);
        } else {
            LE("EnterRoom m_pHttpSvrConn is NULL\n");
            Assert(false);
        }
    }

    //* 2, Add member to RoomList.
    LI("EnterRoom User %s to Room %s\n", mmsg.usr_from().c_str(), mmsg.rom_id().c_str());
    it->second->AddMemberToRoom(mmsg.usr_from(), MRTMeetingRoom::MemberStatus::MS_INMEETING);
    AddUserMeetingRoomId(mmsg.usr_from(), mmsg.rom_id());
    int online = it->second->GetMeetingMemberNumber();
    if (online==1) {
        if (m_pHttpSvrConn) {
            m_pHttpSvrConn->HttpInsertSessionMeetingInfo(mmsg.usr_token().c_str(), mmsg.rom_id().c_str(), it->second->GetSessionId().c_str(), "0", "0", "1");
            m_pHttpSvrConn->HttpUpdateSessionMeetingStatus(mmsg.usr_token().c_str(), it->second->GetSessionId().c_str(), "1");
        }
    }
    if (m_pHttpSvrConn) {
        char mem[4] = {0};
        sprintf(mem, "%d", online);
        m_pHttpSvrConn->HttpUpdateSessionMeetingNumber(mmsg.usr_token().c_str(), it->second->GetSessionId().c_str(), mem, mmsg.rom_id().c_str());
    }

    //* 3, Notify all members, i'm online.
    std::string resp("");
    pms::ToUser *users = new pms::ToUser;
    if (!it->second->GetAllRoomMemberJson(users)) {
        if (users->users_size()>0) {
            //if (it->second->GetGetMembersStatus()!=MRTMeetingRoom::GetMembersStatus::GMS_DONE) {
            //    //store message
            //    LI("==>HandleDcommRoom GetGetMembersStatus WAITING...\n");
            //    it->second->AddWaitingMsgToList(1, 1, tmsg, mmsg);
            //} else {
                mmsg.set_mem_num(online);
                mmsg.set_msg_cont(mmsg.usr_from());
                GenericResponse(pms::EServerCmd::CSNDMSG, rmsg, mmsg, users, resp);
                SendTransferData(resp, (int)resp.length());
            //}
        } else {
             delete users;
             users = nullptr;
        }
    } else {
        delete users;
        users = nullptr;
    }

    //* 5, Enter room done!
    // Need send back event to client?
    return;
}

void MRTRoomManager::LeaveRoom(pms::RelayMsg& rmsg, pms::MeetMsg& mmsg)
{
    //* 1, Remove myself in Room MemberList;
    MeetingRoomMapIt it = m_meetingRoomMap.find(mmsg.rom_id());
    if (it != m_meetingRoomMap.end()) {
        if (it->second) {
            it->second->DelMemberFmMeeting(mmsg.usr_from());
            DelUserMeetingRoomId(mmsg.usr_from());
        } else {
            return;
        }
    } else {
        // not find room
        return;
    }

    //* 2, Notify Other members
    std::string resp("");
    pms::ToUser *users = new pms::ToUser;
    int online = it->second->GetMeetingMemberNumber();
    if (!it->second->GetAllRoomMemberJson(users)) {
        if (users->users_size()>0) {
            //if (it->second->GetGetMembersStatus()!=MRTMeetingRoom::GetMembersStatus::GMS_DONE) {
                //store message
            //    LI("==>HandleDcommRoom GetGetMembersStatus WAITING...\n");
            //    it->second->AddWaitingMsgToList(1, 1, tmsg, mmsg);
            //} else {
                mmsg.set_msg_cont(mmsg.usr_from());
                mmsg.set_mem_num(online);
                GenericResponse(pms::EServerCmd::CSNDMSG, rmsg, mmsg, users, resp);
                SendTransferData(resp, (int)resp.length());
            //}
        } else {
             delete users;
             users = nullptr;
        }
    } else {
        delete users;
        users = nullptr;
    }
    LI("LeaveRoom User %s to Room %s\n", mmsg.usr_from().c_str(), mmsg.rom_id().c_str());

    //* 3, Check size of Room MemberList offline is 0?
    // If 0, maybe release this room?
    // If GetMembersStatus is Waitting, need waitting for http response!!!
    if (it->second->GetGetMembersStatus()==MRTMeetingRoom::GetMembersStatus::GMS_WAITTING) {
        //send wait event
    }
    if (m_pHttpSvrConn) {
        char mem[4] = {0};
        sprintf(mem, "%d", online);
        m_pHttpSvrConn->HttpUpdateSessionMeetingNumber(mmsg.usr_token().c_str(), it->second->GetSessionId().c_str(), mem, mmsg.rom_id().c_str());
    }
    if (online==0) {
        if (m_pHttpSvrConn) {
            m_pHttpSvrConn->HttpUpdateSessionMeetingEndtime(mmsg.usr_token().c_str(), it->second->GetSessionId().c_str());
            m_pHttpSvrConn->HttpUpdateSessionMeetingStatus(mmsg.usr_token().c_str(), it->second->GetSessionId().c_str(), "2");
        }
    }
    it->second->ResetSessionId();

    //* 4, Leave room done!
    // Need send back event to client?
    return;
}



/////////////////////////////////////////////////////////////

bool MRTRoomManager::Init(const std::string& msgQueueIp, unsigned short msgQueuePort, const std::string& httpIp, unsigned short httpPort, const std::string& httpHost)
{
    if (m_pRoomDispatcher==NULL)
        m_pRoomDispatcher = new MRTRoomDispatcher();
    return ConnectMsgQueue(msgQueueIp, msgQueuePort) && ConnectHttpSvrConn(httpIp, httpPort, httpHost);
}

bool MRTRoomManager::ConnectMsgQueue(const std::string& msgQueueIp, unsigned short msgQueuePort)
{
    m_pMsgQueueSession = new MRTTransferSession(pms::ETransferModule::MMSGQUEUE);
    m_pMsgQueueSession->Init();
    // conn to msgqueue
    while(!m_pMsgQueueSession->Connect(msgQueueIp, msgQueuePort)) {
        LI("connecting to msgqueue server %s:%u waiting...\n", msgQueueIp.c_str(), msgQueuePort);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, msgQueuePort,  m_pMsgQueueSession->GetSocket()->GetSocketFD());
    m_pMsgQueueSession->EstablishConnection();
    return true;
}

bool MRTRoomManager::TryConnectMsgQueue(const std::string& msgQueueIp, unsigned short msgQueuePort)
{
    Assert(m_pMsgQueueSession==nullptr);

    m_pMsgQueueSession = new MRTTransferSession(pms::ETransferModule::MMSGQUEUE);
    m_pMsgQueueSession->Init();

    bool ok = false;
    int times;
    // conn to msgqueue
    do{
        ok = m_pMsgQueueSession->Connect(msgQueueIp, msgQueuePort);
        usleep(1000*1000);
        LI("try %d times to connect to msgqueue server %s:%u waiting...\n", times, msgQueueIp.c_str(), msgQueuePort);
    }while(!ok && times++ < 5);

    if (ok) {
        LI("%s port:%u, socketFD:%d\n", __FUNCTION__, msgQueuePort,  m_pMsgQueueSession->GetSocket()->GetSocketFD());
        m_pMsgQueueSession->EstablishConnection();
        return true;
    } else {
        if (m_pRoomDispatcher)
            m_pRoomDispatcher->Signal(Task::kIdleEvent);
        return false;
    }
}

bool MRTRoomManager::ConnectHttpSvrConn(const std::string& httpIp, unsigned short httpPort, const std::string& httpHost)
{
    if (!m_pHttpSvrConn) {
        m_pHttpSvrConn = new rtc::RefCountedObject<MRTHttpSvrConn>();
        m_pHttpSvrConn->SetHttpHost(httpIp, httpPort, httpHost);
    }
    return true;
}

void MRTRoomManager::SendTransferData(const std::string strData, int nLen)
{
    if (m_pMsgQueueSession) {
        m_pMsgQueueSession->SendTransferData(strData.c_str(), nLen);
    } else {
        LE("%s m_pMsgQueueSession %d NULL error\n", __FUNCTION__, __LINE__);
    }
}

void MRTRoomManager::CheckMembers()
{
#if 0
    MeetingRoomMapIt it = m_meetingRoomMap.begin();
    for (; it!=m_meetingRoomMap.end(); it++) {
        LI("meetingRoom roomMember:%d, online:%d\n", it->second->GetRoomMemberNumber(), it->second->GetMeetingMemberNumber());
    }
#endif
}

void MRTRoomManager::SyncHttpRequest()
{

}

void MRTRoomManager::ClearSessionLost(const std::string& uid, const std::string& token, const std::string& connector)
{
    if (m_meetingRoomMap.size()==0) {
        return;
    }
    const std::string roomid = GetUserMeetingRoomId(uid);
    if (roomid.length()==0) {
        return;
    }
    MeetingRoomMapIt it = m_meetingRoomMap.find(roomid);
    if (it==m_meetingRoomMap.end()) {
        return;
    }

    it->second->DelMemberFmMeeting(uid);
    DelUserMeetingRoomId(uid);
    int online = it->second->GetMeetingMemberNumber();
    char strOnline[4] = {0};
    sprintf(strOnline, "%d", online);

    // notify member having room uid leaving
    std::string resp("");
    pms::ToUser *users = new pms::ToUser;
    it->second->GetRoomMemberJson(uid, users);
    GenericConnLostResponse(uid, token, roomid, connector, online, uid, users, resp);
    SendTransferData(resp, (int)resp.length());
    if (it->second->GetGetMembersStatus()!=MRTMeetingRoom::GetMembersStatus::GMS_DONE) {
        //store message
        LI("ClearSessionLost GetGetMembersStatus WAITING...\n");
        //it->second->AddWaitingMsgToList(1, 1, resp);
    } else {

    }

    if (m_pHttpSvrConn) {
        m_pHttpSvrConn->HttpUpdateSessionMeetingNumber(token.c_str(), it->second->GetSessionId().c_str(), strOnline, it->second->GetRoomId().c_str());
    }
    if (online==0) {
        if (m_pHttpSvrConn) {
            m_pHttpSvrConn->HttpUpdateSessionMeetingEndtime(token.c_str(), it->second->GetSessionId().c_str());
            m_pHttpSvrConn->HttpUpdateSessionMeetingStatus(token.c_str(), it->second->GetSessionId().c_str(), "2");
        }
    }
    it->second->ResetSessionId();

}

void MRTRoomManager::ClearMsgQueueSession(const std::string& sid)
{
    // the object MsgQueue RTTransferSession will be delete in Task
    // here just make the pointer to NULL
    m_pMsgQueueSession = NULL;
    LI("ClearMsgQueueSession make m_pMsgQueueSession = NULL sid:%s\n", sid.c_str());
}


bool MRTRoomManager::ClearAll()
{
    if (m_pRoomDispatcher)
        m_pRoomDispatcher->Signal(Task::kKillEvent);
    return true;
}

void MRTRoomManager::ProcessTickEvent(const char*pData, int nLen)
{
    Assert(m_pMsgQueueSession);
    if (m_pMsgQueueSession->GetConnectingStatus()==0) {
        bool ok = false;
        int times = 0;
        do{
            ok = m_pMsgQueueSession->Connect();
            usleep(2000*1000);
        }while(!ok && ++times < 5);
        if (m_pRoomDispatcher)
            m_pRoomDispatcher->Signal(Task::kIdleEvent);
    } else if (m_pMsgQueueSession->GetConnectingStatus() == 1) {
        m_pMsgQueueSession->EstablishConnection();
    }
}



////////////////////////////////////////////////////
/////////////////private////////////////////////////
////////////////////////////////////////////////////

int MRTRoomManager::ChangeToJson(const std::string from, std::string& users)
{
#if DEF_PROTO
    pms::ToUser touser;
    touser.add_users(from);
    users = touser.SerializeAsString();
#else
#endif
    return 0;
}

void MRTRoomManager::SendWaitingMsgs(MeetingRoomMapIt mit)
{
    //TODO:
    MRTMeetingRoom::WaitingMsgsList pWml = mit->second->GetWaitingMsgs();
    if (pWml.size()==0) {
        return;
    }
    LI("SendWaitingMsgs size:%d, roomid:%s\n", (int)pWml.size(), mit->second->GetRoomId().c_str());
    std::string resp("");
    //TODO: check roomUsers delete
    pms::ToUser *roomUsers = new pms::ToUser;
    mit->second->GetAllRoomMemberJson(roomUsers);
    MRTMeetingRoom::WaitingMsgsList::iterator wit = pWml.begin();
    for (; wit!=pWml.end(); wit++) {
        MRTMeetingRoom::WaitingMsg wm = *wit;
        GenericResponse(pms::EServerCmd::CSNDMSG, wm._wtmsg, wm._wmsg, roomUsers, resp);
        SendTransferData(resp.c_str(), (int)resp.length());
    }
    pWml.clear();
}

void MRTRoomManager::OnGetMemberList(pms::RelayMsg& rmsg, pms::MeetMsg& mmsg, std::string& data)
{
#if DEF_PROTO
    //* 1, Update room member list.
    //* 2, Set GetMembersStatus to Done, and notify other members(not in room) meeting is opened.
    //* 3, Check msgs list, and send to notify other members(not in room)
    MeetingRoomMapIt it = m_meetingRoomMap.find(mmsg.rom_id());
    if (it != m_meetingRoomMap.end()) {
        MEETINGMEMBERLIST memList;
        std::string err;
        memList.GetMsg(data, err);
        if (err.length()==0) {
            it->second->UpdateMemberList(memList._uslist);
        } else {
            LE("OnGetMemberList error:%s, mmsg.rom_id:%s\n", err.c_str(), mmsg.rom_id().c_str());
            assert(false);
        }
    } else {
        // not find room
        return;
    }

    it->second->SetGetMembersStatus(MRTMeetingRoom::GetMembersStatus::GMS_DONE);
    SendWaitingMsgs(it);
#else
    LE("not define DEF_PROTO\n");
#endif
}

void MRTRoomManager::GenericResponse(pms::EServerCmd cmd, const pms::RelayMsg& rmsg, const pms::MeetMsg& mmsg, const pms::ToUser* tos, std::string& response)
{
    switch (mmsg.msg_tag()) {
        case pms::EMsgTag::TENTER:
        case pms::EMsgTag::TLEAVE:
        case pms::EMsgTag::TCHAT:
        case pms::EMsgTag::TNOTIFY:
            ResponseSndMsg(cmd, rmsg, mmsg, tos, response);
            break;
        default:
            LE("mmsg._cmd error:%d !!!\n", mmsg.msg_tag());
            break;
    }
}

void MRTRoomManager::GenericConnLostResponse(const std::string& uid
                                            , const std::string& token
                                            , const std::string& roomid
                                            , const std::string& connector
                                            , int nmem
                                            , const std::string& cont
                                            , const pms::ToUser* tos
                                            , std::string& response)
{
#if DEF_PROTO
    //LI("GenericConnLostResponse cont:%s\n", cont.c_str());
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;
    pms::MsgRep resp;
    pms::MeetMsg mmsg;

    mmsg.set_msg_head(pms::EMsgHead::HSND);
    mmsg.set_msg_tag(pms::EMsgTag::TLEAVE);
    mmsg.set_msg_type(pms::EMsgType::TMSG);
    mmsg.set_usr_from(uid);
    mmsg.set_usr_token(token);
    mmsg.set_rom_id(roomid);
    mmsg.set_mem_num(nmem);
    mmsg.set_msg_cont(cont);


    LI("GenericConnLostResponse meet msg--->:\n");
    //mmsg.PrintDebugString();
    // set response
    resp.set_svr_cmds(pms::EServerCmd::CLOGOUT);
    resp.set_mod_type(pms::EModuleType::TMEETING);
    resp.set_rsp_code(0);
    resp.set_rsp_cont(mmsg.SerializeAsString());

    // set relay
    r_msg.set_svr_cmds(pms::EServerCmd::CLOGOUT);
    r_msg.set_tr_module(pms::ETransferModule::MMEETING);
    r_msg.set_connector(connector);
    r_msg.set_content(resp.SerializeAsString());
    r_msg.set_allocated_touser((pms::ToUser*)tos);

    // set transfer
    t_msg.set_type(pms::ETransferType::TQUEUE);
    t_msg.set_content(r_msg.SerializeAsString());
    response = t_msg.SerializeAsString();
#else
#endif
}

void MRTRoomManager::ResponseSndMsg(pms::EServerCmd cmd, const pms::RelayMsg& rmsg, const pms::MeetMsg& mmsg, const pms::ToUser* tos,  std::string& response)
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;
    pms::MsgRep resp;

    LI("ResponseSndMsg meet msg--->:\n");
    //mmsg.PrintDebugString();
    // set response
    resp.set_svr_cmds(cmd);
    resp.set_mod_type(pms::EModuleType::TMEETING);
    resp.set_rsp_code(0);
    resp.set_rsp_cont(mmsg.SerializeAsString());

    // set relay
    r_msg.set_svr_cmds(cmd);
    r_msg.set_tr_module(pms::ETransferModule::MMEETING);
    r_msg.set_connector(rmsg.connector());
    r_msg.set_content(resp.SerializeAsString());
    r_msg.set_allocated_touser((pms::ToUser*)tos);

    // set transfer
    t_msg.set_type(pms::ETransferType::TQUEUE);
    t_msg.set_content(r_msg.SerializeAsString());
    response = t_msg.SerializeAsString();
#else
    LE("not define DEF_PROTO\n");
#endif
}
