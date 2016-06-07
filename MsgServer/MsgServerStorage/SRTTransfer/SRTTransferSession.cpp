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
, mRecvRequest(0)
{
    AddObserver(this);
}

SRTTransferSession::~SRTTransferSession()
{
    printf("~SRTTransferSession was called\n");
    Unit();
    DelObserver(this);
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
    m_RedisManager.Init(this);
}

void SRTTransferSession::InitConf()
{
    ListZero(&m_listRequest);
    m_RedisManager.Init(this);
}

void SRTTransferSession::Unit()
{
    Disconn();
    m_RedisManager.Unin();
    ListEmpty(&m_listRequest);
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
    this->RTTcpNoTimeout::UpdateTimer();
}

void SRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    SRTStorageManager::Instance().SendResponseCounter();
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void SRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

// from RTTcpNoTimeout
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
    m_RedisManager.PushRedisRequest(message, nLen);
}

void SRTTransferSession::ConnectionDisconnected()
{
    printf("!!!!!!!!SRTTransferSession::ConnectionDisconnected!!!!!!!!!!\n");
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
