//
//  XGrpMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XGrpMsgClient.h"
#include "webrtc/base/logging.h"

#ifdef WEBRTC_ANDROID
#include <android/log.h>
#define  LOG_TAG    "XGrpMsgClient"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#include <iostream>
#include <string>
#endif

#define TIMEOUT_TS (60*1000)

XGrpMsgClient::XGrpMsgClient()
: XJSBuffer()
, m_pClientImpl(NULL)
, m_pGrpMsgProcesser(NULL)
, m_lastUpdateTime(0)
, m_uid("")
, m_token("")
, m_server("180.150.179.128")
, m_port(6630)
, m_autoConnect(true)
, m_login(false)
, m_msState(MSNOT_CONNECTED)
, m_curSeqn(250)
{
    printf("XGrpMsgClient XGrpMsgClient ok!!\n");
}

XGrpMsgClient::~XGrpMsgClient()
{

}

int XGrpMsgClient::Init(XMsgCallback* cb, const std::string& uid, const std::string& token, int module, const std::string& server, int port, bool bAutoConnect)
{
    if (!cb) {
        return -1;
    }
    if (!m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser = new XGrpMsgProcesser(*this);
        m_pGrpMsgProcesser->Init();
        m_pCallback = cb;
    }
    if (!m_pGrpMsgProcesser) {
        return -1;
    }

    if (!m_pClientImpl) {
        m_pClientImpl = new XTcpClientImpl(*this);
    }
    if (!m_pClientImpl) {
        if (m_pGrpMsgProcesser) {
            delete m_pGrpMsgProcesser;
            m_pGrpMsgProcesser = NULL;
        }
        return -1;
    }
    m_uid = uid;
    m_token = token;
    m_module = (pms::EModuleType)module;
    if (server.length()>0) {
        m_server = server;
    }
    if (port>2048) {
        m_port = port;
    }
    m_autoConnect = bAutoConnect;
    m_pClientImpl->Connect(server, port, bAutoConnect);
    m_msState = MSCONNECTTING;
    m_pCallback->OnMsgServerState(MSCONNECTTING);

    printf("XGrpMsgClient Init ok!!\n");
    return 0;
}

int XGrpMsgClient::Unin()
{
    m_pCallback = nullptr;
    if (m_pClientImpl) {
        m_pClientImpl->Disconnect();
        if (m_pGrpMsgProcesser) {
            delete m_pGrpMsgProcesser;
            m_pGrpMsgProcesser = nullptr;
        }
        delete m_pClientImpl;
        m_pClientImpl = nullptr;
    }

    return 0;
}

int XGrpMsgClient::GroupNotify(const std::string& userid, const std::string& groupid)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeGroupNotify(outstr, userid, groupid, m_curSeqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    printf("XGrpMsgClient GroupNotify ok!!\n");
    return SendEncodeMsg(outstr);
}

int XGrpMsgClient::GroupNotifys(const std::vector<std::string>& userids, const std::string& groupid)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeGroupNotifys(outstr, userids, groupid, m_curSeqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    printf("XGrpMsgClient GroupNotifys ok!!\n");
    return SendEncodeMsg(outstr);
}

int XGrpMsgClient::GenSyncDataRequest(const std::string& userid, const std::string& groupid, long long seqn)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeSyncDataRequest(outstr, userid, groupid, seqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    printf("XGrpMsgClient GenSyncDataRequest ok!!\n");
    return SendEncodeMsg(outstr);
}


int XGrpMsgClient::GenSyncDataRequests(const std::vector<std::string>& userids, const std::string& groupid, long long seqn)
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeSyncDataRequests(outstr, userids, groupid, seqn, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    printf("XGrpMsgClient GenSyncDataRequests ok!!\n");
    return SendEncodeMsg(outstr);
}

////////////////////////////////////////////
////////////////private/////////////////////
////////////////////////////////////////////

