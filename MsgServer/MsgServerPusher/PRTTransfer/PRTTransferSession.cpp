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
, m_module(pms::EModuleType::TPUSHER)
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
    std::vector<std::string> vec;
    PRTPusherManager::Instance().GetRedisHosts(&vec);
    if (vec.size()>0)
    {
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(vec.at(0).c_str(), "%s %u", ip, &port);
        LI("PUSHER redis ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048)
            m_xRedis.Init(ip, port);
        else
            LE("xRedis ip or port error\n");
    } else {
        LE("PRTTransferSession::Init redis vec.size:%d not bigger 0\n", vec.size());
    }
}

void PRTTransferSession::Unit()
{
    if (m_pPushMsgProcesser)
    {
        m_pPushMsgProcesser->Unin();
        delete m_pPushMsgProcesser;
        m_pPushMsgProcesser = nullptr;
    }

    m_xRedis.Unin();
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

void PRTTransferSession::OnPushEvent(const char*pData, int nLen)
{
    // get 10 needed push msg in redis each time
    int64_t start=0, stop=0;
    ArrayReply reply;
    if (!m_xRedis.GetNeedPushMsg("ios", reply, start, stop))
    {
        LE("PRTTransferSession::OnPushEvent GetNeedPushMsg start:%lld stop:%lld failed\n", start, stop);
        return;
    }
    LI("PRTTransferSession::OnPushEvent GetNeedPushMsg reply.size:%d\n", reply.size());

    // parse each msg one by one
    ReplyData::iterator iter = reply.begin();
    for (; iter != reply.end(); iter++) {
        if (((*iter).str.empty()) || ((*iter).str.length()==0)) continue;

        pms::Pushing pushMsg;
        if (!pushMsg.ParseFromString((*iter).str))
        {
            LE("PRTTransferSession::OnPushEvent pushMsg.ParseFromString failed, continue\n");
            continue;
        }
        std::string strJson = pushMsg.content();
        rapidjson::Document jReqDoc;
        if (jReqDoc.ParseInsitu<0>((char*)strJson.c_str()).HasParseError())
        {
            LE("PRTTransferSession::OnPushEvent ParseInsitu failed\n");
            continue;
        }

        if (!(jReqDoc.HasMember("token")&&jReqDoc["token"].IsString()))
        {
            LE("PRTTransferSession::OnPushEvent has no member token\n");
            continue;
        }
        std::string token = jReqDoc["token"].GetString();

        if (!(jReqDoc.HasMember("cont")&&jReqDoc["cont"].IsString()))
        {
            LE("PRTTransferSession::OnPushEvent has no member cont\n");
            continue;
        }
        std::string cont = jReqDoc["cont"].GetString();

        if (!(jReqDoc.HasMember("groupid")&&jReqDoc["groupid"].IsString()))
        {
            LE("PRTTransferSession::OnPushEvent has no member groupid\n");
            continue;
        }
        std::string groupid = jReqDoc["groupid"].GetString();

        if (!(jReqDoc.HasMember("frommsgid")&&jReqDoc["frommsgid"].IsString()))
        {
            LE("PRTTransferSession::OnPushEvent has no member frommsgid\n");
            continue;
        }
        std::string frommsgid = jReqDoc["frommsgid"].GetString();

        if (token.length()==0 || cont.length()==0 || groupid.length()==0 || frommsgid.length()==0)
        {
            LE("PRTTransferSession::OnPushEvent member length is 0\n");
            continue;
        }

        std::vector<const char*> extra;
        extra.push_back("groupid");
        extra.push_back(groupid.c_str());
        extra.push_back("frommsgid");
        extra.push_back(frommsgid.c_str());
        LI("PRTTransferSession::OnPushEvent GetNeedPushMsg call push msg\n");
        if (!IosPusher::Instance().PushMsg(token.c_str(), cont.c_str(), extra.data(), extra.size()))
        {
            // push failed, so store msg to redis
            LI("PRTTransferSession::OnPushEvent SetNeedPushMsg call push msg:%s\n", pushMsg.content().c_str());
            m_xRedis.SetNeedPushMsg("ios", pushMsg.SerializeAsString());
        }
    }

    // if still has need push msg in redis, signal
    int64_t count = 0;
    if (m_xRedis.LenListMsg("ios", count))
    {
        LI("PRTTransferSession::OnPushEvent LenListMsg count:%lld\n", count);
        if (count > 0)
            this->Signal(Task::kPushEvent);
    }
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

// this is from rtlive module push data
void PRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    pms::MsgRep resp;
    if (!resp.ParseFromString(str))
    {
        LE("PRTTransferSession::OnTypeDispatch resp ParseFromString error\n");
        return;
    }

    pms::StorageMsg store;
    if (!store.ParseFromString(resp.rsp_cont()))
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

    // parse to get msg info
    std::string fromId(""), nickName(""), groupId(""), toNickName(""), toId("");
    int msgType;
    rapidjson::Document jsonMsgDoc;
    if (!jsonMsgDoc.ParseInsitu<0>((char*)entity.msg_cont().c_str()).HasParseError())
    {
        if (jsonMsgDoc.HasMember("fromId")&&jsonMsgDoc["fromId"].IsString())
        {
            fromId = jsonMsgDoc["fromId"].GetString();
            LI("push msg fromId :%s\n", fromId.c_str());
        } else {
            LE("in jsonMsgDoc not has member fromId, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        if (jsonMsgDoc.HasMember("nickName")&&jsonMsgDoc["nickName"].IsString())
        {
            nickName = jsonMsgDoc["nickName"].GetString();
            LI("push msg nickName :%s\n", nickName.c_str());
        } else {
            LE("in jsonMsgDoc not has member nickname, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        if (jsonMsgDoc.HasMember("groupId")&&jsonMsgDoc["groupId"].IsString())
        {
            groupId = jsonMsgDoc["groupId"].GetString();
            LI("push msg groupId: %s\n", groupId.c_str());
        } else {
            LE("in jsonMsgDoc not has member groupId, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        if (jsonMsgDoc.HasMember("toNickName")&&jsonMsgDoc["toNickName"].IsString())
        {
            toNickName = jsonMsgDoc["toNickName"].GetString();
            LI("push msg toNickName :%s\n", toNickName.c_str());
        } else {
            LE("in jsonMsgDoc not has member toNickName, jstr:%s\n", entity.msg_cont().c_str());
        }
        if (jsonMsgDoc.HasMember("toId")&&jsonMsgDoc["toId"].IsString())
        {
            toId = jsonMsgDoc["toId"].GetString();
            LI("push msg toId :%s\n", toId.c_str());
        } else {
            LE("in jsonMsgDoc not has member toId, jstr:%s\n", entity.msg_cont().c_str());
        }
        if (jsonMsgDoc.HasMember("msgType")&&jsonMsgDoc["msgType"].IsInt())
        {
            msgType = jsonMsgDoc["msgType"].GetInt();
            LI("push msg msgType :%d\n", msgType);
        } else {
            LE("in jsonMsgDoc not has member msgType, jstr:%s\n", entity.msg_cont().c_str());
            return;
        }
        LI("PRTTransferSession::OnTypeDispatch invoke PushMsg test, rapidjson fromId:%s, nickName:%s, groupId:%s, msgId:%s\n"\
                , fromId.c_str()\
                , nickName.c_str()\
                , groupId.c_str()\
                , toNickName.c_str()\
                , entity.cmsg_id().c_str());


        // store.ruserid() is that this push msg will push to
        // fromId is that this entity msg is send from
        // toId is that this entity msg is send to
        std::string token;
        m_xRedis.GetSettingPush(store.ruserid(), resp.mod_type(), "token", token);
        LI("PRTTransferSession::OnTypeDispatch user:%s in module:%d push token is:%s\n", store.ruserid().c_str(), resp.mod_type(), token.c_str());
        if (token.length()==0) return;

        LI("PRTTransferSession::OnTypeDispatch msg type is:%d\n", msgType);
        // compose push msg
        char cont[128] = {0};
        switch (msgType) {
        case pms::EMsgType::TLIV:
             {
                 if (store.ruserid().compare(fromId)==0) break; // push to sender is not fittable
                 sprintf(cont, "%s 正在直播，赶快来看吧!!!", nickName.c_str());
             }
             break;
        case pms::EMsgType::TREN:
             {
                 if (store.ruserid().compare(fromId)==0) break; // push to sender is not fittable
                 if (store.ruserid().compare(toId)==0) { // push to client
                     sprintf(cont, "%s 给你发送了红包，快去查看吧!!!", nickName.c_str());
                 } else {
                     sprintf(cont, "%s 给 %s 发送了红包，快去查看吧!!!", nickName.c_str(), toNickName.c_str());
                 }
             }
             break;
        case pms::EMsgType::TBLK:
             {
                 if (store.ruserid().compare(fromId)==0) break; // push to sender is not fittable
                 if (store.ruserid().compare(toId)==0) { // push to client
                     sprintf(cont, "%s 把你拉入了黑名单!!!", nickName.c_str());
                 } else {
                     sprintf(cont, "%s 把 %s 拉入了黑名单!!!", nickName.c_str(), toNickName.c_str());
                 }
             }
             break;
        case pms::EMsgType::TFBD:
             {
                 if (store.ruserid().compare(fromId)==0) break; // push to sender is not fittable
                 if (store.ruserid().compare(toId)==0) { // push to client
                     sprintf(cont, "%s 把你禁言了!!!", nickName.c_str());
                 } else {
                     sprintf(cont, "%s 把 %s 禁言了!!!", nickName.c_str(), toNickName.c_str());
                 }
             }
             break;
        case pms::EMsgType::TMGR:
             {
                 if (store.ruserid().compare(fromId)==0) break; // push to sender is not fittable
                 if (store.ruserid().compare(toId)==0) { // push to client
                     sprintf(cont, "%s 把你设置为管理员!!!", nickName.c_str());
                 } else {
                     sprintf(cont, "%s 把 %s 设置为管理员!!!", nickName.c_str(), toNickName.c_str());
                 }
             }
             break;
        case pms::EMsgType::TTXT:
             {
                 if (nickName.length()==0 || store.ruserid().compare(fromId)==0) break; // push to sender is not fittable
                 sprintf(cont, "%s 主播发来一条消息!!!", nickName.c_str());
             }
             break;
        default:
             return;
        }

        if (strlen(cont)==0) return;
        LI("push content:%s\n", cont);

        // redis push setting key:'push:set:module:userid' by hmset, 'token':token, 'enablepush':enablepush
        // redis push msg key:'push:msg:module:userid' by lpush, value...
        std::string fromMsgId = fromId + ":" + entity.cmsg_id();
        rapidjson::Document jDoc;
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        jDoc.SetObject();
        jDoc.AddMember("token", token.c_str(), jDoc.GetAllocator());
        jDoc.AddMember("cont", cont, jDoc.GetAllocator());
        jDoc.AddMember("groupid", groupId.c_str(), jDoc.GetAllocator());
        jDoc.AddMember("frommsgid", fromMsgId.c_str(), jDoc.GetAllocator());
        jDoc.AddMember("userid", store.ruserid().c_str(), jDoc.GetAllocator());
        jDoc.AddMember("modtype", resp.mod_type(), jDoc.GetAllocator());
        jDoc.AddMember("devtype", "ios", jDoc.GetAllocator());

        jDoc.Accept(writer);
        std::string s = sb.GetString();

        pms::Pushing pushMsg;
        pushMsg.set_type("ios");
        pushMsg.set_content(s);
        LI("PRTTransferSession::OnTypeDispatch push msg to redis:%s\n", s.c_str());
        // write to redis
        if (!m_xRedis.SetNeedPushMsg("ios", pushMsg.SerializeAsString()))
        {
            LE("PRTTransferSession::OnTypeDispatch push msg to redis failed\n");
            return;
        }
        this->Signal(Task::kPushEvent);
    } else {
        LE("parse msg:%s error\n", entity.msg_cont().c_str());
        return;
    }
}

// this is from connector update setting
void PRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    pms::RelayMsg r_msg;
    if (!r_msg.ParseFromString(str)) return;
    pms::MsgReq m_req;
    if (!m_req.ParseFromString(r_msg.content())) return;
    pms::Setting m_set;
    if (!m_set.ParseFromString(m_req.content())) return;
    LI("PRTTransferSession::OnTypePush setting usr_from:%s, version:%s, set_type:%lld, json_cont:%s\n" \
            , m_set.usr_from().c_str()\
            , m_set.version().c_str()\
            , m_set.set_type()\
            , m_set.json_cont().c_str());

    int msgType;
    rapidjson::Document jsonMsgDoc;
    if (jsonMsgDoc.ParseInsitu<0>((char*)m_set.json_cont().c_str()).HasParseError())
    {
        LE("PRTTransferSession::OnTypePush parse json cont:%s, error\n", m_set.json_cont().c_str());
        return;
    }
    if (m_set.set_type()==1)
    {
        std::string enablepush;
        if (jsonMsgDoc.HasMember("enablepush")&&jsonMsgDoc["enablepush"].IsString())
        {
            enablepush = jsonMsgDoc["enablepush"].GetString();
            LI("push msg enablepush :%s\n", enablepush.c_str());
        } else {
            LE("in jsonMsgDoc not has member enablepush, jstr:%s\n", m_set.json_cont().c_str());
            return;
        }
        LI("PRTTransferSession::OnTypePush usr:%s, module:%d, setting enablepush:%s\n", m_set.usr_from().c_str(), m_req.mod_type(), enablepush.c_str());
        // redis user push setting key:'push:set:module:userid' by hmset, 'token':token, 'enablepush':enablepush
        m_xRedis.SetSettingPush(m_set.usr_from(), m_req.mod_type(), "enablepush", enablepush);
    } else if (m_set.set_type()==2)
    {
        std::string token;
        if (jsonMsgDoc.HasMember("token")&&jsonMsgDoc["token"].IsString())
        {
            token = jsonMsgDoc["token"].GetString();
            LI("push msg token :%s\n", token.c_str());
        } else {
            LE("in jsonMsgDoc not has member token, jstr:%s\n", m_set.json_cont().c_str());
            return;
        }
        LI("PRTTransferSession::OnTypePush usr:%s, module:%d, setting token:%s\n", m_set.usr_from().c_str(), m_req.mod_type(), token.c_str());
        // redis user push setting key:'push:set:module:userid' by hmset, 'token':token, 'enablepush':enablepush
        m_xRedis.SetSettingPush(m_set.usr_from(), m_req.mod_type(), "token", token);
    } else if (m_set.set_type()==4)
    {
        std::string mutenotify;
        if (jsonMsgDoc.HasMember("mutenotify")&&jsonMsgDoc["mutenotify"].IsString())
        {
            mutenotify = jsonMsgDoc["mutenotify"].GetString();
            LI("push msg mutenotify :%s\n", mutenotify.c_str());
        } else {
            LE("in jsonMsgDoc not has member mutenotify, jstr:%s\n", m_set.json_cont().c_str());
            return;
        }
        LI("PRTTransferSession::OnTypePush usr:%s, module:%d, setting mutenotify:%s\n", m_set.usr_from().c_str(), m_req.mod_type(), mutenotify.c_str());
        // redis user push setting key:'push:set:module:userid' by hmset, 'token':token, 'enablepush':enablepush
        m_xRedis.SetSettingPush(m_set.usr_from(), m_req.mod_type(), "mutenotify", mutenotify);
    }
}

void PRTTransferSession::OnTypeTLogin(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    pms::RelayMsg r_msg;
    if (!r_msg.ParseFromString(str)) return;
    pms::MsgReq m_req;
    if (!m_req.ParseFromString(r_msg.content())) return;
    pms::Login login;
    if (!login.ParseFromString(m_req.content())) {
        LE("login.ParseFromString error\n");
        return;
    }

    LI("PRTTransferSession::OnTypeTLogin usr:%s, module:%d, setting token:%s\n", login.usr_from().c_str(), m_req.mod_type(), login.usr_token().c_str());
    // redis user push setting key:'push:set:module:userid' by hmset, 'token':token, 'enablepush':enablepush
    m_xRedis.SetSettingPush(login.usr_from(), m_req.mod_type(), "token", login.usr_token());
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

