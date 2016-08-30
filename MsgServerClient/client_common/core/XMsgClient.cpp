//
//  XMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "core/XMsgClient.h"


#define TIMEOUT_TS (60*1000)

XMsgClient::XMsgClient()
: XJSBuffer()
, m_pCallback(nullptr)
, m_pClientImpl(nullptr)
, m_pMsgProcesser(nullptr)
, m_lastUpdateTime(0)
, m_uid("")
, m_token("")
, m_nname("")
, m_server("180.150.179.128")
, m_port(6630)
, m_autoConnect(true)
, m_login(false)
, m_msState(MSNOT_CONNECTED)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient XMsgClient ok!!\n");
#else
    LOG(INFO) << "XMsgClient XMsgClient ok!!";
#endif
}

XMsgClient::~XMsgClient()
{

}

int XMsgClient::Init(const std::string& uid, const std::string& token, const std::string& nname, int devType, int push, int module)
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
    m_version = MSG_VERSION;
    m_devType = devType;
    m_enablePush = push;
    m_module = (pms::EModuleType)module;

#if WEBRTC_ANDROID
    LOGI("XMsgClient Init ok!!\n");
#else
    LOG(INFO) << "XMsgClient Init ok!!";
#endif
    return 0;
}

int XMsgClient::Unin()
{
    if (m_uWait4AckMsgMap.size()>0)
        m_uWait4AckMsgMap.clear();
    if (m_uSyncedMsgMap.size()>0)
        m_uSyncedMsgMap.clear();
    if (m_uUserSeqnMap.size()>0)
        m_uUserSeqnMap.clear();
    if (m_uRecvMsgList.size()>0)
        m_uRecvMsgList.clear();
    
    if (m_gWait4AckMsgMap.size()>0)
        m_gWait4AckMsgMap.clear();
    if (m_gSyncedMsgMap.size()>0)
        m_gSyncedMsgMap.clear();
    if (m_gUserSeqnMap.size()>0)
        m_gUserSeqnMap.clear();
    if (m_gRecvMsgList.size()>0)
        m_gRecvMsgList.clear();
    
    if (m_Wait4CheckSeqnKeyMap.size()>0)
        m_Wait4CheckSeqnKeyMap.clear();
    
    if (m_MaxSeqnMap.size()>0)
        m_MaxSeqnMap.clear();
    
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

#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnServerDisconnect m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnServerDisconnect m_pCallback is null";
#endif
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
        m_pCallback->OnCmdCallback(0, MSRMV_GROUP, groupid, cbData);
    }
    return 0;
}


