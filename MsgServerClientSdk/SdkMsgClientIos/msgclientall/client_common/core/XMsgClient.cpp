//
//  XMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "core/XMsgClient.h"
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
: XJSBuffer()
, m_pCallback(nullptr)
, m_pClientImpl(nullptr)
, m_pMsgProcesser(nullptr)
, m_lastUpdateTime(0)
, m_uid("")
, m_grpid("")
, m_token("")
, m_nname("")
, m_server("180.150.179.128")
, m_port(6630)
, m_autoConnect(true)
, m_login(false)
, m_msState(MSNOT_CONNECTED)
{
    printf("XMsgClient XMsgClient ok!!\n");
}

XMsgClient::~XMsgClient()
{

}

int XMsgClient::Init(const std::string& uid, const std::string& token, const std::string& nname, int module)
{
    if (!m_pMsgProcesser) {
        m_pMsgProcesser = new XMsgProcesser(*this);
    }
    if (!m_pMsgProcesser) {
        return -1;
    }

    if (!m_pClientImpl) {
        m_pClientImpl = new XTcpClientImpl(*this);
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
    m_nname = nname;
    m_module = (pms::EModuleType)module;

    printf("XMsgClient Init ok!!\n");
    return 0;
}

int XMsgClient::Unin()
{
    if (m_pClientImpl) {
        m_pClientImpl->Disconnect();
        if (m_pMsgProcesser) {
            delete m_pMsgProcesser;
            m_pMsgProcesser = nullptr;
        }
        delete m_pClientImpl;
        m_pClientImpl = nullptr;
    }

    return 0;
}

int XMsgClient::RegisterMsgCb(XMsgCallback* cb)
{
    if (!cb) return -1;
    m_pCallback = cb;
    return 0;
}

int XMsgClient::UnRegisterMsgCb(XMsgCallback* cb)
{
    if (!cb) return -1;
    m_pCallback = nullptr;
    return 0;
}

int XMsgClient::ConnToServer(const std::string& server, int port, bool bAutoConnect)
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
        printf("XMsgClient::OnServerDisconnect m_pCallback is null\n");
    }
    return 0;
}

int XMsgClient::AddGroup(const std::string& groupid)
{
    FetchGroupSeqn(groupid);
    return 0;
}

int XMsgClient::RmvGroup(const std::string& groupid)
{
    if (m_pCallback)
    {
        MSCbData cbData;
        cbData.type = 1;
        cbData.data = "del ok";
        m_pCallback->OnCmdGroup(0, 1, groupid, cbData);
    }
    return 0;
}


