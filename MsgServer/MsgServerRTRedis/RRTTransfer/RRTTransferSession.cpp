//
//  RRTTransferSession.cpp
//  dyncRTRTRedis
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RRTTransferSession.h"
#include "RTMessage.h"
#include "RRTRTRedisManager.h"
#include "RTUtils.hpp"

RRTTransferSession::RRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, m_transferSessId("")
, m_processingMsg("")
{
    AddObserver(this);
}

RRTTransferSession::~RRTTransferSession()
{
    DelObserver(this);
    Unit();
}

void RRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(96L * 1024L);

    socket->SetTask(this);
    this->SetTimer(120*1000);
}

void RRTTransferSession::Unit()
{
    Disconn();
}

bool RRTTransferSession::Connect(const std::string addr, int port)
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

void RRTTransferSession::Disconn()
{

}

void RRTTransferSession::TestConnection()
{

}

void RRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void RRTTransferSession::TransferMsg(const std::string& msg)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

// from RTTcp
void RRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void RRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    // write message into worker list
    m_processingMsg.assign(message, nLen);
    RRTRTRedisManager::Instance().PushBack(this);
}

// from RTTransfer

void RRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void RRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
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

void RRTTransferSession::OnTypeConn(const std::string& str)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

void RRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

void RRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

void RRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

void RRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

void RRTTransferSession::OnTypeTLogin(const std::string& str)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

void RRTTransferSession::OnTypeTLogout(const std::string& str)
{
    LI("%s was called, but not implement\n", __FUNCTION__);
}

void RRTTransferSession::ConnectionDisconnected()
{

}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