int XMsgClient::SndMsg(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, int push)
{
    if (groupid.length()==0 || msg.length()==0 || msg.length()>1024 || grpname.length()>128) {
        return -2;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        std::vector<std::string> uvec;
        m_pMsgProcesser->EncodeSndMsg(outstr, outmsgid, m_uid, m_token\
                , m_nname, groupid, grpname, uvec, msg\
                , tag, type, module, flag, push);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    UAddWait4AckMsg(outmsgid, outstr);
#if WEBRTC_ANDROID
    LOGI("XMsgClient SndMsg ok!!!\n");
#else
    LOG(INFO) << "XMsgClient SndMsg ok!!!";
#endif
    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsgTo(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, int push, const std::vector<std::string>& uvec)
{
    if (groupid.length()==0 || msg.length()==0 || msg.length()>1024 || grpname.length()>128) {
        return -2;
    }
    if (uvec.size()==0) {
        return -3;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSndMsg(outstr, outmsgid, m_uid, m_token\
                                      , m_nname, groupid, grpname, uvec, msg\
                                      , tag, type, module, flag, push);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    GAddWait4AckMsg(outmsgid, outstr);
#if WEBRTC_ANDROID
    LOGI("XMsgClient SndMsgTo ok!\n");
#else
    LOG(INFO) << "XMsgClient SndMsgTo ok!";
#endif
    return SendEncodeMsg(outstr);
}

int XMsgClient::FetchSeqn()
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::FetchSeqn seqn is wait...\n");
#else
    LOG(INFO) << "XMsgClient::FetchSeqn seqn is wait...";
#endif
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncSeqn(outstr, m_uid, m_token, -1, m_module, pms::EStorageTag::TFETCHSEQN, pms::EMsgFlag::FSINGLE, pms::EMsgRole::RSENDER);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::FetchSeqn was called\n");
#else
    LOG(INFO) << "XMsgClient::FetchSeqn was called";
#endif

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncSeqn(int64 seqn, int role)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncSeqn seqn is:%lld, wait...\n", seqn);
#else
    LOG(INFO) << "XMsgClient::SyncSeqn seqn is:" << seqn << ", wait...";
#endif
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncSeqn(outstr, m_uid, m_token, seqn, m_module, pms::EStorageTag::TSEQN, pms::EMsgFlag::FSINGLE, role);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncSeqn was called\n");
#else
    LOG(INFO) << "XMsgClient::SyncSeqn was called";
#endif

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncData(int64 seqn)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncData seqn is:%lld\n", seqn);
#else
    LOG(INFO) << "XMsgClient::SyncData seqn is:" << seqn;
#endif
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncData(outstr, m_uid, m_token, seqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FSINGLE);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncData was called\n");
#else
    LOG(INFO) << "XMsgClient::SyncData was called";
#endif
    int ret = SendEncodeMsg(outstr);
    if (ret>0) {
        rtc::CritScope cs(&m_csWait4CheckSeqnKey);
        char seqnKey[256] = {0};
        sprintf(seqnKey, "%s:%lld", m_uid.c_str(), seqn);
        if (m_Wait4CheckSeqnKeyMap.find(seqnKey)==m_Wait4CheckSeqnKeyMap.end()) {
            m_Wait4CheckSeqnKeyMap.insert(make_pair(seqnKey, 1));
        } else {
            m_Wait4CheckSeqnKeyMap[seqnKey]++;
        }
#if WEBRTC_ANDROID
        LOGI("XMsgClient::SyncData seqnKey:%s, times:%d\n", seqnKey, m_Wait4CheckSeqnKeyMap[seqnKey]);
#else
        LOG(INFO) << "XMsgClient::SyncData seqnKey:" << seqnKey << ", times:" << m_Wait4CheckSeqnKeyMap[seqnKey];
#endif       
    }
    return ret;
}

int XMsgClient::FetchGroupSeqn(const std::string& groupid)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::FetchGroupSeqn groupid is:%s\n", groupid.c_str());
#else
    LOG(INFO) << "XMsgClient::FetchGroupSeqn groupid is:" << groupid;
#endif
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, -1, m_module, pms::EStorageTag::TFETCHSEQN, pms::EMsgFlag::FGROUP, pms::EMsgRole::RSENDER);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::FetchGroupSeqn was called\n");
#else
    LOG(INFO) << "XMsgClient::FetchGroupSeqn was called";
#endif

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupSeqn(const std::string& groupid, int64 seqn, int role)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncGroupSeqn groupid is:%s, seqn is:%lld\n", groupid.c_str(), seqn);
#else
    LOG(INFO) << "XMsgClient::SyncGroupSeqn groupid is:" << groupid << ", seqn is:" << seqn;
#endif
    std::string outstr;
    if (m_pMsgProcesser) {
            m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, seqn, m_module, pms::EStorageTag::TSEQN, pms::EMsgFlag::FGROUP, role);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncGroupSeqn was called\n");
#else
    LOG(INFO) << "XMsgClient::SyncGroupSeqn was called";
#endif

    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupData(const std::string& groupid, int64 seqn)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncGroupData userid:%s, groupid:%s, seqn is:%lld, wait...\n"\
           , m_uid.c_str(), groupid.c_str(), seqn);
#else
    LOG(INFO) << "XMsgClientit::SyncGruopData userid:" << m_uid << ", groupid:" << groupid << ", seqn:" << seqn << ", wait...";
#endif
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncGroupData(outstr, m_uid, m_token, groupid, seqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::SyncGroupData was called\n");
#else
    LOG(INFO) << "XMsgClient::SyncGroupData was called";
#endif

    int ret = SendEncodeMsg(outstr);
    if (ret>0) {
        rtc::CritScope cs(&m_csWait4CheckSeqnKey);
        char seqnKey[256] = {0};
        sprintf(seqnKey, "%s:%lld", groupid.c_str(), seqn);
        if (m_Wait4CheckSeqnKeyMap.find(seqnKey)==m_Wait4CheckSeqnKeyMap.end()) {
            m_Wait4CheckSeqnKeyMap.insert(make_pair(seqnKey, 1));
        } else {
            m_Wait4CheckSeqnKeyMap[seqnKey]++;
        }
#if WEBRTC_ANDROID
        LOGI("XMsgClient::SyncGroupData seqnKey:%s, times:%d\n", seqnKey, m_Wait4CheckSeqnKeyMap[seqnKey]);
#else
        LOG(INFO) << "XMsgClient::SyncGroupData seqnKey:" << seqnKey << ", times:" << m_Wait4CheckSeqnKeyMap[seqnKey];
#endif
    }
    return ret;
}

int XMsgClient::UpdateSetting(int64 setType, const std::string& jsonSetting)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::UpdateSetting setType:%lld wait...\n", setType);
#else
    LOG(INFO) << "XMsgClientit::UpdateSetting setType:" << setType << " wait...";
#endif
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeUpdateSetting(outstr, m_uid, setType, jsonSetting, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::UpdateSetting was called\n");
#else
    LOG(INFO) << "XMsgClient::UpdateSetting was called";
#endif
    
    return SendEncodeMsg(outstr);
    return 0;
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
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnServerDisconnect m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnServerDisconnect m_pCallback is null";
#endif
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
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnServerConnectionFailure m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnServerConnectionFailure m_pCallback is null";
#endif
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
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpLogin m_pCallback is null\n");
#else
            LOG(INFO) << "XMsgClient::OnHelpLogin m_pCallback is null";
#endif
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
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpLogout m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnHelpLogout m_pCallback is null";
#endif
    }
}