int XMsgClient::SndMsg(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag)
{
    if (msg.length()>1024 || grpname.length()>128) {
        return -2;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        std::vector<std::string> uvec;
        m_pMsgProcesser->EncodeSndMsg(outstr, outmsgid, m_uid, m_token\
                , m_nname, groupid, grpname, uvec, msg\
                , tag, type, module, flag);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    printf("XMsgClient SndMsg ok!!\n");
    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsgTo(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, const std::vector<std::string>& uvec)
{
    if (msg.length()>1024 || grpname.length()>128) {
        return -2;
    }
    if (uvec.size()==0) {
        return -3;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
            m_pMsgProcesser->EncodeSndMsg(outstr, outmsgid, m_uid, m_token\
                    , m_nname, groupid, grpname, uvec, msg\
                    , tag, type, module, flag);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncSeqn(int64 seqn, int role)
{
    printf("XMsgClient::SyncSeqn seqn is:%lld, wait...\n", seqn);
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncSeqn(outstr, m_uid, m_token, seqn, m_module, pms::EStorageTag::TSEQN, pms::EMsgFlag::FSINGLE, role);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::SyncSeqn was called\n");

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncData(int64 seqn)
{
    printf("XMsgClient::SyncData seqn is:%lld, wait...\n", seqn);
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncData(outstr, m_uid, m_token, seqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FSINGLE);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::SyncData was called\n");

    return SendEncodeMsg(outstr);
}

int XMsgClient::FetchGroupSeqn(const std::string& groupid)
{
    printf("XMsgClient::FetchGroupSeqn groupid is:%s, wait...\n", groupid.c_str());
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, -1, m_module, pms::EStorageTag::TFETCHSEQN, pms::EMsgFlag::FGROUP, pms::EMsgRole::RSENDER);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::FetchGroupSeqn was called\n");
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupSeqn(const std::string& groupid, int64 seqn, int role)
{
    printf("XMsgClient::SyncGroupSeqn groupid is:%s, seqn :%lld, wait...\n", groupid.c_str(), seqn);
    std::string outstr;
    if (m_pMsgProcesser) {
            m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, seqn, m_module, pms::EStorageTag::TSEQN, pms::EMsgFlag::FGROUP, role);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::SyncGroupSeqn was called\n");
    
    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupData(const std::string& groupid, int64 seqn)
{
    printf("XMsgClient::SyncGroupData userid:%s, groupid:%s, seqn is:%lld, wait...\n"\
           , m_uid.c_str(), groupid.c_str(), seqn);
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncGroupData(outstr, m_uid, m_token, groupid, seqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::SyncGroupData was called\n");
    
    return SendEncodeMsg(outstr);
}


////////////////////////////////////////////
////////////////private/////////////////////
////////////////////////////////////////////

int XMsgClient::Login()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogin(outstr, m_uid, m_token, m_nname, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::Logout()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogout(outstr, m_uid, m_token, m_module);
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
    uint32_t now = rtc::Time();
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
    //if login or not
    if (!m_login) {
        return -1;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid
        m_pMsgProcesser->EncodeKeepAlive(outstr, m_uid, m_module);
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
    if (TcpState::CONNECTED!=m_pClientImpl->Status()) {
#ifdef WEBRTC_ANDROID
        LOGI("XMsgClient::SendEncodeMsg NOT_CONNECTED\n");
#else
        std::cout << "XMsgClient::SendEncodeMsg NOT_CONNECTED" << std::endl;
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
            LOGI("XMsgClient::SendEncodeMsg m_pClientImpl is NULL\n");
#else
            std::cout << "XMsgClient::SendEncodeMsg m_pClientImpl is NULL" << std::endl;
#endif
            delete [] ptr;
            ptr = NULL;
            pptr = NULL;
            LOG(LS_ERROR) << "SendEncodeMsg m_pClientImpl is NULL";
            return -1;
        }
    } else {
#ifdef WEBRTC_ANDROID
        LOGI("XMsgClient::SendEncodeMsg ptr is NULL\n");
#else
        std::cout << "XMsgClient::SendEncodeMsg ptr is NULL" << std::endl;
#endif
        LOG(LS_ERROR) << "SendEncodeMsg ptr is NULL";
        return -1;
    }
}

void XMsgClient::writeShort(char** pptr, unsigned short anInt)
{
    **pptr = (char)(anInt / 256);
    (*pptr)++;
    **pptr = (char)(anInt % 256);
    (*pptr)++;
}

unsigned short XMsgClient::readShort(char** pptr)
{
    char* ptr = *pptr;
    unsigned short len = 256 * ((unsigned char)(*ptr)) + (unsigned char)(*(ptr + 1));
    *pptr += 2;
    return len;
}

//////////////////////////////////////////////////
//////////////XTcpClientCallback//////////////////
//////////////////////////////////////////////////

void XMsgClient::OnServerConnected()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    Login();
}

void XMsgClient::OnServerDisconnect()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerDisconnect();
    } else {
        printf("XMsgClient::OnServerDisconnect m_pCallback is null\n");
    }
}

void XMsgClient::OnServerConnectionFailure()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerConnectionFailure();
    } else {
        printf("XMsgClient::OnServerConnectionFailure m_pCallback is null\n");
    }
}

void XMsgClient::OnTick()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    if (m_msState == MSCONNECTED) {
        RefreshTime();
    }

}

void XMsgClient::OnMessageSent(int err)
{
    //LOG(INFO) << __FUNCTION__ << " was called err:" << err;
}

void XMsgClient::OnMessageRecv(const char*pData, int nLen)
{
    //LOG(INFO) << __FUNCTION__ << " was called nLen:" << nLen << ",pData:" << pData;
    if (!pData) {
        return;
    }
    XJSBuffer::RecvData(pData, nLen);
}

//////////////////////////////////////////////////
//////////////XMsgClientHelper////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnHelpLogin(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    //LOGI("XMsgClient::OnHelpLogin code:%d\n", code);
#else
    //std::cout << "XMsgClient::OnHelpLogin code:" << code << std::endl;
#endif
    if (code == 0) {
        m_login = true;
        m_msState = MSCONNECTED;
        if (m_pCallback)
        {
            m_pCallback->OnMsgServerConnected();
            //printf("after connected to server, call SyncMsg once\n");
        } else {
            printf("XMsgClient::OnHelpLogin m_pCallback is null\n");
        }
    } else {
        Login();
    }
}

