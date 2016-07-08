//
//  XGrpMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XGrpMsgClient.h"
#include "webrtc/base/logging.h"

#include <iostream>
#include <string>

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
, m_curSeqn(0)
{
    printf("XGrpMsgClient XGrpMsgClient ok!!\n");
}

XGrpMsgClient::~XGrpMsgClient()
{

}

int XGrpMsgClient::Init(const std::string& uid, const std::string& token, int module)
{
    if (!m_pGrpMsgProcesser) {
        m_pGrpMsgProcesser = new XGrpMsgProcesser(*this);
        m_pGrpMsgProcesser->Init();
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

    printf("XGrpMsgClient Init ok!!\n");
    return 0;
}

int XGrpMsgClient::Unin()
{
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

int XGrpMsgClient::RegisterMsgCb(XMsgCallback* cb)
{
    if (!cb) return -1;
    m_pCallback = cb;
    return 0;
}

int XGrpMsgClient::UnRegisterMsgCb(XMsgCallback* cb)
{
    if (!cb) return -1;
    m_pCallback = nullptr;
    return 0;
}

int XGrpMsgClient::ConnToServer(const std::string& server, int port, bool bAutoConnect)
{
    if (server.length()>0) {
        m_server = server;
    }
    if (port>2048) {
        m_port = port;
    }
    m_autoConnect = bAutoConnect;
    m_pClientImpl->Connect(server, port, bAutoConnect);
    m_msState = MSCONNECTTING;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerConnecting();
    } else {
        printf("XGrpMsgClient::OnServerDisconnect m_pCallback is null\n");
    }
    return 0;
}

int XGrpMsgClient::CreateGroupSeqn(const std::string& cltUserid, const std::string& groupid)
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

    printf("XGrpMsgClient CreateGroupSeqn ok!!\n");
    return SendEncodeMsg(outstr);
}

int XGrpMsgClient::DeleteGroupSeqn(const std::string& cltUserid, const std::string& groupid)
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

    printf("XGrpMsgClient DeleteGroupSeqn ok!!\n");
    return SendEncodeMsg(outstr);
}

int XGrpMsgClient::GenGrpSyncDataNotify(const std::string& userid, const std::string& groupid, int64 seqn)
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

    printf("XGrpMsgClient GenGrpSyncDataNotify ok!!\n");
    return SendEncodeMsg(outstr);
}


int XGrpMsgClient::GenGrpSyncDataNotifys(const std::vector<std::string>& userids, const std::string& groupid, int64 seqn)
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

    printf("XGrpMsgClient GenGrpSyncDataNotifys ok!!\n");
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
        m_pCallback->OnMsgServerDisconnect();
    }
}

void XGrpMsgClient::OnServerConnectionFailure()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    if (m_pCallback) {
        m_login = false;
        m_msState = MSNOT_CONNECTED;
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
    printf("XGrpMsgClient::OnGroupNotify ruserid:%s, sequence:%lld, groupid:%s, mflag:%d, rsvrcmd:%d\n\n"\
            , store.ruserid().c_str()\
            , store.sequence()\
            , store.groupid().c_str()\
            , store.mflag()\
            , store.rsvrcmd());
    // check userid, groupid, send sync data request
    std::string mem_user1("BCD9D958-985A-4454-B2C8-1551DB9C1A8A");
    GenGrpSyncDataNotify(mem_user1, store.groupid(), store.sequence());
    std::string mem_user2("2C665ED7-3854-4411-9536-947A4340B86E");
    GenGrpSyncDataNotify(mem_user2, store.groupid(), store.sequence());
    std::string mem_user3("9297E785-59BD-483A-ABEA-5C9F8D88FEB0");
    GenGrpSyncDataNotify(mem_user3, store.groupid(), store.sequence());

    return;
}

void XGrpMsgClient::OnCreateGroupSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XGrpMsgClient::OnCreateGroupSeqn =====>code:%d, cont.length:%lu, store.result:%d\n\n"\
            , code, cont.length(), store.result());
    return;
}

void XGrpMsgClient::OnDeleteGroupSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XGrpMsgClient::OnDeleteGroupSeqn =====>code:%d, cont.length:%lu, store.result:%d\n\n"\
            , code, cont.length(), store.result());
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