void XMsgClient::OnHelpSndMsg(int code, const std::string& cont)
{
    if (m_pCallback)
    {
        m_pCallback->OnSndMsg(code, cont);
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpSndMsg m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnHelpSndMsg m_pCallback is null";
#endif
    }
    return;
}

void XMsgClient::OnHelpKeepLive(int code, const std::string& cont)
{

}

void XMsgClient::OnHelpSyncSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncSeqn store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpSyncSeqn store.ParseFromString error";
#endif
        return;
    }
    if (store.result()!=0)
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncSeqn store.result is not 0, get msg error\n");
#else
        LOG(INFO) << "OnHelpSyncSeqn store.result is not 0, get msg error";
#endif
        return;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::OnHelpSyncSeqn ruserid:%s, sequence:%lld, maxseqn:%lld\n"\
            , store.ruserid().c_str(), store.sequence(), store.maxseqn());
#else
    LOG(INFO) << "XMsgClient::OnHelpSyncSeqn ruserid:" << store.ruserid() << ", sequence:" << store.sequence() << ", maxseqn:" << store.maxseqn();
#endif
    // store or update storeid's max seqn
    if (store.maxseqn()>0) {
        UserSeqnMapIt uit =  m_MaxSeqnMap.find(store.storeid());
        if (uit == m_MaxSeqnMap.end()) {
            m_MaxSeqnMap.insert(make_pair(store.storeid(), store.maxseqn()));
        } else {
            if (uit->second < store.maxseqn()) {
#if WEBRTC_ANDROID
                LOGI("XMsgClient::OnHelpSyncSeqn update max seqn old:%lld, new:%lld\n"\
                     , uit->second, store.maxseqn());
#else
                LOG(INFO) << "XMsgClient::OnHelpSyncSeqn update max seqn old:" << uit->second << ", new:" << store.maxseqn();
#endif
                uit->second = store.maxseqn();
            }
        }
    }
    switch (store.mtag())
    {
        case pms::EStorageTag::TSEQN:
        {
            if (store.mflag()==pms::EMsgFlag::FGROUP)
            {
                UserSeqnMapIt it = m_gUserSeqnMap.find(store.storeid());
                // if cur seqn is smaller then maxseqn just synced
                // then sync data with the smaller seqn
                if (it != m_gUserSeqnMap.end() && it->second < store.maxseqn())
                {
#if WEBRTC_ANDROID
                    LOGI("XMsgClient::OnHelpSyncSeqn group SyncGruopData storeid:%s, seqn:%lld\n", store.storeid().c_str(), it->second);
#else
                    LOG(INFO) << "XMsgClient::OnHelpSyncSeqn group SyncGroupData storeid:" << store.storeid() << ", seqn:" << it->second;
#endif
                    SyncGroupData(store.storeid(), it->second);
                }
            } else if (store.mflag()==pms::EMsgFlag::FSINGLE)
            {
                UserSeqnMapIt it = m_uUserSeqnMap.find(store.storeid());
                // if cur seqn is smaller then maxseqn just synced
                // then sync data with the smaller seqn
                if (it != m_uUserSeqnMap.end() && it->second < store.maxseqn())
                {
#if WEBRTC_ANDROID
                    LOGI("XMsgClient::OnHelpSyncSeqn single SyncData storeid:%s, seqn:%lld\n", store.storeid().c_str(), it->second);
#else
                    LOG(INFO) << "XMsgClient::OnHelpSyncSeqn single SyncData storeid:" << store.storeid() << ", seqn:" << it->second;
#endif
                    SyncData(it->second);
                }
            } else {
#if WEBRTC_ANDROID
                LOGI("XMsgClient::OnHelpSyncSeqn store.mflag:%d is not handled\n", store.mflag());
#else
                LOG(INFO) << "XMsgClient::OnHelpSyncSeqn store.mflag:" << store.mflag() << " is not handled";
#endif
            }
        }
            break;

        case pms::EStorageTag::TFETCHSEQN:
        {
            if (store.mflag()==pms::EMsgFlag::FGROUP)
            {
#if WEBRTC_ANDROID
                LOGI("XMsgClient::OnHelpSyncSeqn fetchseqn group storeid:%s, ruserid:%s, seqn:%lld, maxseqn:%lld\n"\
                       , store.storeid().c_str(), store.ruserid().c_str(), store.sequence(), store.maxseqn());
#else
                LOG(INFO) << "XMsgClient::OnHelpSyncSeqn fetchseqn group storeid:" << store.storeid() << ", ruserid" << store.ruserid() << ", seqn:" << store.sequence() << ", maxseqn:" << store.maxseqn();
#endif
                if (m_pCallback)
                {
                    if (store.maxseqn()>=0)
                    {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "add ok";
                        cbData.seqn = store.maxseqn();
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(0, MSADD_GROUP, store.storeid(), cbData);
                        m_pCallback->OnCmdCallback(0, MSFETCH_SEQN, store.storeid(), cbData);
                    } else {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "add fail";
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(-1, MSADD_GROUP, store.storeid(), cbData);
                        m_pCallback->OnCmdCallback(-1, MSFETCH_SEQN, store.storeid(), cbData);
                    }
                } else {
#if WEBRTC_ANDROID
                    LOGI("XMsgClient::OnHelpSyncSeqn fetchseqn group m_pCallback is null\n");
#else
                    LOG(INFO) << "XMsgClient::OnHelpSyncSeqn fetchseqn group m_pCallback is null";
#endif
                }
            } else if (store.mflag()==pms::EMsgFlag::FSINGLE)
            {
#if WEBRTC_ANDROID
                LOGI("XMsgClient::OnHelpSyncSeqn fetchseqn single storeid:%s, ruserid:%s, seqn:%lld, maxseqn:%lld\n"\
                       , store.storeid().c_str(), store.ruserid().c_str(), store.sequence(), store.maxseqn());
#else
                LOG(INFO) << "XMsgClient::OnHelpSyncSeqn fetch seqn single storeid:" << store.storeid() << ", ruserid:" << store.ruserid() << ", seqn:" << store.sequence() << ", maxseqn:" << store.maxseqn();
#endif
                if (m_pCallback)
                {
                    if (store.maxseqn()>=0)
                    {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "fetch ok";
                        cbData.seqn = store.maxseqn();
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(0, MSFETCH_SEQN, "", cbData);
                    } else {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "fetch fail";
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(-1, MSFETCH_SEQN, "", cbData);
                    }
                } else {
#if WEBRTC_ANDROID
                    LOGI("XMsgClient::OnHelpSyncSeqn fetchseqn single m_pCallback is null\n");
#else
                    LOG(INFO) << "XMsgClient::OnHelpSyncSeqn fetchseqn single m_pCallback is null";
#endif
                }
            }
        }
            break;
        default:
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpSyncSeqn mtag :%d not handle\n", store.mtag());
#else
            LOG(INFO) << "XMsgClient::OnHelpSyncSeqn mtag:" << store.mtag() << " not handle";
#endif
            break;
    }
    return;
}

