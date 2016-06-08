//
//  ClientSession.cpp
//  SeqnClient
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "ClientSession.h"
#include "ClientManager.h"
#include "RTUtils.hpp"

ClientSession::ClientSession()
: RTJSBuffer()
, mRecvResponse(0)
{
    AddObserver(this);
}

ClientSession::~ClientSession()
{
    DelObserver(this);
    Unit();
}

void ClientSession::Init()
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

void ClientSession::Unit()
{
    Disconn();
}

bool ClientSession::Connect(const std::string addr, int port)
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

void ClientSession::Disconn()
{

}

void ClientSession::SendTransferData(const char* pData, int nLen)
{
    LI("ClientSession::SendTransferData nLen:%d, sendRequest:%lld\n", nLen, mSendRequest++);
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void ClientSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), (int)data.length());
}

void ClientSession::UpdateTime()
{
    this->RTTcp::UpdateTimer();
	LI("Client::UpdateTime...\n");
}

// from RTTcp
void ClientSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void ClientSession::OnRecvMessage(const char*message, int nLen)
{
    // write message into worker list
    //LI("ClientSession::OnRecvMessage nLen:%d, recvResponse:%lld\n", nLen, mRecvResponse++);
    ClientManager::Instance().ProcessRecvMessage(message, nLen);
}


void ClientSession::ConnectionDisconnected()
{

}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