void XMsgClient::OnHelpLogout(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    //LOGI("XMsgClient::OnHelpLogout code:%d\n", code);
#else
    //std::cout << "XMsgClient::OnHelpLogout code:" << code << std::endl;
#endif
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerDisconnect();
    } else {
        printf("XMsgClient::OnHelpLogout m_pCallback is null\n");
    }
}

void XMsgClient::OnHelpSndMsg(int code, const std::string& cont)
{
    if (m_pCallback)
    {
        m_pCallback->OnSndMsg(code, cont);
    } else {
        printf("XMsgClient::OnHelpSndMsg m_pCallback is null\n");
    }
    return;
}

void XMsgClient::OnHelpKeepLive(int code, const std::string& cont)
{

}

void XMsgClient::OnHelpSyncSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnHelpSyncSeqn ruserid:%s, sequence:%lld, maxseqn:%lld\n"\
            , store.ruserid().c_str(), store.sequence(), store.maxseqn());
    printf("XMsgClient::OnHelpSyncSeqn should be equal here???\n");
    //assert(store.maxseqn()>=m_curSeqn);
    switch (store.mtag())
    {
        case pms::EStorageTag::TSEQN:
        {
            if (store.mflag()==pms::EMsgFlag::FGROUP)
            {
                if (m_pCallback)
                {
                    m_pCallback->OnSyncGroupSeqn(store.storeid(), store.maxseqn());
                } else {
                    printf("XMsgClient::OnHelpSyncGroupSeqn m_pCallback is null\n");
                }
            } else if (store.mflag()==pms::EMsgFlag::FSINGLE)
            {
                if (m_pCallback)
                {
                    m_pCallback->OnSyncSeqn(store.maxseqn(), store.mrole());
                } else {
                    printf("XMsgClient::OnHelpSyncSeqn m_pCallback is null\n");
                }
            } else {
                
            }
        }
            break;
            
        case pms::EStorageTag::TFETCHSEQN:
        {
            if (store.mflag()==pms::EMsgFlag::FGROUP)
            {
                printf("XMsgClient::OnHelpSyncSeqn group storeid:%s, ruserid:%s, seqn:%lld, maxseqn:%lld\n"\
                       , store.storeid().c_str(), store.ruserid().c_str(), store.sequence(), store.maxseqn());
                if (m_pCallback)
                {
                    if (store.maxseqn()>=0)
                    {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "add ok";
                        cbData.seqn = store.maxseqn();
                        m_pCallback->OnCmdGroup(0, 0, store.storeid(), cbData);
                    } else {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "add fail";
                        m_pCallback->OnCmdGroup(-1, 0, store.storeid(), cbData);
                    }
                } else {
                    printf("XMsgClient::OnHelpSyncSeqn GETNEW m_pCallback is null\n");
                }
            }    
        }
            break;
        default:
            printf("XMsgClient::OnHelpSyncSeqn mtag :%d not handle\n", store.mtag());
            break;
    }
    return;
}

