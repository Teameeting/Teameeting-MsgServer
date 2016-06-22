//
//  LRTTransferSession.cpp
//  dyncRTLogical
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "LRTTransferSession.h"
#include "RTMessage.h"
#include "RTUtils.hpp"
#include "LRTConnManager.h"
#include "LRTLogicalManager.h"

#define PACKED_MSG_ONCE_NUM (10)

#define TIMEOUT_TS (60*1000)

static int g_read_event_counter = 0;
static int g_write_event_counter = 0;

LRTTransferSession::LRTTransferSession()
: RTJSBuffer()
, RTLstorage()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
, m_tmpWMsgId(0)
, m_tmpRSeqnId(0)
, m_tmpRDataId(0)
{
    AddObserver(this);
}

LRTTransferSession::~LRTTransferSession()
{
    DelObserver(this);
    Unit();
}

void LRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(96L * 1024L);

    socket->SetTask(this);
    this->SetTimer(120*1000);
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respReadMsg.add_msgs();
    }
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respWriteMsg.add_msgs();
    }
    m_isRun = 1;
}

void LRTTransferSession::InitConf()
{
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respReadMsg.add_msgs();
    }
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respWriteMsg.add_msgs();
    }
    m_isRun = 1;
}

void LRTTransferSession::Unit()
{
    m_isRun = 0;
    Disconn();
    m_connectingStatus = 0;
}

bool LRTTransferSession::Connect(const std::string addr, int port)
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

bool LRTTransferSession::Connect()
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

void LRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool LRTTransferSession::RefreshTime()
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

void LRTTransferSession::KeepAlive()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MSEQUENCE);
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

void LRTTransferSession::TestConnection()
{

}

void LRTTransferSession::EstablishConnection()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MSEQUENCE);
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

void LRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    LRTLogicalManager::Instance().SendResponseCounter();
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void LRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

void LRTTransferSession::PushReadMsg(pms::StorageMsg store)
{
    {
        OSMutexLocker locker(&m_mutexQRead);
        m_queueReadMsg.push(store);
    }
    this->Signal(Task::kPushEvent);
}

void LRTTransferSession::PushWriteMsg(pms::StorageMsg store)
{
    {
        OSMutexLocker locker(&m_mutexQWrite);
        m_queueWriteMsg.push(store);
    }
    this->Signal(Task::kIdleEvent);
}


// from RTTcpNoTimeout
void LRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    LI("LRTTransferSession::OnRecvData was called\n");
    RTJSBuffer::RecvData(pData, nLen);
}

void LRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    LI("LRTTransferSession::OnRecvMessage was called\n");
    RTLstorage::DoProcessData(message, nLen);
}

// for read
void LRTTransferSession::OnPushEvent(const char*pData, int nLen)
{
    if (!m_isRun) return;
    if(m_queueReadMsg.size()==0) return;
    bool hasData = false;
    printf("LRTTransferSession::OnPushEvent g_idle_event_counter:%d, m_queueReadMsg.size:%d\n",  ++g_read_event_counter, m_queueReadMsg.size());
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_queueReadMsg.size()>0)
        {
            hasData = true;
            *(m_respReadMsg.mutable_msgs(i)) = m_queueReadMsg.front();
            {
                OSMutexLocker locker(&m_mutexQRead);
                m_queueReadMsg.pop();
            }
        } else {
             break;
        }
    }
    if (hasData)
    {
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TDISPATCH);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(m_respReadMsg.SerializeAsString());
            this->SendTransferData(tmsg.SerializeAsString());
        }
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_respReadMsg.mutable_msgs(i)->Clear();
        }
    }
    if(m_queueReadMsg.size()>0)
    {
        this->Signal(Task::kPushEvent);
    }
}

