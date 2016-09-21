//
//  GRTTransferSession.cpp
//  dyncRTGrouper
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include <unordered_set>
#include "GRTTransferSession.h"
#include "RTUtils.hpp"
#include "GRTConnManager.h"
#include "GRTGrouperManager.h"

#define TIMEOUT_TS (60*1000)

GRTTransferSession::GRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_uid("sddsddsdd")
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
, m_pGrpMsgProcesser(nullptr)
, m_module(pms::EModuleType::TLIVE)
{
    AddObserver(this);
}

GRTTransferSession::~GRTTransferSession()
{
    if (m_pGrpMsgProcesser)
    {
        m_pGrpMsgProcesser->Unin();
        delete m_pGrpMsgProcesser;
        m_pGrpMsgProcesser = nullptr;
    }
    DelObserver(this);
    Unit();
}

void GRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(96L * 1024L);

    socket->SetTask(this);
    this->SetTimer(120*1000);
    if (!m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser = new XGrpMsgProcesser(*this);
    }
    if (!m_pGrpMsgProcesser) {
        LE("GRTTransferSession XGrpMsgProcesser new error");
        assert(false);
    }
    m_pGrpMsgProcesser->Init();

}

void GRTTransferSession::Unit()
{
    Disconn();
    m_connectingStatus = 0;
}

bool GRTTransferSession::Connect(const std::string addr, int port)
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

bool GRTTransferSession::Connect()
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

void GRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool GRTTransferSession::RefreshTime()
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

void GRTTransferSession::KeepAlive()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MGRPNOTIFY);
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

void GRTTransferSession::TestConnection()
{

}

void GRTTransferSession::EstablishConnection()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MGRPNOTIFY);
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

void GRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    GRTGrouperManager::Instance().SendResponseCounter();
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void GRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}


int GRTTransferSession::CreateGroupSeqn(const std::string& cltUserid, const std::string& groupid)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeCreateGroupSeqn(outstr, m_uid, cltUserid, groupid, m_module);
    } else {
        return -2;
    }
    if (outstr.length()==0) {
        return -1;
    }

    LI("GRTTransferSession CreateGroupSeqn ok!!\n");
    SendTransferData(outstr);
    return 0;
}

int GRTTransferSession::DeleteGroupSeqn(const std::string& cltUserid, const std::string& groupid)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeDeleteGroupSeqn(outstr, m_uid, cltUserid, groupid, m_module);
    } else {
        return -2;
    }
    if (outstr.length()==0) {
        return -1;
    }

    LI("GRTTransferSession DeleteGroupSeqn ok!!\n");
    SendTransferData(outstr);
    return 0;
}

int GRTTransferSession::GenGrpSyncSeqnNotify(const std::string& userid, const std::string& groupid, const std::string& mtype, const std::string& ispush, const std::string& version, int64 seqn)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeGrpSyncSeqnNotify(outstr, userid, groupid, mtype, ispush, version, seqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    LI("GRTTransferSession GenGrpSyncSeqnNotify ok!!\n");
    SendTransferData(outstr);
    return 0;
}


int GRTTransferSession::GenGrpSyncSeqnNotifys(const std::vector<std::string>& userids, const std::string& groupid, const std::string& mtype, const std::string& ispush, const std::string& version, int64 seqn)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeGrpSyncSeqnNotifys(outstr, userids, groupid, mtype, ispush, version, seqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    LI("GRTTransferSession GenGrpSyncSeqnNotifys ok!!\n");
    SendTransferData(outstr);
    return 0;
}


int GRTTransferSession::GenGrpSyncDataNotify(const std::string& userid, const std::string& groupid, int64 seqn)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeGrpSyncDataNotify(outstr, userid, groupid, seqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    LI("GRTTransferSession GenGrpSyncDataNotify ok!!\n");
    SendTransferData(outstr);
    return 0;
}


int GRTTransferSession::GenGrpSyncDataNotifys(const std::vector<std::string>& userids, const std::string& groupid, int64 seqn)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeGrpSyncDataNotifys(outstr, userids, groupid, seqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    LI("GRTTransferSession GenGrpSyncDataNotifys ok!!\n");
    SendTransferData(outstr);
    return 0;
}



// from RTTcpNoTimeout
void GRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void GRTTransferSession::OnWakeupEvent(const char*pData, int nLen)
{
    // here return true means still has groupid to process, so wakeup continue
    if (GRTGrouperManager::Instance().ProcessTmpGroupMsg(this))
        this->Signal(kWakeupEvent);
}


void GRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    RTTransfer::DoProcessData(message, nLen);
}


// from RTTransfer

void GRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcpNoTimeout::SendTransferData(str.c_str(), (int)str.length());
}

void GRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
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

void GRTTransferSession::OnTypeConn(const std::string& str)
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
        // send the transfersessionid and GrouperId to other
        pms::TransferMsg t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        c_msg.set_tr_module(pms::ETransferModule::MGRPNOTIFY);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self Grouper id to other
        c_msg.set_moduleid(GRTConnManager::Instance().GrouperId());

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
                GRTConnManager::ModuleInfo* pmi = new GRTConnManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = c_msg.tr_module();
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    //bind session and transfer id
                    GRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                    GRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MGRPNOTIFY);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self Grouper id to other
            c_msg.set_moduleid(GRTConnManager::Instance().GrouperId());

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
            GRTConnManager::ModuleInfo* pmi = new GRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                GRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store module type:%d, moduleid:%s, transfersessid:%s\n", (int)c_msg.tr_module(), c_msg.moduleid().c_str(), m_transferSessId.c_str());
                GRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
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

void GRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called, str:%s\n", __FUNCTION__, str.c_str());
    GRTGrouperManager::Instance().RecvRequestCounter();
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->DecodeRecvData(str.c_str(), str.length());
    }
}

void GRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called, str:%s\n", __FUNCTION__, str.c_str());
}

void GRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void GRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void GRTTransferSession::OnTypeTLogin(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void GRTTransferSession::OnTypeTLogout(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}


/////////////////////////////////////////////////////////

void GRTTransferSession::OnGroupNotify(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont)) return;
    LI("GRTTransferSession::OnGroupNotify ruserid:%s, groupid:%s, mflag:%d, rsvrcmd:%d, push:%s, mtype:%s, store.sequence:%lld, store.version:%s\n\n"\
            , store.ruserid().c_str()\
            , store.groupid().c_str()\
            , store.mflag()\
            , store.rsvrcmd()\
            , store.ispush().c_str()\
            , store.mtype().c_str()\
            , store.sequence()\
            , store.version().c_str());
    if (store.groupid().compare("wocaowocaowocao")==0)
    {
        std::string u1("8ca64d158a505876");
        std::string u2("BCD9D958-985A-4454-B2C8-1551DB9C1A8A");
        GenGrpSyncSeqnNotify(u1, store.groupid(), store.mtype(), store.ispush(), store.version(), store.sequence());
        GenGrpSyncSeqnNotify(u2, store.groupid(), store.mtype(), store.ispush(), store.version(), store.sequence());
        return;
    }
    std::unordered_set<std::string> *puset = NULL;
    GRTGrouperManager::Instance().GetGroupMembersLocal(store.groupid(), &puset);
    if (!puset)
    {
         LE("groupid :%s is not find in group manager, so return\n", store.groupid().c_str());
         if (GRTGrouperManager::Instance().FindTmpGroupId(store.groupid())) {
             GRTGrouperManager::Instance().TmpStoreGroupMsg(store.groupid(), store.sequence(), true);
         } else {
             GRTGrouperManager::Instance().TmpStoreGroupMsg(store.groupid(), store.sequence(), false);
             this->Signal(kWakeupEvent);
         }
         return;
    }
    for(auto & l : *puset)
    {
        //group22222
        LI("GRTTransferSession::OnGroupNotify userid:%s, groupid:%s, seqn:%lld\n", l.c_str(), store.groupid().c_str(), store.sequence());
        GenGrpSyncSeqnNotify(l, store.groupid(), store.mtype(), store.ispush(), store.version(), store.sequence());
    }

    return;
}

void GRTTransferSession::OnCreateGroupSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont)) return;
    LI("GRTTransferSession::OnCreateGroupSeqn =====>code:%d, cont.length:%lu, store.result:%d\n\n"\
            , code, cont.length(), store.result());
    return;
}

void GRTTransferSession::OnDeleteGroupSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont)) return;
    LI("GRTTransferSession::OnDeleteGroupSeqn =====>code:%d, cont.length:%lu, store.result:%d\n\n"\
            , code, cont.length(), store.result());
    return;
}


void GRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        GRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