void XMsgClient::OnHelpSyncData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnHelpSyncData ruserid:%s, storeid:%s, sequence:%lld, maxseqn:%lld\n\n"\
            , store.ruserid().c_str()\
            , store.storeid().c_str()\
            , store.sequence()\
            , store.maxseqn());
    printf("XMsgClient::OnHelpSyncData should be equal here???\n");
    //assert(store.maxseqn()>=m_curSeqn);// ???????

    pms::Entity entity;
    entity.ParseFromString(store.content());
    if (entity.usr_toto().users_size()==0)
    {
        printf("OnHelpSyncData usr_toto.size is 0 so return\n");
        return;
    }
    printf("OnHelpSyncData entity.usr_from:%s, usr_toto:%s, msg_cont:%s\n"\
            , entity.usr_from().c_str()\
            , entity.usr_toto().users(0).c_str()\
            , entity.msg_cont().c_str());
    if (m_pCallback)
    {
        m_pCallback->OnRecvMsg(store.sequence(), store.content());
    } else {
        printf("XMsgClient::OnHelpSyncData m_pCallback is null\n");
    }
    return;
}

void XMsgClient::OnHelpSyncGroupData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnHelpSyncGroupData ruserid:%s, groupid:%s, storeid:%s, rsvrcmd:%d, mflag:%d, sequence:%lld, maxseqn:%lld\n\n"\
            , store.ruserid().c_str()\
            , store.groupid().c_str()\
            , store.storeid().c_str()\
            , store.rsvrcmd()\
            , store.mflag()\
            , store.sequence()\
            , store.maxseqn());

    pms::Entity entity;
    entity.ParseFromString(store.content());
    printf("OnHelpSyncGroupData entity.usr_from:%s, rom_id:%s, msg_cont:%s\n"\
            , entity.usr_from().c_str()\
            , entity.rom_id().c_str()\
            , entity.msg_cont().c_str());
    if (m_pCallback)
    {
        assert(store.storeid().compare(store.groupid())==0);
        m_pCallback->OnRecvGroupMsg(store.sequence(), store.storeid(), store.content());
    } else {
        printf("XMsgClient::OnHelpSyncGroupData m_pCallback is null\n");
    }
    return;
}

void XMsgClient::OnHelpGroupNotify(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnHelpGroupNotify ruserid:%s, groupid:%s, rsvrcmd:%d, mflag:%d, sequence:%lld\n\n"\
           , store.ruserid().c_str()\
           , store.groupid().c_str()\
           , store.rsvrcmd()\
           , store.mflag()\
           , store.sequence());
    if (m_pCallback)
    {
        m_pCallback->OnGroupNotify(code, store.groupid());
    } else {
        printf("XMsgClient::OnHelpGroupNotify m_pCallback is null\n");
    }
    return;
}

// this means you are sender, you send msg and server gen new seqn for you
// you just get new seqn from server
// after 
void XMsgClient::OnHelpNotifySeqn(int code, const std::string& cont)
{
    printf("XMsgClient::OnHelpNotifySeqn code:%d, cont:%s\n", code, cont.c_str());
    if (m_pCallback)
    {
        m_pCallback->OnNotifySeqn(code, cont);
    } else {
        printf("XMsgClient::OnHelpNotifySeqn m_pCallback is null\n");
    }
    printf("XMsgClient::OnHelpNotifySeqn recv sync seqn\n");
    return;
}

void XMsgClient::OnHelpNotifyData(int code, const std::string& cont)
{
    printf("XMsgClient::OnHelpNotifyData code:%d, cont:%s\n", code, cont.c_str());
    if (m_pCallback)
    {
        m_pCallback->OnNotifyData(code, cont);
    } else {
        printf("XMsgClient::OnHelpNotifyData m_pCallback is null\n");
    }
    printf("XMsgClient::OnHelpNotifyData recv sync data\n");
    return;
}


//////////////////////////////////////////////////
//////////////XJSBuffer///////////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnRecvMessage(const char*message, int nLen)
{
    if (m_pMsgProcesser) {
        m_pMsgProcesser->DecodeRecvData((char *)message, nLen);
    }
}