// for write
void LRTTransferSession::OnTickEvent(const char*pData, int nLen)
{
    if (!m_isRun) return;
    if(m_queueWriteMsg.size()==0) return;
    bool hasData = false;
    printf("LRTTransferSession::OnTickEvent g_idle_event_counter:%d, m_queueWriteMsg.size:%d\n",  ++g_write_event_counter, m_queueWriteMsg.size());
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_queueWriteMsg.size()>0)
        {
            hasData = true;
            *(m_respWriteMsg.mutable_msgs(i)) = m_queueWriteMsg.front();
            {
                OSMutexLocker locker(&m_mutexQWrite);
                m_queueWriteMsg.pop();
            }
        } else {
             break;
        }
    }
    if (hasData)
    {
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TQUEUE);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(m_respWriteMsg.SerializeAsString());
            this->SendTransferData(tmsg.SerializeAsString());
        }
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_respWriteMsg.mutable_msgs(i)->Clear();
        }
    }
    if(m_queueWriteMsg.size()>0)
    {
        this->Signal(Task::kIdleEvent);
    }
}

// from RTTransfer

void LRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcpNoTimeout::SendTransferData(str.c_str(), (int)str.length());
}

void LRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
#if DEF_PROTO
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    //OnTransfer(ack_msg.SerializeAsString());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void LRTTransferSession::OnTypeConn(const std::string& str)
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
        // send the transfersessionid and LogicalId to other
        pms::TransferMsg t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        c_msg.set_tr_module(pms::ETransferModule::MSEQUENCE);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self Logical id to other
        c_msg.set_moduleid(LRTConnManager::Instance().LogicalId());

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
                LRTConnManager::ModuleInfo* pmi = new LRTConnManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = c_msg.tr_module();
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    //bind session and transfer id
                    LRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                    LRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MSEQUENCE);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self Logical id to other
            c_msg.set_moduleid(LRTConnManager::Instance().LogicalId());

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
            LRTConnManager::ModuleInfo* pmi = new LRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                LRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store module type:%d, moduleid:%s, transfersessid:%s\n", (int)c_msg.tr_module(), c_msg.moduleid().c_str(), m_transferSessId.c_str());
                LRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
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

void LRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeWriteRequest(const std::string& str)
{
    pms::RelayMsg rmsg;
    rmsg.ParseFromString(str);

    if (rmsg.svr_cmds()==pms::EServerCmd::CNEWMSG)
    {
        LRTLogicalManager::Instance().RecvRequestCounter();
        pms::PackedStoreMsg store;
        store.ParseFromString(rmsg.content());
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%S store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }
            printf("LRTTransferSession::OnTypeWriteRequest NEWMSG msgid:%s, mtag:%d\n", store.msgs(i).msgid().c_str(), store.msgs(i).mtag());
            if (store.msgs(i).msgid().length()==0)
            {
                char msgid[16] = {0};
                sprintf(msgid, "wm:%u", m_tmpWMsgId++);
                store.mutable_msgs(i)->set_msgid(msgid);
            }
            assert(store.mutable_msgs(i)->msgid().length()!=0);
            LRTLogicalManager::Instance().InsertDataWrite(this, store.mutable_msgs(i));
        }
        pms::TransferMsg tmsg;
        tmsg.set_type(pms::ETransferType::TWRITE_REQUEST);
        tmsg.set_flag(pms::ETransferFlag::FNOACK);
        tmsg.set_priority(pms::ETransferPriority::PNORMAL);
        tmsg.set_content(store.SerializeAsString());
        LRTConnManager::Instance().PushSeqnWriteMsg(tmsg.SerializeAsString());
    } else {
        LE("OnTypeWriteRequest not handle svr_cmd:%d\n", rmsg.svr_cmds());
    }
}