void XMsgClient::OnHelpSyncData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncData store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpSyncData store.ParseFromString error";
#endif
        return;
    }
    if (store.result()!=0)
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncData store.result is not 0, get msg error\n");
#else
        LOG(INFO) << "OnHelpSyncData store.result is not 0, get msg error";
#endif
        return;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::OnHelpSyncData ruserid:%s, storeid:%s, sequence:%lld, maxseqn:%lld\n\n"\
            , store.ruserid().c_str()\
            , store.storeid().c_str()\
            , store.sequence()\
            , store.maxseqn());
#else
    LOG(INFO) << "XMsgClient::OnHelpSyncData ruserid:" << store.ruserid() \
                        << ", storeid:" << store.storeid() \
                        << ", sequence:" << store.sequence() \
                        << ", maxseqn:" << store.maxseqn();
#endif

    char seqnKey[256] = {0};
    sprintf(seqnKey, "%s:%lld", store.storeid().c_str(), store.sequence());
    UAddSyncedMsg(seqnKey, store);
    UUpdateUserSeqn();

#if WEBRTC_ANDROID
    LOGI("XMsgClient::OnHelpSyncData m_uRecvMsgList.size:%u\n", m_uRecvMsgList.size());
#else
    LOG(INFO) << "XMsgClient::OnHelpSyncData m_uRecvMsgList.size:" << m_uRecvMsgList.size();
