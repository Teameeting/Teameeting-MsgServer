//
//  PRTTransferSession.cpp
//  dyncRTModule
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "PRTTransferSession.h"
#include "RTUtils.hpp"
#include "PRTConnManager.h"
#include "PRTPusherManager.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#include "MsgServer/proto/storage_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/entity_msg.pb.h"
#include "MsgServer/proto/entity_msg_type.pb.h"

#include "IosPusher.h"


#define TIMEOUT_TS (60*1000)

PRTTransferSession::PRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
, m_pPushMsgProcesser(nullptr)
, m_module(pms::EModuleType::TLIVE)
{
    AddObserver(this);
}

PRTTransferSession::~PRTTransferSession()
{
    DelObserver(this);
    Unit();
}

void PRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(96L * 1024L);

    socket->SetTask(this);
    this->SetTimer(120*1000);
    if (!m_pPushMsgProcesser) {
        m_pPushMsgProcesser = new XPushMsgProcesser(*this);
    }
    if (!m_pPushMsgProcesser) {
        LE("PRTTransferSession XPushMsgProcesser new error");
        assert(false);
    }
    m_pPushMsgProcesser->Init();
}

void PRTTransferSession::Unit()
{
    Disconn();
    m_connectingStatus = 0;
}

bool PRTTransferSession::Connect(const std::string addr, int port)
{
    m_addr = addr;
    m_port = port;
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

bool PRTTransferSession::Connect()
{
    if (m_addr.empty() || m_port < 2048) {
        LE("%s invalid params addr:%s, port:%d\n", __FUNCTION__, m_addr.c_str(), m_port);
        return false;
    }
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

void PRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool PRTTransferSession::RefreshTime()
{
    UInt64 now = OS::Milliseconds();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        RTTcpNoTimeout::UpdateTimer();
        return true;
    } else {
        return false;
    }
}

void PRTTransferSession::KeepAlive()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MPUSHER);
    c_msg.set_conn_tag(pms::EConnTag::TKEEPALIVE);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNOACK);
    t_msg.set_priority(pms::ETransferPriority::PNORMAL);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void PRTTransferSession::TestConnection()
{

}

void PRTTransferSession::EstablishConnection()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MPUSHER);
    c_msg.set_conn_tag(pms::EConnTag::THI);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
    t_msg.set_priority(pms::ETransferPriority::PHIGH);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void PRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    PRTPusherManager::Instance().SendResponseCounter();
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void PRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

// from RTTcpNoTimeout
void PRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void PRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    LI("PRTTransferSession::OnRecvMessage ~~~~~~~~~~~~~~~~~~\n");
    RTTransfer::DoProcessData(message, nLen);
}


// from RTTransfer

void PRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcpNoTimeout::SendTransferData(str.c_str(), (int)str.length());
}

void PRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
#if DEF_PROTO
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    OnTransfer(ack_msg.SerializeAsString());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void PRTTransferSession::OnTypeConn(const std::string& str)
{
#if DEF_PROTO
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg ParseFromString error\n");
        return;
    }
    LI("OnTypeConn connmsg--->:\n");

    if ((c_msg.conn_tag() == pms::EConnTag::THI)) {
        // when other connect to ME:
        // send the transfersessionid and ModuleId to other
        pms::TransferMsg t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        c_msg.set_tr_module(pms::ETransferModule::MPUSHER);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self Module id to other
        c_msg.set_moduleid(PRTConnManager::Instance().ModuleId());

        t_msg.set_type(pms::ETransferType::TCONN);
        //this is for transfer
        t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
        t_msg.set_priority(pms::ETransferPriority::PHIGH);
        t_msg.set_content(c_msg.SerializeAsString());

        std::string s = t_msg.SerializeAsString();
        SendTransferData(s.c_str(), (int)s.length());
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLO)) {
        // when ME connector to other:
        // store other's transfersessionid and other's moduleId
        if (c_msg.transferid().length()>0) {
            m_transferSessId = c_msg.transferid();
            {
                PRTConnManager::ModuleInfo* pmi = new PRTConnManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = c_msg.tr_module();
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    //bind session and transfer id
                    PRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                    PRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MPUSHER);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self Module id to other
            c_msg.set_moduleid(PRTConnManager::Instance().ModuleId());

            t_msg.set_type(pms::ETransferType::TCONN);
            //this is for transfer
            t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
            t_msg.set_priority(pms::ETransferPriority::PHIGH);
            t_msg.set_content(c_msg.SerializeAsString());

            std::string s = t_msg.SerializeAsString();
            SendTransferData(s.c_str(), (int)s.length());
        } else {
            LE("Connection id:%s error!!!\n", c_msg.transferid().c_str());
        }
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLOHI)) {
        // when other connect to ME:
        if (m_transferSessId.compare(c_msg.transferid()) == 0) {
            PRTConnManager::ModuleInfo* pmi = new PRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                PRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store module type:%d, moduleid:%s, transfersessid:%s\n", (int)c_msg.tr_module(), c_msg.moduleid().c_str(), m_transferSessId.c_str());
                PRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
            } else {
                LE("new ModuleInfo error!!!!\n");
            }
        }
    }  else if (c_msg.conn_tag() == pms::EConnTag::TKEEPALIVE) {
        RTTcpNoTimeout::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void PRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called, str:%lu\n", __FUNCTION__, str.length());
    PRTPusherManager::Instance().RecvRequestCounter();

}