void LRTTransferSession::OnTypeWriteResponse(const std::string& str)
{

    pms::RelayMsg rmsg;
    rmsg.ParseFromString(str);

    if (rmsg.svr_cmds()==pms::EServerCmd::CNEWMSGSEQN)
    {
        pms::PackedStoreMsg store;
        store.ParseFromString(rmsg.content());
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%s newmsgseqn  store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }
            printf("LRTTransferSession::OnTypeWriteResponse NEWMSGSEQN 1 userid:%s, msgid:%s, sequence:%lld, mtag:%d, cont:%s\n\n"\
                    , store.msgs(i).userid().c_str(), store.msgs(i).msgid().c_str()\
                    , store.msgs(i).sequence(), store.msgs(i).mtag(), store.msgs(i).content().c_str());

            pms::StorageMsg* pstore = store.mutable_msgs(i);
            LRTLogicalManager::Instance().UpdateDataWrite(&pstore);

            printf("LRTTransferSession::OnTypeWriteResponse NEWMSGSEQN 111 userid:%s, msgid:%s, sequence:%lld, mtag:%d, cont:%s\n\n"\
                    , store.msgs(i).userid().c_str(), store.msgs(i).msgid().c_str()\
                    , store.msgs(i).sequence(), store.msgs(i).mtag(), store.msgs(i).content().c_str());
        }

        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%s store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }
            printf("LRTTransferSession::OnTypeWriteResponse NEWMSGSEQN 2 userid:%s, msgid:%s, sequence:%lld, mtag:%d, cont:%s\n\n"\
                    , store.msgs(i).userid().c_str(), store.msgs(i).msgid().c_str()\
                    , store.msgs(i).sequence(), store.msgs(i).mtag(), store.msgs(i).content().c_str());

            printf("LRTTransferSession::OnTypeWriteResponse NEWMSGSEQN 222 userid:%s, msgid:%s, sequence:%lld, mtag:%d, cont:%s\n\n"\
                    , store.mutable_msgs(i)->userid().c_str(), store.mutable_msgs(i)->msgid().c_str()\
                    , store.mutable_msgs(i)->sequence(), store.mutable_msgs(i)->mtag(), store.mutable_msgs(i)->content().c_str());
        }
        pms::TransferMsg tmsg;
        tmsg.set_type(pms::ETransferType::TWRITE_REQUEST);
        tmsg.set_flag(pms::ETransferFlag::FNOACK);
        tmsg.set_priority(pms::ETransferPriority::PNORMAL);
        tmsg.set_content(store.SerializeAsString());
        LRTConnManager::Instance().PushStoreWriteMsg(tmsg.SerializeAsString());
    } else if (rmsg.svr_cmds()==pms::EServerCmd::CNEWMSGDATA)
    {
        pms::PackedStoreMsg store;
        store.ParseFromString(rmsg.content());
        LI("%s was called, store.msgs_size:%d\n", __FUNCTION__, store.msgs_size());
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%s newmsgdata store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }
            printf("LRTTransferSession::OnTypeWriteResponse NEWMSGDATA  msgid:%s, sequence:%lld, result:%d, mtag:%d\n\n", store.msgs(i).msgid().c_str(), store.msgs(i).sequence(), store.msgs(i).result(), store.msgs(i).mtag());

            // after msg was generated, update server local seqn for user;
            // this sequene is from sequence()
            LRTLogicalManager::Instance().UpdateLocalSeqn(store.mutable_msgs(i));
            LRTLogicalManager::Instance().DeleteDataWrite(store.mutable_msgs(i));
        }
    } else {
        LE("OnTypeWriteResponse not handle svr_cmd:%d\n", rmsg.svr_cmds());
    }
}