#endif
    for (RecvMsgListIt it = m_uRecvMsgList.begin();it!=m_uRecvMsgList.end();++it)
    {
        if (m_pCallback)
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpSyncData call OnRecvGroupMsg seqn:%lld, seqnid:%s\n", it->seqn, it->seqnid.c_str());
#else
            LOG(INFO) << "XMsgClient::OnHelpSyncData call OnRecvGroupMsg seqn:" << it->seqn << ", seqnid:" << it->seqnid;
#endif
            m_pCallback->OnRecvMsg(it->seqn, it->msgcont);
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpSyncData m_pCallback is null\n");
#else
            LOG(INFO) << "XMsgClient::OnHelpSyncData m_pCallback is null";
#endif
        }
    }
    m_uRecvMsgList.clear();
    return;
}

void XMsgClient::OnHelpSyncGroupData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncGroupData store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpSyncGroupData store.ParseFromString error";
#endif
        return;
    }
    if (store.result()!=0)
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncGroupData store.result is not 0, get msg error\n");
#else
        LOG(INFO) << "OnHelpSyncGroupData store.result is not 0, get msg error";
#endif
        return;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::OnHelpSyncGroupData ruserid:%s, groupid:%s, storeid:%s, rsvrcmd:%d, mflag:%d, sequence:%lld, maxseqn:%lld\n\n"\
            , store.ruserid().c_str()\
            , store.groupid().c_str()\
            , store.storeid().c_str()\
            , store.rsvrcmd()\
            , store.mflag()\
            , store.sequence()\
            , store.maxseqn());
#else
    LOG(INFO) << "XMsgClient::OnHelpSyncGroupData ruserid:" << store.ruserid() \
                        << ", groupid:" << store.groupid() \
                        << ", storeid:" << store.storeid() \
                        << ", rsvrcmd:" << store.rsvrcmd() \
                        << ", mflag:" << store.mflag() \
                        << ", sequence:" << store.sequence() \
                        << ", maxseqn:" << store.maxseqn();
#endif

    char seqnKey[256] = {0};
    sprintf(seqnKey, "%s:%lld", store.storeid().c_str(), store.sequence());
#if WEBRTC_ANDROID
    LOGI("===>OnHelpSyncGroupData storeid.len:%u, storeid:%s, seqn:%lld\n", store.storeid().length(), store.storeid().c_str(), store.sequence());
#else
    LOG(INFO) << "OnHelpSyncGroupData storeid.len:" << store.storeid().length() << ", storeid:" << store.storeid() << ", seqn:" << store.sequence();
#endif
    GAddSyncedMsg(seqnKey, store);
    GUpdateUserSeqn(store.storeid());

#if WEBRTC_ANDROID
    LOGI("XMsgClient::OnHelpSyncGroupData m_gRecvMsgList.size:%u\n", m_gRecvMsgList.size());
#else
    LOG(INFO) << "XMsgClient::OnHelpSyncGroupData m_gRecvMsgList.size:" << m_gRecvMsgList.size();