// this is from connector sequence push
void PRTTransferSession::OnTypeQueue(const std::string& str)
{
    //LI("%s was called, str:%s\n", __FUNCTION__, str.c_str());
    pms::RelayMsg r_msg;
    if (!r_msg.ParseFromString(str)) {
        LE("PRTTransferSession::OnTypeQueue r_msg ParseFromString error\n");
        return;
    }
    LI("PRTTransferSession::OnTypeQueue svrcmd:%d, trmodule:%d, connector:%s, handlecmd:%s, handlemtype:%s, handledata:%s, users.size:%d, user.name:%s\n\n"\
            , r_msg.svr_cmds()\
            , r_msg.tr_module()\
            , r_msg.content().c_str()\
            , r_msg.handle_cmd().c_str()\
            , r_msg.handle_mtype().c_str()\
            , r_msg.handle_data().c_str()\
            , r_msg.touser().users_size()\
            , r_msg.touser().users(0).c_str());

    pms::MsgRep resp;
    if (!resp.ParseFromString(r_msg.content()))
    {
        LE("PRTTransferSession::OnTypeQueue resp ParseFromString error\n");
        return;
    }
    LI("PRTTransferSession::OnTypeQueue resp svr_cmd:%d, mod_type:%d, code:%d\n\n"\
            , resp.svr_cmds()\
            , resp.mod_type()\
            , resp.rsp_code());

    pms::StorageMsg store;
    if (!store.ParseFromString(resp.rsp_cont()))
    {
        LE("PRTTransferSession::OnTypeQueue store ParseFromString error\n");
        return;
    }
    LI("PRTTransferSession::OnTypeQueue sequence:%lld, ruseid:%s, storeid:%s, ispush:%s, version:%s, groupid:%s, mtype:%s, group or single flag:%d\n\n"\
            , store.sequence()\
            , store.ruserid().c_str()\
            , store.storeid().c_str()\
            , store.ispush().c_str()\
            , store.version().c_str()\
            , store.groupid().c_str()\
            , store.mtype().c_str()\
            , store.mflag());

    store.set_rsvrcmd(pms::EServerCmd::CPGETDATA);
    store.set_tsvrcmd(pms::EServerCmd::CPGETDATA);
    store.set_mtag(pms::EStorageTag::TDATA);
    std::string outstr("");
    if (m_pPushMsgProcesser) {
        m_pPushMsgProcesser->EncodePushGetDataNotify(outstr, store, m_module);
    } else {
        LE("PRTTransferSession::OnTypeQueue m_pPushMsgProcesser is null  return~~~~\n");
        return;
    }
    if (outstr.length()==0)
    {
        LE("PRTTransferSession::OnTypeQueue outstr length is 0000000000 return~~~~\n");
        return;
    }
    PRTConnManager::ModuleInfo* pmodule = PRTConnManager::Instance().findModuleInfo("", pms::ETransferModule::MLIVE);
    if (pmodule && pmodule->pModule && pmodule->pModule->IsLiveSession())
    {
        LI("PRTTransferSession::OnTypeQueue module session is live~~~~\n");
        pmodule->pModule->SendTransferData(outstr);
    }

}

void PRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    pms::StorageMsg store;
    if (!store.ParseFromString(str))
    {
        LE("PRTTransferSession::OnTypeDispatch store ParseFromString error\n");
        return;
    }
    LI("PRTTransferSession::OnTypeDispatch sequence:%lld, ruseid:%s, storeid:%s, ispush:%s, version:%s, groupid:%s, mtype:%s, group or single flag:%d\n\n"\
            , store.sequence()\
            , store.ruserid().c_str()\
            , store.storeid().c_str()\
            , store.ispush().c_str()\
            , store.version().c_str()\
            , store.groupid().c_str()\
            , store.mtype().c_str()\
            , store.mflag());
    pms::Entity entity;
    if (!entity.ParseFromString(store.content()))
    {
        LE("PRTTransferSession::OnTypeDispatch entity ParseFromString error\n");
        return;
    }
    std::string fromId, nickName, groupId;
    int msgType;
    rapidjson::Document jsonMsgDoc;
    if (!jsonMsgDoc.ParseInsitu<0>((char*)entity.msg_cont().c_str()).HasParseError())
    {
        if (jsonMsgDoc.HasMember("fromId"))
        {
            fromId = jsonMsgDoc["fromId"].GetString();
            LI("push msg fromId :%s\n", fromId.c_str());
        } else {
            LE("in jsonMsgDoc not has member fromId, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        if (jsonMsgDoc.HasMember("nickName"))
        {
            nickName = jsonMsgDoc["nickName"].GetString();
            LI("push msg nickName :%s\n", nickName.c_str());
        } else {
            LE("in jsonMsgDoc not has member nickname, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        if (jsonMsgDoc.HasMember("groupId"))
        {
            groupId = jsonMsgDoc["groupId"].GetString();
            LI("push msg groupId: %s\n", groupId.c_str());
        } else {
            LE("in jsonMsgDoc not has member groupId, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        if (jsonMsgDoc.HasMember("msgType"))
        {
            msgType = jsonMsgDoc["msgType"].GetInt();
            LI("push msg msgType :%d\n", msgType);
        } else {
            LE("in jsonMsgDoc not has member msgType, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        LI("PRTTransferSession::OnTypeDispatch invoke PushMsg test, rapidjson fromId:%s, nickName:%s, groupId:%s\n", fromId.c_str(), nickName.c_str(), groupId.c_str());
        //IosPusher::Instance().ConfigPayload();
        IosPusher::Instance().PushMsg();
    } else {
        LE("parse msg:%s error\n", entity.msg_cont().c_str());
        return;
    }

}

void PRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void PRTTransferSession::OnTypeTLogin(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void PRTTransferSession::OnTypeTLogout(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

// this is get msg data
void PRTTransferSession::OnPGetData(int code, const std::string& cont)
{
     LI("PRTTransferSession::OnPGetData was called, code:%d\n", code);
}

void PRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        PRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