void LRTTransferSession::OnTypeReadRequest(const std::string& str)
{
    pms::RelayMsg rmsg;
    rmsg.ParseFromString(str);

    if (rmsg.svr_cmds()==pms::EServerCmd::CSYNCSEQN)
    {
        pms::PackedStoreMsg store, r_store;
        store.ParseFromString(rmsg.content());
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%s csyncseqn store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }

            // first get seqn from local server
            // if not find seqn in local server,
            // then send request to sequence server
            long long seqn = -1;
            if (LRTLogicalManager::Instance().ReadLocalSeqn(store.mutable_msgs(i), &seqn))
            {
                // update max seqn
                printf("LRTTransferSession::OnTypeReadRequest get maxseqn:%lld from server local\n", seqn);
                assert(seqn>0);
                store.mutable_msgs(i)->set_maxseqn(seqn);
                this->PushReadMsg(store.msgs(i));
            } else {
                char msgid[16] = {0};
                sprintf(msgid, "rs:%u", m_tmpRSeqnId++);
                store.mutable_msgs(i)->set_msgid(msgid);
                printf("LRTTransferSession::OnTypeReadRequest SYNCSEQN msgid:%s, mtag:%d\n", store.msgs(i).msgid().c_str(), store.msgs(i).mtag());
                LRTLogicalManager::Instance().InsertSeqnRead(this, store.mutable_msgs(i));
                r_store.add_msgs()->MergeFrom(store.msgs(i));
            }
        }
        printf("LRTTransferSession::OnTypeReadRequest SYNCSEQN r_store.size:%d\n", r_store.msgs_size());
        if (r_store.msgs_size()>0)
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(r_store.SerializeAsString());
            LRTConnManager::Instance().PushSeqnReadMsg(tmsg.SerializeAsString());
        }
    } else if (rmsg.svr_cmds()==pms::EServerCmd::CSYNCDATA) {
        pms::PackedStoreMsg store;
        store.ParseFromString(rmsg.content());
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%s csyncdata  store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }
            char msgid[16] = {0};
            sprintf(msgid, "rd:%u", m_tmpRDataId++);
            store.mutable_msgs(i)->set_msgid(msgid);
            printf("LRTTransferSession::OnTypeReadRequest SYNCDATA msgid:%s, mtag:%d, seqn:%lld\n"\
                    , store.msgs(i).msgid().c_str()\
                    , store.msgs(i).mtag()\
                    , store.msgs(i).sequence());
            LRTLogicalManager::Instance().InsertDataRead(this, store.mutable_msgs(i));
        }
        pms::TransferMsg tmsg;
        tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
        tmsg.set_flag(pms::ETransferFlag::FNOACK);
        tmsg.set_priority(pms::ETransferPriority::PNORMAL);
        tmsg.set_content(store.SerializeAsString());
        LRTConnManager::Instance().PushStoreReadMsg(tmsg.SerializeAsString());
    } else {
         LE("OnTypeReadRequest not handle svr_cmd:%d\n", rmsg.svr_cmds());
    }
}

// for sequence, all read request, update maxseqn
// all write request, udpate sequence
void LRTTransferSession::OnTypeReadResponse(const std::string& str)
{
    pms::RelayMsg rmsg;
    rmsg.ParseFromString(str);

    if (rmsg.svr_cmds()==pms::EServerCmd::CSYNCSEQN)
    {
        pms::PackedStoreMsg store;
        store.ParseFromString(rmsg.content());
        LI("%s was called, store.msgs_size:%d\n", __FUNCTION__, store.msgs_size());
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%s syncseqn store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }
            printf("LRTTransferSession::OnTypeReadResponse SYNCSEQN msgid:%s, sequence:%lld, maxseqn:%lld, result:%d, mtag:%d\n\n"\
                    , store.msgs(i).msgid().c_str()\
                    , store.msgs(i).sequence()\
                    , store.msgs(i).maxseqn()\
                    , store.msgs(i).result()\
                    , store.msgs(i).mtag());

            // after get seqn from remote server, update server local seqn for user;
            // this sequence is from maxseqn
            LRTLogicalManager::Instance().UpdateLocalMaxSeqn(store.mutable_msgs(i));
            LRTLogicalManager::Instance().DeleteSeqnRead(store.mutable_msgs(i));
        }
    } else if (rmsg.svr_cmds()==pms::EServerCmd::CSYNCDATA)
    {
        pms::PackedStoreMsg store;
        store.ParseFromString(rmsg.content());
        LI("%s was called, store.msgs_size:%d\n", __FUNCTION__, store.msgs_size());
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).userid().length()==0)
            {
                LI("%s syncdata store.msgs(%d).userid length is 0\n", __FUNCTION__, i);
                break;
            }
            printf("LRTTransferSession::OnTypeReadResponse SYNCDATA  msgid:%s, sequence:%lld, result:%d, mtag:%d, cont.len:%d, cont:%s\n\n"\
                    , store.msgs(i).msgid().c_str()\
                    , store.msgs(i).sequence()\
                    , store.msgs(i).result()\
                    , store.msgs(i).mtag()\
                    , store.msgs(i).content().length()\
                    , store.msgs(i).content().c_str());

            LRTLogicalManager::Instance().DeleteDataRead(store.mutable_msgs(i));
        }
    } else {
        LE("OnTypeReadResponse not handle svr_cmd:%d\n", rmsg.svr_cmds());
    }
}


void LRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        LRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

