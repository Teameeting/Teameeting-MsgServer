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
, m_pClientImpl(NULL)
, m_pMsgProcesser(NULL)
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
, m_curSeqn(0)
, m_curGroupSeqn(0)
{
    printf("XMsgClient XMsgClient ok!!\n");
}

XMsgClient::~XMsgClient()
{

}

int XMsgClient::Init(XMsgCallback* cb, const std::string& uid, const std::string& token, const std::string& nname, int module, const std::string& server, int port, bool bAutoConnect)
{
    if (!cb) {
        return -1;
    }
    if (!m_pMsgProcesser) {
        m_pMsgProcesser = new XMsgProcesser(*this);
        //m_pCallback = cb;
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
    if (server.length()>0) {
        m_server = server;
    }
    if (port>2048) {
        m_port = port;
    }
    m_autoConnect = bAutoConnect;
    m_pClientImpl->Connect(server, port, bAutoConnect);
    m_msState = MSCONNECTTING;
    //m_pCallback->OnMsgServerState(MSCONNECTTING);

    printf("XMsgClient Init ok!!\n");
    return 0;
}

int XMsgClient::Unin()
{
    //m_pCallback = nullptr;
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
    {
        rtc::CritScope l(&m_csMsgCallback);
        m_setMsgCallback.insert(cb);
    }

    return 0;
}

int XMsgClient::UnRegisterMsgCb(XMsgCallback* cb)
{
    if (!cb) return -1;
    {
        rtc::CritScope l(&m_csMsgCallback);
        m_setMsgCallback.erase(cb);
    }

    return 0;
}

int XMsgClient::AddGroup(const std::string& groupid)
{

    FetchGroupSeqn(groupid);
    return 0;
}

int XMsgClient::DelGroup(const std::string& groupid)
{
    return 0;
}


int XMsgClient::SndMsg(const std::string& roomid, const std::string& rname, const std::string& msg)
{
    if (msg.length()>1024 || rname.length()>128) {
        return -2;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        std::vector<std::string> uvec;
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token\
                , m_nname, roomid, rname, uvec, msg\
                , pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, m_module, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    printf("XMsgClient SndMsg ok!!\n");
    return SendEncodeMsg(outstr);
}

int XMsgClient::GetMsg(pms::EMsgTag tag)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeGetMsg(outstr, m_uid, m_token, tag, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::OptRoom(pms::EMsgTag tag, const std::string& roomid, const std::string& rname, const std::string& remain)
{
    if (!pms::EMsgTag_IsValid(tag)) {
        return -1;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        std::vector<std::string> uvec;
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token\
                , m_nname, roomid, rname, uvec, ""\
                , tag, pms::EMsgType::TTXT, m_module, pms::EMsgFlag::FINVALID);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsgTo(const std::string& roomid, const std::string& rname, const std::string& msg, const std::vector<std::string>& uvec)
{
    if (msg.length()>1024 || rname.length()>128) {
        return -2;
    }
    if (uvec.size()==0) {
        return -3;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        if (uvec.size()==1) {
            m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token\
                    , m_nname, roomid, rname, uvec, msg\
                    , pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, m_module, pms::EMsgFlag::FSINGLE);
        } else {
            m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token\
                    , m_nname, roomid, rname, uvec, msg\
                    , pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, m_module, pms::EMsgFlag::FMULTI);
        }
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::NotifyMsg(const std::string& roomid, const std::string& rname, pms::EMsgTag tag, const std::string& msg)
{
    if (msg.length()>1024 || rname.length()>128) {
        return -2;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        std::vector<std::string> uvec;
        m_pMsgProcesser->EncodeSndMsg(outstr, m_uid, m_token\
                , m_nname, roomid, rname, uvec, msg\
                , tag, pms::EMsgType::TTXT, m_module, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncSeqn()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncSeqn(outstr, m_uid, m_token, m_curSeqn, m_module, pms::EStorageTag::TCOMMON, pms::EMsgFlag::FSINGLE);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::SyncSeqn was called\n");

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncData()
{
    printf("XMsgClient::SyncData m_curSeqn is:%lld, wait...\n", m_curSeqn);
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncData(outstr, m_uid, m_token, m_curSeqn, m_module, pms::EStorageTag::TCOMMON, pms::EMsgFlag::FSINGLE);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
     printf("XMsgClient::SyncData was called\n");
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupSeqn()
{
    int ret = 0;
    printf("XMsgClient::SyncGroupSeqn ---mapGroupSeqn.size:%lu\n", m_mapGroupSeqn.size());
    for (GroupSeqnMapIt it=m_mapGroupSeqn.begin(); it!=m_mapGroupSeqn.end();++it)
    {
        printf("XMsgClient::SyncGroupSeqn first group:%s, second seqn:%lld\n", it->first.c_str(), it->second);
        ret = SyncGroupSeqn(it->first, it->second, false);
    }
    return ret;
}

int XMsgClient::SyncGroupData()
{
    int ret = 0;
    for (GroupSeqnMapIt it=m_mapGroupSeqn.begin(); it!=m_mapGroupSeqn.end();++it)
    {
        ret = SyncGroupData(it->first, it->second, false);
    }
    return ret;
}

int XMsgClient::FetchGroupSeqn(const std::string& groupid)
{
    if (groupid.length()==0) return -10;
    int ret = 0;
    {
        ret = SyncGroupSeqn(groupid, -1, true);
    }
    return ret;
}

int XMsgClient::SyncGroupSeqn(const std::string& groupid)
{
    if (groupid.length()==0) return -10;
    int ret = 0;
    GroupSeqnMapIt it = m_mapGroupSeqn.find(groupid);
    if (it != m_mapGroupSeqn.end())
    {
        ret = SyncGroupSeqn(it->first, it->second, false);
    }
    return ret;
}

int XMsgClient::SyncGroupData(const std::string& groupid)
{
    if (groupid.length()==0) return -10;
    int ret = 0;
    GroupSeqnMapIt it = m_mapGroupSeqn.find(groupid);
    if (it != m_mapGroupSeqn.end())
    {
        ret = SyncGroupData(it->first, it->second, false);
    }
    return ret;
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

int XMsgClient::SyncGroupSeqn(const std::string& groupid, int64 seqn, bool isGetNew)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        if (isGetNew)
            m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, seqn, m_module, pms::EStorageTag::TGETNEW, pms::EMsgFlag::FGROUP);
        else
            m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, seqn, m_module, pms::EStorageTag::TCOMMON, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::SyncGroupSeqn was called\n");

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupData(const std::string& groupid, int64 seqn, bool isGetNew)
{
    printf("XMsgClient::SyncGroupData userid:%s, groupid:%s, seqn is:%lld, wait...\n"\
            , m_uid.c_str(), groupid.c_str(), seqn);
    std::string outstr;
    if (m_pMsgProcesser) {
        if (isGetNew)
        m_pMsgProcesser->EncodeSyncGroupData(outstr, m_uid, m_token, groupid, seqn, m_module, pms::EStorageTag::TGETNEW, pms::EMsgFlag::FGROUP);
        else
        m_pMsgProcesser->EncodeSyncGroupData(outstr, m_uid, m_token, groupid, seqn, m_module, pms::EStorageTag::TCOMMON, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    printf("XMsgClient::SyncGroupData was called\n");

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
            delete ptr;
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
    for(MsgCallbackSetIt it=m_setMsgCallback.begin();it!=m_setMsgCallback.end();++it)
    {
        if (*it)
        {
            (*it)->OnMsgServerState(MSNOT_CONNECTED);
            (*it)->OnMsgServerDisconnect();
        } else {
            printf("XMsgClient::OnServerDisconnect m_pCallback is null\n");
        }
    }
    //if (m_pCallback) {
    //    m_pCallback->OnMsgServerState(MSNOT_CONNECTED);
    //    m_pCallback->OnMsgServerDisconnect();
    //}
}

void XMsgClient::OnServerConnectionFailure()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    for(MsgCallbackSetIt it=m_setMsgCallback.begin();it!=m_setMsgCallback.end();++it)
    {
        if (*it)
        {
            (*it)->OnMsgServerState(MSNOT_CONNECTED);
            (*it)->OnMsgServerDisconnect();
        } else {
            printf("XMsgClient::OnServerConnectionFailure m_pCallback is null\n");
        }
    }
    //if (m_pCallback) {
    //    m_login = false;
    //    m_msState = MSNOT_CONNECTED;
    //    m_pCallback->OnMsgServerState(MSNOT_CONNECTED);
    //    m_pCallback->OnMsgServerConnectionFailure();
    //}
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

void XMsgClient::OnLogin(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    //LOGI("XMsgClient::OnLogin code:%d\n", code);
#else
    //std::cout << "XMsgClient::OnLogin code:" << code << std::endl;
#endif
    if (code == 0) {
        m_login = true;
        m_msState = MSCONNECTED;
        for(MsgCallbackSetIt it=m_setMsgCallback.begin();it!=m_setMsgCallback.end();++it)
        {
            if (*it)
            {
                (*it)->OnMsgServerState(MSCONNECTED);
                (*it)->OnMsgServerConnected();
            } else {
                printf("XMsgClient::OnLogin m_pCallback is null\n");
            }
        }
        //m_pCallback->OnMsgServerState(MSCONNECTED);
        //m_pCallback->OnMsgServerConnected();
    } else {
        Login();
    }
}

void XMsgClient::OnLogout(int code, const std::string& userid)
{
#ifdef WEBRTC_ANDROID
    //LOGI("XMsgClient::OnLogout code:%d\n", code);
#else
    //std::cout << "XMsgClient::OnLogout code:" << code << std::endl;
#endif
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    for(MsgCallbackSetIt it=m_setMsgCallback.begin();it!=m_setMsgCallback.end();++it)
    {
        if (*it)
        {
            (*it)->OnMsgServerState(MSNOT_CONNECTED);
            (*it)->OnMsgServerConnected();
        } else {
            printf("XMsgClient::OnLogin m_pCallback is null\n");
        }
    }
}

void XMsgClient::OnSndMsg(int code, const std::string& cont)
{
    for(MsgCallbackSetIt it=m_setMsgCallback.begin();it!=m_setMsgCallback.end();++it)
    {
        if (*it)
        {
            (*it)->OnSndMsg(cont);
        } else {
            printf("XMsgClient::OnSndMsg m_pCallback is null\n");
        }
    }
    //if (m_pCallback)
    //    m_pCallback->OnSndMsg(cont);
    //else
    //    printf("XMsgClient::OnSndMsg m_pCallback is null\n");

    pms::Entity entity;
    entity.ParseFromString(cont);
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnSndMsg sequence:%lld\n\n"\
            , entity.msg_seqs());

    return;
}

void XMsgClient::OnGetMsg(int code, const std::string& cont)
{
    for(MsgCallbackSetIt it=m_setMsgCallback.begin();it!=m_setMsgCallback.end();++it)
    {
        if (*it)
        {
            (*it)->OnGetMsg(cont);
        } else {
            printf("XMsgClient::OnGetMsg m_pCallback is null\n");
        }
    }
    //if (m_pCallback)
    //    m_pCallback->OnGetMsg(cont);
    //else
    //    printf("XMsgClient::OnSndMsg m_pCallback is null\n");
    return;
}

void XMsgClient::OnKeepLive(int code, const std::string& cont)
{

}

void XMsgClient::OnSyncSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnSyncSeqn ruserid:%s, sequence:%lld, maxseqn:%lld, m_curSeqn:%lld\n"\
            , store.ruserid().c_str(), store.sequence(), store.maxseqn(), m_curSeqn);
    printf("XMsgClient::OnSyncSeqn should be equal here???\n");
    //assert(store.maxseqn()>=m_curSeqn);
    if (store.mtag()==pms::EStorageTag::TCOMMON)
    {

    } else if (store.mtag()==pms::EStorageTag::TGETNEW)
    {
        if (store.mflag()==pms::EMsgFlag::FGROUP)
        {
            printf("XMsgClient::OnSyncSeqn group storeid:%s, ruserid:%s, seqn:%lld, maxseqn:%lld\n"\
                    , store.storeid().c_str(), store.ruserid().c_str(), store.sequence(), store.maxseqn());
            rtc::CritScope l(&m_csGroupSeqn);
            m_mapGroupSeqn.insert(make_pair(store.groupid(), store.maxseqn()));
            printf("XMsgClient::OnSyncSeqn mapGrouSeqn size:%lu\n", m_mapGroupSeqn.size());
            SyncGroupSeqn();
        }
    } else {

    }
    if (store.maxseqn()>=m_curSeqn)
    m_curSeqn = store.maxseqn(); // the sender sync seqn
    else
    printf("XMsgClient::OnSyncSeqn is smaller here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11111111!!!!!!!!!!!!\n");
    return;
}

void XMsgClient::OnSyncData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnSyncData ruserid:%s, storeid:%s, sequence:%lld, maxseqn:%lld, m_curSeqn:%lld\n\n"\
            , store.ruserid().c_str()\
            , store.storeid().c_str()\
            , store.sequence()\
            , store.maxseqn()\
            , m_curSeqn);
    printf("XMsgClient::OnSyncData should be equal here???\n");
    //assert(store.maxseqn()>=m_curSeqn);// ???????
    if (store.maxseqn()>=m_curSeqn)
    m_curSeqn = store.maxseqn(); // the receiver sync data and seqn
    else
    printf("XMsgClient::OnSyncData is smaller here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    pms::Entity entity;
    entity.ParseFromString(store.content());
    if (entity.usr_toto().users_size()==0)
    {
        printf("OnSyncData usr_toto.size is 0 so return\n");
        return;
    }
    printf("OnSyncData entity.usr_from:%s, usr_toto:%s, msg_cont:%s\n"\
            , entity.usr_from().c_str()\
            , entity.usr_toto().users(0).c_str()\
            , entity.msg_cont().c_str());
    return;
}

void XMsgClient::OnGroupNotify(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnGroupNotify ruserid:%s, groupid:%s, rsvrcmd:%d, mflag:%d, sequence:%lld\n\n"\
            , store.ruserid().c_str()\
            , store.groupid().c_str()\
            , store.rsvrcmd()\
            , store.mflag()\
            , store.sequence());
    SyncGroupData(store.groupid());
    return;
}

void XMsgClient::OnSyncGroupData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("XMsgClient::OnSyncGroupData ruserid:%s, groupid:%s, rsvrcmd:%d, mflag:%d, sequence:%lld, maxseqn:%lld\n\n"\
            , store.ruserid().c_str()\
            , store.groupid().c_str()\
            , store.rsvrcmd()\
            , store.mflag()\
            , store.sequence()\
            , store.maxseqn());

    if (store.maxseqn()>=m_curGroupSeqn)
    m_curGroupSeqn = store.maxseqn(); // the group sync seqn
    pms::Entity entity;
    entity.ParseFromString(store.content());
    printf("OnSyncGroupData entity.usr_from:%s, rom_id:%s, msg_cont:%s\n"\
            , entity.usr_from().c_str()\
            , entity.rom_id().c_str()\
            , entity.msg_cont().c_str());
    return;
}

void XMsgClient::OnAddGroup(int code, const std::string& groupid)
{

}

void XMsgClient::OnDelGroup(int code, const std::string& groupid)
{

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
