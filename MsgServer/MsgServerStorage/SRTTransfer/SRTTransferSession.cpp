//
//  SRTTransferSession.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTTransferSession.h"
#include "RTMessage.h"
#include "SRTStorageManager.h"
#include "RTUtils.hpp"
#include "assert.h"

SRTTransferSession::SRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, mRecvRequest(0)
{
    AddObserver(this);
}

SRTTransferSession::~SRTTransferSession()
{
    printf("~SRTTransferSession was called\n");
    DelObserver(this);
    Unit();
}

void SRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(96L * 1024L);

    socket->SetTask(this);
    this->SetTimer(120*1000);

    ListZero(&m_listRequest);
    m_StorageGenerator.Init(this);
}

void SRTTransferSession::InitConf()
{
    ListZero(&m_listRequest);
    m_StorageGenerator.Init(this);
}

void SRTTransferSession::Unit()
{
    m_StorageGenerator.Unin();
    ListEmpty(&m_listRequest);
    Disconn();
}

bool SRTTransferSession::Connect(const std::string addr, int port)
{
    if (addr.empty() || port < 2048) {
        LE("invalid params addr:%s, port:%d\n", addr.c_str(), port);
        return false;
    }
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(addr.c_str()), port);
    if (err == OS_NoErr || err == EISCONN) {
        return true;
    } else {
        LE("Socket Connect  ERR:%d\n", err);
        return false;
    }
}

void SRTTransferSession::Disconn()
{

}

void SRTTransferSession::RefreshTime()
{
    this->RTTcp::UpdateTimer();
    LI("SRTTransferSession::RefreshTime.....\n");
}

void SRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    SRTStorageManager::Instance().SendResponseCounter();
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void SRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

// from RTTcp
void SRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void SRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    SRTStorageManager::Instance().RecvRequestCounter();
    //SInt64 curTime = OS::Milliseconds();
    //LI("SRTTransferSession::OnRecvMessage recv request:%lld, time:%lld\n", mRecvRequest++, curTime);
    m_StorageGenerator.WakeupData(message, nLen);
}

// from RTTransfer

void SRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void SRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
#if DEF_PROTO
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    const std::string s = ack_msg.SerializeAsString();
    OnTransfer(s);
#else
    LE("not define DEF_PROTO\n");
#endif
}

void SRTTransferSession::OnTypeConn(const std::string& str)
{
    //LI("%s was called, but not implement\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeTrans(const std::string& str)
{
    //LI("%s was called, but not implement\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeQueue(const std::string& str)
{
    //LI("%s was called, but not implement\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeDispatch(const std::string& str)
{
    //LI("%s was called, but not implement\n", __FUNCTION__);
}

void SRTTransferSession::OnTypePush(const std::string& str)
{
    //LI("%s was called, but not implement\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeTLogin(const std::string& str)
{
    //LI("%s was called, but not implement\n", __FUNCTION__);
}

void SRTTransferSession::OnTypeTLogout(const std::string& str)
{
    //LI("%s was called, but not implement\n", __FUNCTION__);
}

void SRTTransferSession::ConnectionDisconnected()
{

}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
