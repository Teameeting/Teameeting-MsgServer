//
//  XMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XMsgClient.h"
#include "webrtc/base/logging.h"

#ifdef WEBRTC_ANDROID
#include <android/log.h>
#define  LOG_TAG    "XMsgClient"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#include <iostream>
#include <string>
#endif

#define TIMEOUT_TS (60*1000)

XMsgClient::XMsgClient()
: m_pClientImpl(NULL)
, m_pMsgProcesser(NULL)
, m_lastUpdateTime(0)
, m_uid("")
, m_token("")
, m_server("")
, m_port(0)
, m_autoConnect(true)
, m_msTcpState(MSNOT_CONNECTED)
{
    
}

XMsgClient::~XMsgClient()
{
    
}

int XMsgClient::Init(XMsgCallback& cb, const std::string& uid, const std::string& token, const std::string& server, int port, bool bAutoConnect)
{
    if (!m_pMsgProcesser) {
        m_pMsgProcesser = new XMsgProcesser(cb, *this);
    }
    if (!m_pMsgProcesser) {
        return -1;
    }
    
    if (!m_pClientImpl) {
        m_pClientImpl = new XTcpClientImpl(*this);// XTcpClient::Create(*this);
    }
    if (!m_pClientImpl) {
        if (m_pMsgProcesser) {
            delete m_pMsgProcesser;
            m_pMsgProcesser = NULL;
        }
        return -1;
    }
    m_uid = uid;
    m_token = token;
    m_server = server;
    m_port = port;
    m_autoConnect = bAutoConnect;
    m_pClientImpl->Connect(server, port, bAutoConnect);
    m_msTcpState = MSCONNECTTING;
    m_pMsgProcesser->ServerState(MSCONNECTTING);
    
    return 0;
}

int XMsgClient::Unin()
{
    if (m_pClientImpl) {
        m_pClientImpl->Disconnect();
        if (m_pMsgProcesser) {
            delete m_pMsgProcesser;
            m_pMsgProcesser = NULL;
        }
        delete m_pClientImpl;
        m_pClientImpl = NULL;
    }
    
    return 0;
}

int XMsgClient::SndMsg(const std::string& roomid, const std::string& msg)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, roomid, "a", msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::sendtags_talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::GetMsg(GETCMD cmd)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeGetMsg(outstr, m_uid, m_token, cmd);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::OptRoom(MEETCMD cmd, const std::string& roomid, const std::string& remain)
{
    if (cmd<=0 || cmd>=MEETCMD::meetcmd_invalid) {
        return -1;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, roomid, "", remain, cmd, DCOMMACTION::dcommaction_invalid, SENDTAGS::sendtags_invalid, SENDTYPE::sendtype_invalid);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsgTo(const std::string& roomid, const std::string& msg, const std::list<std::string>& ulist)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        std::string tousers;
        m_pMsgProcesser->GetMemberToJson(ulist, tousers);
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, roomid, tousers, msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::sendtags_talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::NotifyMsg(const std::string& roomid, SENDTAGS tags, const std::string& msg)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token, roomid, "a", msg, MEETCMD::dcomm, DCOMMACTION::msend, tags, SENDTYPE::msg);
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

int XMsgClient::Login()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogin(outstr, m_uid, m_token);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

#ifdef WEBRTC_ANDROID
    LOGI("XMsgClient::Login call SendEncodeMsg\n");
#else
    std::cout << "XMsgClient::Login call SendEncodeMsg" << std::endl;
#endif
    return SendEncodeMsg(outstr);
}

int XMsgClient::Logout()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogout(outstr, m_uid, m_token);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    
    return SendEncodeMsg(outstr);
}

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
        //outstr, userid
        m_pMsgProcesser->EncodeKeepAlive(outstr, m_uid);
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
    char* ptr = new char[msg.length()+6];//sprintf will add 1 in the end
    if (ptr) {
        sprintf(ptr, "$%4d%s", (int)msg.length(), msg.c_str());
        if (TcpState::NOT_CONNECTED==m_pClientImpl->Status()) {
            m_pClientImpl->Connect(m_server, m_port, m_autoConnect);
            m_msTcpState = MSCONNECTTING;
            m_pMsgProcesser->ServerState(MSCONNECTTING);
        }
        if (m_pClientImpl) {
            m_pClientImpl->SendMessageX(ptr, (int)strlen(ptr));
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
//////////////XTcpClientCallback//////////////////
//////////////////////////////////////////////////

void XMsgClient::OnServerConnected()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    Login();
    KeepAlive();
}

void XMsgClient::OnServerDisconnect()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    if (m_pMsgProcesser) {
        m_msTcpState = MSNOT_CONNECTED;
        m_pMsgProcesser->ServerState(MSNOT_CONNECTED);
        m_pMsgProcesser->ServerDisconnect();
    }
}

void XMsgClient::OnServerConnectionFailure()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    if (m_pMsgProcesser) {
        m_msTcpState = MSNOT_CONNECTED;
        m_pMsgProcesser->ServerState(MSNOT_CONNECTED);
        m_pMsgProcesser->ServerConnectionFailure();
    }
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
            char l[5] = {0};
            memset(l, 0x00, 5);
            memcpy(l, pMsg+offset, 4);
            offset += 4;
            ll = (int)strtol(l, NULL, 10);
            if (m_pMsgProcesser && ll>0 && ll <= nLen) {
                int nlen = m_pMsgProcesser->DecodeRecvData((char *)(pMsg+offset), ll);
                if (nlen == -1) {
                    break;
                } else { // nlen == 0
                    offset += ll;
                    parsed += offset;
                }
            } else { // ll>0 && ll <= nLen
#ifdef WEBRTC_ANDROID
                LOGI("XMsgClient::OnMessageRecv Error!!!, ll:%d, nLen:%d, parsed:%d\n", ll, nLen, parsed);
#else
                std::cout << "XMsgClient::OnMessageRecv Error!!!, ll:" << ll << ", nLen:" << nLen << ", parsed:" << parsed << std::endl;
#endif
            }
        }
    }
}

//////////////////////////////////////////////////
//////////////XMsgClientHelper////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnLogin(int code, const std::string& status, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    LOGI("XMsgClient::OnLogin code:%d, userid:%s\n", code, userid.c_str());
#else
    std::cout << "XMsgClient::OnLogin code:" << code << ", userid:" << userid << std::endl;
#endif
    if (code == 0) {
        m_msTcpState = MSCONNECTED;
        m_pMsgProcesser->ServerState(MSCONNECTED);
        m_pMsgProcesser->ServerConnected();
    } else {
        Login();
    }
    KeepAlive();
}

void XMsgClient::OnLogout(int code, const std::string& status, const std::string& userid)
{
    
}