int XGrpMsgClient::Login()
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeLogin(outstr, m_uid, m_token, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XGrpMsgClient::Logout()
{
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->EncodeLogout(outstr, m_uid, m_token, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

bool XGrpMsgClient::RefreshTime()
{
    uint32_t now = rtc::Time();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        KeepAlive();
        return true;
    } else {
        return false;
    }
}

int XGrpMsgClient::KeepAlive()
{
    //if login or not
    if (!m_login) {
        return -1;
    }
    std::string outstr;
    if (m_pGrpMsgProcesser) {
        //outstr, userid
        m_pGrpMsgProcesser->EncodeKeepAlive(outstr, m_uid, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XGrpMsgClient::SendEncodeMsg(std::string& msg)
{
    if (TcpState::CONNECTED!=m_pClientImpl->Status()) {
#ifdef WEBRTC_ANDROID
        LOGI("XGrpMsgClient::SendEncodeMsg NOT_CONNECTED\n");
#else
        std::cout << "XGrpMsgClient::SendEncodeMsg NOT_CONNECTED" << std::endl;
#endif
        return -1;
    }
    char* ptr = new char[msg.length()+4];//sprintf will add 1 in the end
    if (ptr) {
        char* pptr = ptr;
        *pptr = '$';
        pptr++;
        writeShort(&pptr, (unsigned short)msg.length());
        memcpy((pptr), msg.c_str(), msg.length());
        ptr[msg.length()+3] = '\0';
        if (m_pClientImpl) {
            int n = m_pClientImpl->SendMessageX(ptr, (int)(msg.length()+3));
            delete [] ptr;
            ptr = NULL;
            pptr = NULL;
            return n;
        } else {
#ifdef WEBRTC_ANDROID
            LOGI("XGrpMsgClient::SendEncodeMsg m_pClientImpl is NULL\n");
#else
            std::cout << "XGrpMsgClient::SendEncodeMsg m_pClientImpl is NULL" << std::endl;
#endif
            delete ptr;
            ptr = NULL;
            pptr = NULL;
            LOG(LS_ERROR) << "SendEncodeMsg m_pClientImpl is NULL";
            return -1;
        }
    } else {
#ifdef WEBRTC_ANDROID
        LOGI("XGrpMsgClient::SendEncodeMsg ptr is NULL\n");
#else
        std::cout << "XGrpMsgClient::SendEncodeMsg ptr is NULL" << std::endl;
#endif
        LOG(LS_ERROR) << "SendEncodeMsg ptr is NULL";
        return -1;
    }
}

void XGrpMsgClient::writeShort(char** pptr, unsigned short anInt)
{
    **pptr = (char)(anInt / 256);
    (*pptr)++;
    **pptr = (char)(anInt % 256);
    (*pptr)++;
}

unsigned short XGrpMsgClient::readShort(char** pptr)
{
    char* ptr = *pptr;
    unsigned short len = 256 * ((unsigned char)(*ptr)) + (unsigned char)(*(ptr + 1));
    *pptr += 2;
    return len;
}

//////////////////////////////////////////////////
//////////////XTcpClientCallback//////////////////
//////////////////////////////////////////////////

void XGrpMsgClient::OnServerConnected()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    Login();
}

void XGrpMsgClient::OnServerDisconnect()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    if (m_pCallback) {
        m_login = false;
        m_msState = MSNOT_CONNECTED;
        m_pCallback->OnMsgServerState(MSNOT_CONNECTED);
        m_pCallback->OnMsgServerDisconnect();
    }
}

void XGrpMsgClient::OnServerConnectionFailure()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    if (m_pCallback) {
        m_login = false;
        m_msState = MSNOT_CONNECTED;
        m_pCallback->OnMsgServerState(MSNOT_CONNECTED);
        m_pCallback->OnMsgServerConnectionFailure();
    }
}

void XGrpMsgClient::OnTick()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    if (m_msState == MSCONNECTED) {
        RefreshTime();
    }

}

void XGrpMsgClient::OnMessageSent(int err)
{
    //LOG(INFO) << __FUNCTION__ << " was called err:" << err;
}

void XGrpMsgClient::OnMessageRecv(const char*pData, int nLen)
{
    //LOG(INFO) << __FUNCTION__ << " was called nLen:" << nLen << ",pData:" << pData;
    if (!pData) {
        return;
    }
    XJSBuffer::RecvData(pData, nLen);
}

//////////////////////////////////////////////////
//////////////XGrpMsgClientHelper////////////////////
//////////////////////////////////////////////////

void XGrpMsgClient::OnLogin(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    //LOGI("XGrpMsgClient::OnLogin code:%d\n", code);
#else
    //std::cout << "XGrpMsgClient::OnLogin code:" << code << std::endl;
#endif
    if (code == 0) {
        m_login = true;
        m_msState = MSCONNECTED;
        m_pCallback->OnMsgServerState(MSCONNECTED);
        m_pCallback->OnMsgServerConnected();
    } else {
        Login();
    }
}

void XGrpMsgClient::OnLogout(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    //LOGI("XGrpMsgClient::OnLogout code:%d\n", code);
#else
    //std::cout << "XGrpMsgClient::OnLogout code:" << code << std::endl;
#endif
    m_login = false;
}

void XGrpMsgClient::OnKeepLive(int code, const std::string& cont)
{

}

void XGrpMsgClient::OnGroupNotify(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XGrpMsgClient::OnGroupNotify userid:%s, sequence:%lld, groupid:%s, mflag:%d, svrcmd:%d\n\n"\
            , store.userid().c_str()\
            , store.sequence()\
            , store.groupid().c_str()\
            , store.mflag()\
            , store.svrcmd());
    // check userid, groupid, send sync data request
    std::string mem_user("xddxdd");
    GenSyncDataRequest(mem_user, store.groupid(), store.sequence());

    return;
}


//////////////////////////////////////////////////
//////////////XJSBuffer///////////////////////////
//////////////////////////////////////////////////

void XGrpMsgClient::OnRecvMessage(const char*message, int nLen)
{
    if (m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser->DecodeRecvData((char *)message, nLen);
    }
}
