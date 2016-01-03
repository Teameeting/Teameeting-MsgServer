//
//  XMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XMsgClient.h"
#include "webrtc/base/logging.h"

#define TIMEOUT_TS (60*1000)

XMsgClient::XMsgClient()
: m_pClient(NULL)
, m_pMsgProcesser(NULL)
{
    
}

XMsgClient::~XMsgClient()
{
    
}

int XMsgClient::Init(XMsgCallback& cb, const std::string& server, int port, bool bAutoConnect)
{
    if (!m_pClient) {
        m_pClient = XTcpClient::Create(*this);
    }
    if (!m_pClient) {
        return -1;
    }
    m_pClient->Connect(server, port, bAutoConnect);
    m_pMsgProcesser = new XMsgProcesser(cb);
    if (!m_pMsgProcesser) {
        return -1;
    }
    return 0;
}

int XMsgClient::Unin()
{
    if (m_pMsgProcesser) {
        delete m_pMsgProcesser;
        m_pMsgProcesser = NULL;
    }
    if (m_pClient) {
        m_pClient->Disconnect();
        delete m_pClient;
        m_pClient = NULL;
    }
    
    return 0;
}

int XMsgClient::Login(const std::string& userid, const std::string& pass)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogin(outstr, userid, pass);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsg(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, pass, roomid, "a", msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::GetMsg(const std::string& userid, const std::string& pass)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeGetMsg(outstr, userid, pass);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::Logout(const std::string& userid, const std::string& pass)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogout(outstr, userid, pass);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::OptRoom(MEETCMD cmd, const std::string& userid, const std::string& pass, const std::string& roomid)
{
    if (cmd<=0 || cmd>=MEETCMD::meetcmd_invalid) {
        return -1;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, pass, roomid, "", "", cmd, 0, 0, 0);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsgTo(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg, const std::list<std::string>& ulist)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        std::string tousers;
        m_pMsgProcesser->GetMemberToJson(ulist, tousers);
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, pass, roomid, tousers, msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

////////////////////////////////////////////
////////////////private/////////////////////
////////////////////////////////////////////

bool XMsgClient::RefreshTime()
{
    uint32 now = rtc::Time();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        KeepAlive();
        return true;
    } else {
        return false;
    }
}

int XMsgClient::KeepAlive()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeKeepAlive(outstr);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SendEncodeMsg(std::string& msg)
{
    char* ptr = new char[msg.length()+5];
    if (ptr) {
        sprintf(ptr, "$%4d%s", (int)msg.length(), msg.c_str());
        if (m_pClient && TcpState::CONNECTED==m_pClient->Status()) {
            m_pClient->SendMessageX(ptr, (int)strlen(ptr));
            delete ptr;
            ptr = NULL;
            return 0;
        } else {
            delete ptr;
            ptr = NULL;
            LOG(LS_ERROR) << "SendEncodeMsg m_pClient is NULL";
            return -1;
        }
    } else {
        LOG(LS_ERROR) << "SendEncodeMsg ptr is NULL";
        return -1;
    }
}

//////////////////////////////////////////////////
//////////////MsgClientCallback////////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnServerConnected()
{
    LOG(INFO) << __FUNCTION__ << " was called";
}

void XMsgClient::OnServerDisconnect()
{
    LOG(INFO) << __FUNCTION__ << " was called";
}

void XMsgClient::OnServerConnectionFailure()
{
    LOG(INFO) << __FUNCTION__ << " was called";
}

void XMsgClient::OnTick()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    RefreshTime();
}

void XMsgClient::OnMessageSent(int err)
{
    //LOG(INFO) << __FUNCTION__ << " was called err:" << err;
}

void XMsgClient::OnMessageRecv(const char*pData, int nLen)
{
    //LOG(INFO) << __FUNCTION__ << " was called nLen:" << nLen << ",pData:" << pData;
    int parsed = 0;
    int ll = 0;
    while (parsed < nLen)
    {
        const char* pMsg = pData + parsed;
        int offset = 0;
        if (*(pMsg+offset) == '$') {
            offset += 1;
            char l[4] = {0};
            memcpy(l, pMsg+offset, 4);
            offset += 4;
            ll = (int)strtol(l, NULL, 10);
            if (ll>0 && ll <= nLen) {
                int nlen = m_pMsgProcesser->DecodeRecvData((char *)(pMsg+offset), ll);
                if (nlen == -1) {
                    break;
                } else { // nlen == 0
                    assert(nlen == ll);
                    offset += ll;
                    parsed += offset;
                }
            } else { // ll>0 && ll <= nLen
                LOG(LS_ERROR) << "Get Msg Len Error!!!";
            }
        }
    }
}