#endif
    for (RecvMsgListIt it = m_gRecvMsgList.begin();it!=m_gRecvMsgList.end();++it)
    {
        if (m_pCallback)
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpSyncGroupData call OnRecvGroupMsg seqn:%lld, seqnid:%s\n", it->seqn, it->seqnid.c_str());
#else
            LOG(INFO) << "XMsgClient::OnHelpSyncGroupData call OnRecvGroupMsg seqn:" << it->seqn << ", seqnid:" << it->seqnid;
#endif
            m_pCallback->OnRecvGroupMsg(it->seqn, it->seqnid, it->msgcont);
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpSyncGroupData m_pCallback is null\n");
#else
            LOG(INFO) << "XMsgClient::OnHelpSyncGroupData m_pCallback is null";
#endif
        }
    }
    m_gRecvMsgList.clear();
    return;
}

void XMsgClient::OnHelpGroupNotify(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpGroupNotify store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpGroupNotify store.ParseFromString error";
#endif
        return;
    }
    if (store.result()!=0)
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpGroupNotify store.result is not 0, get msg error\n");
#else
        LOG(INFO) << "OnHelpGroupNotify store.result is not 0, get msg error";
#endif
        return;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::OnHelpGroupNotify ruserid:%s, groupid:%s, rsvrcmd:%d, mflag:%d, sequence:%lld\n\n"\
           , store.ruserid().c_str()\
           , store.groupid().c_str()\
           , store.rsvrcmd()\
           , store.mflag()\
           , store.sequence());
#else
    LOG(INFO) << "XMsgClient::OnHelpGroupNotify ruserid:" \
           << store.ruserid() \
           << ", groupid:" << store.groupid() \
           << ", rsvrcmd:" << store.rsvrcmd() \
           << ", mflag:" << store.mflag() \
           << ", sequence:" << store.sequence();
#endif
    if (m_pCallback)
    {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpGroupNotify finding...store.groupid:%s\n", store.groupid().c_str());
#else
        LOG(INFO) << "XMsgClient::OnHelpGroupNotify finding...store.groupid:" << store.groupid();
#endif
        UserSeqnMapIt it = m_gUserSeqnMap.find(store.groupid());
        if (it != m_gUserSeqnMap.end())
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpGroupNotify SyncGroupSeqn was called, grouid:%s, seqn:%lld\n", it->first.c_str(), it->second);
#else
            LOG(INFO) << "XMsgClient::OnHelpGroupNotify SyncGroupSeqn was called, groupid:" << it->first << ", seqn:" << it->second;
#endif
            SyncGroupSeqn(it->first, it->second, pms::EMsgRole::RSENDER);
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpGroupNotify not find store.groupid:%s\n", store.groupid().c_str());
#else
            LOG(INFO) << "XMsgClient::OnHelpGroupNotify not find store.groupid:" << store.groupid();
#endif
        }
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpGroupNotify m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnHelpGroupNotify m_pCallback is null";
#endif
    }
    return;
}

// this means you need to sync seqn, get new seqn
// you just get new seqn from server
void XMsgClient::OnHelpNotifySeqn(int code, const std::string& cont)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient::OnHelpNotifySeqn code:%d, cont.size:%d\n", code, cont.size());
#else
    LOG(INFO) << "XMsgClient::OnHelpNotifySeqn code:" << code << ", cont.size:" << cont.size();
#endif
    if (code==0)
    {
        UserSeqnMapIt  itCurSeqn = m_uUserSeqnMap.find(m_uid);
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpNotifySeqn itCurSeqn is:%lld\n", itCurSeqn->second);
#else
        LOG(INFO) << "XMsgClient::OnHelpNotifySeqn itCurSeqn is:" << itCurSeqn->second;
#endif
        if (itCurSeqn->second<0)
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpNotifySeqn itCurSeqn is <0, so return\n");
#else
            LOG(INFO) << "XMsgClient::OnHelpNotifySeqn itCurSeqn is <0, so return";
#endif
            return;
        }
        this->SyncSeqn(itCurSeqn->second, pms::EMsgRole::RSENDER);
    }
    //if (m_pCallback)
    //{
    //    m_pCallback->OnNotifySeqn(code, cont);
    //}
    return;
}

// this may not be useful, because the server will not notify to sync data directly
// instead server will notify to sync seqn
void XMsgClient::OnHelpNotifyData(int code, const std::string& cont)
{
    //if (m_pCallback)
    //{
    //    m_pCallback->OnNotifyData(code, cont);
    //}
    //return;
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
