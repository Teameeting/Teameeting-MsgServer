//
//  XMsgClient.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef __dyncRTCMsgClient__XMsgClient__
#define __dyncRTCMsgClient__XMsgClient__

#include <stdio.h>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>

#include "core/XTcpClientImpl.h"
#include "core/XMsgProcesser.h"
#include "core/XMsgCallback.h"
#include "core/XJSBuffer.h"

#include "RTMsgCommon.h"

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

#define DEVICE_IOS (1)
#define DEVICE_ANDROID (2)

#define PUSH_ENABLE (1)
#define PUSH_DISABLE (2)

class XMsgClientHelper {
public:
    XMsgClientHelper() {}
    ~XMsgClientHelper() {}

    virtual void OnHelpLogin(int code, const std::string& userid) = 0;
    virtual void OnHelpLogout(int code, const std::string& userid) = 0;
    virtual void OnHelpSndMsg(int code, const std::string& cont) = 0;
    virtual void OnHelpKeepLive(int code, const std::string& cont) = 0;
    virtual void OnHelpSyncSeqn(int code, const std::string& cont) = 0;
    virtual void OnHelpSyncData(int code, const std::string& cont) = 0;
    virtual void OnHelpSyncGroupData(int code, const std::string& cont) = 0;
    
    virtual void OnHelpGroupNotify(int code, const std::string& cont) = 0;
    virtual void OnHelpNotifySeqn(int code, const std::string& cont) = 0;
    virtual void OnHelpNotifyData(int code, const std::string& cont) = 0;
};

class XMsgClient
: public XTcpClientCallback
, public XMsgClientHelper
, public XJSBuffer{
public:
    XMsgClient();
    ~XMsgClient();

    int Init(const std::string& uid, const std::string& token, const std::string& nname, int devType, int push, int module);
    int Unin();

    int RegisterMsgCb(XMsgCallback* cb);
    int UnRegisterMsgCb(XMsgCallback* cb);

    int ConnToServer(const std::string& server="", int port=0, bool bAutoConnect=true);
    
    int AddGroup(const std::string& groupid);
    int RmvGroup(const std::string& groupid);

    int SndMsg(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, int push);
    int SndMsgTo(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, int push, const std::vector<std::string>& uvec);
    
    int FetchSeqn();
    int SyncSeqn(int64 seqn, int role);
    int SyncData(int64 seqn);
    int FetchGroupSeqn(const std::string& groupid);
    int SyncGroupSeqn(const std::string& groupid, int64 seqn, int role);
    int SyncGroupData(const std::string& gropuid, int64 seqn);
    int UpdateSetting(int64 setType, const std::string& jsonSetting);

    MSState MSStatus() { return m_msState; }
    void SetUserId(const std::string& userid) { m_uid = userid; }
    void SetToken(const std::string& token) { m_token = token; }
    void SetNickName(const std::string& nickname) { m_nname = nickname; }

    void SetUIconUrl(const std::string& uiconurl) { m_uicon = uiconurl; }
    void SetEnablePush(int push)
    {
        m_enablePush = push;
    }

    void InitUserSeqns(const std::string& seqnid, int64 seqn)
    {
#if WEBRTC_ANDROID
        LOGI("InitUserSeqns seqnid:%s, seqn:%lld\n", seqnid.c_str(), seqn);
#else
        LOG(INFO) << "InitUserSeqns seqnid:" << seqnid << ", seqn:" << seqn;
#endif
        if (seqnid.compare(m_uid)==0)
        {
            m_uUserSeqnMap.insert(make_pair(seqnid, seqn));
        } else {
            m_gUserSeqnMap.insert(make_pair(seqnid, seqn));
        }
    }

    void UpdateUserSeqns(const std::string& seqnid, int64 seqn)
    {
#if WEBRTC_ANDROID
        LOGI("UpdateUserSeqns seqnid:%s, seqn:%lld\n", seqnid.c_str(), seqn);
#else
        LOG(INFO) << "UpdateUserSeqns seqnid:" << seqnid << ", seqn:" << seqn;
#endif
         if (seqnid.compare(m_uid)==0)
         {
            m_uUserSeqnMap[seqnid] = seqn;
         } else {
            m_gUserSeqnMap[seqnid] = seqn;
         }
    }

public:
    // For XTcpClientCallback
    virtual void OnServerConnected();
    virtual void OnServerDisconnect();
    virtual void OnServerConnectionFailure();

    virtual void OnTick();
    virtual void OnMessageSent(int err);
    virtual void OnMessageRecv(const char*pData, int nLen);

    // For XMsgClientHelper
    virtual void OnHelpLogin(int code, const std::string& userid);
    virtual void OnHelpLogout(int code, const std::string& userid);
    virtual void OnHelpSndMsg(int code, const std::string& cont);
    virtual void OnHelpKeepLive(int code, const std::string& cont);
    virtual void OnHelpSyncSeqn(int code, const std::string& cont);
    virtual void OnHelpSyncData(int code, const std::string& cont);
    virtual void OnHelpGroupNotify(int code, const std::string& cont);
    virtual void OnHelpSyncGroupData(int code, const std::string& cont);
    virtual void OnHelpNotifySeqn(int code, const std::string& cont);
    virtual void OnHelpNotifyData(int code, const std::string& cont);

    // For XJSBuffer
    virtual void OnRecvMessage(const char*message, int nLen);

private:
    int Login();
    int Logout();
    bool RefreshTime();
    int KeepAlive();
    int SendEncodeMsg(std::string& msg);
    unsigned short readShort(char** pptr);
    void writeShort(char** pptr, unsigned short anInt);

    typedef struct _CachedMsgInfo {
        int64 seqn;
        std::string seqnid;
        std::string msgcont;
    }CachedMsgInfo;
    
    //<msgid, str_msg>
    typedef std::unordered_map<std::string, std::string>        Wait4AckMsgMap;
    typedef Wait4AckMsgMap::iterator                            Wait4AckMsgMapIt;

    //<seqnkey, storage_msg>
    typedef std::unordered_map<std::string, pms::StorageMsg>    SyncedMsgMap;
    typedef SyncedMsgMap::iterator                              SyncedMsgMapIt;

    //<seqnid, seqn>
    typedef std::unordered_map<std::string, int64>              UserSeqnMap;
    typedef UserSeqnMap::iterator                               UserSeqnMapIt;

    //<StorageMsg>
    typedef std::list<CachedMsgInfo>                            RecvMsgList;
    typedef RecvMsgList::iterator                               RecvMsgListIt;

    // for user
    bool UAddWait4AckMsg(const std::string& msgid, const std::string& strMsg)
    {
        rtc::CritScope cs(&m_csuWait4AckMsg);
        Wait4AckMsgMapIt it = m_uWait4AckMsgMap.find(msgid);
        if (it == m_uWait4AckMsgMap.end())
        {
            m_uWait4AckMsgMap.insert(make_pair(msgid, strMsg));
        }
        return true;
    }

    bool UAddSyncedMsg(const std::string& seqnKey, pms::StorageMsg pmsMsg)
    {
	    rtc::CritScope cs(&m_csuSyncedMsg);
        SyncedMsgMapIt it = m_uSyncedMsgMap.find(seqnKey);
        if (it == m_uSyncedMsgMap.end())
        {
            m_uSyncedMsgMap.insert(make_pair(seqnKey, pmsMsg));
        }
        return true;
    }

    bool UUpdateUserSeqn()
    {
        UserSeqnMapIt  itCurSeqn = m_uUserSeqnMap.find(m_uid);
#if WEBRTC_ANDROID
        LOGI("UUpdateUserSeqn get here, itCurSeqn is:%lld\n", itCurSeqn->second);
#else
        LOG(INFO) << "UUpdateUserSeqn get here, itCurSeqn is:" << itCurSeqn->second;
#endif
        if (itCurSeqn->second<0)
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::UUpdateUserSeqn itCurSeqn is <0, so return\n");
#else
            LOG(INFO) << "XMsgClient::UUpdateUserSeqn itCurSeqn is <0, so return";
#endif
            return false;
        }
        while(1)
        {
#if WEBRTC_ANDROID
            LOGI("UUpdateUserSeqn m_gSyncedMsgMap.size:%u\n", m_gSyncedMsgMap.size());
#else
            LOG(INFO) << "UUpdateUserSeqn m_gSyncedMsgMap.size:" << m_gSyncedMsgMap.size();
#endif
            if (m_uSyncedMsgMap.size()==0) break;
            char sk[256] = {0};
            sprintf(sk, "%s:%lld", m_uid.c_str(), itCurSeqn->second +1);
            SyncedMsgMapIt it = m_uSyncedMsgMap.find(sk);
            if (it != m_uSyncedMsgMap.end())
            {
                pms::Entity en;
                if (!en.ParseFromString(it->second.content()))
                {
#if WEBRTC_ANDROID
                    LOGI("UUpdateUserSeqn en.ParseFromString error\n");
#else
                    LOG(INFO) << "UUpdateUserSeqn en.ParseFromString error";
#endif
                    m_uSyncedMsgMap.erase(sk);
                    continue;
                }
                // find curSeqn+1 msg, update curSeqn, and callback this msg, and continue
                itCurSeqn->second += 1;
#if WEBRTC_ANDROID
                LOGI("UUpdateUserSeqn update here, itCurSeqn is:%lld\n", itCurSeqn->second);
#else
                LOG(INFO) << "UUpdateUserSeqn update here, itCurSeqn is:" << itCurSeqn->second;
#endif
                if (m_uid.compare(en.usr_from())==0)
                {
                    // recv the msg you send to other
                    // erase the msg in Wait4AckMsgMap by entity.cmsg_id
                    m_uWait4AckMsgMap.erase(en.cmsg_id());
#if WEBRTC_ANDROID
                    LOGI("UUpdateUserSeqn update the msg you send to other, msgid is:%s\n", en.cmsg_id().c_str());
#else
                    LOG(INFO) << "UUpdateUserSeqn update the msg you send to other, msgid is:" << en.cmsg_id();
#endif
                } else {
                    // recv the msg other send to you
#if WEBRTC_ANDROID
                    LOGI("UUpdateUserSeqn update the msg other send to you, seqnkey is:%s\n", sk);
#else
                    LOG(INFO) << "UUpdateUserSeqn update the msg other send ot you, seqnkey is:" << sk;
#endif
                }
                CachedMsgInfo cmi;
                cmi.seqn = itCurSeqn->second;
                cmi.seqnid = m_uid;
                cmi.msgcont = en.SerializeAsString();
                m_uRecvMsgList.push_back(cmi);
                m_uSyncedMsgMap.erase(sk);
                continue;
            } else {
#if WEBRTC_ANDROID
                LOGI("UUpdateUserSeqn break here, itCurSeqn is:%lld\n", itCurSeqn->second);
#else
                LOG(INFO) << "UUpdateUserSeqn break here, itCurSeqn is:" << itCurSeqn->second;
#endif
                break;
            }
        }
        return true;
    }

    // for group
    bool GAddWait4AckMsg(const std::string& msgid, const std::string& strMsg)
    {
        rtc::CritScope cs(&m_csgWait4AckMsg);
        Wait4AckMsgMapIt it = m_gWait4AckMsgMap.find(msgid);
        if (it == m_gWait4AckMsgMap.end())
        {
            m_gWait4AckMsgMap.insert(make_pair(msgid, strMsg));
        }
        return true;
    }

    bool GAddSyncedMsg(const std::string& seqnKey, pms::StorageMsg pmsMsg)
    {
	    rtc::CritScope cs(&m_csgSyncedMsg);
#if WEBRTC_ANDROID
        LOGI("GAddSyncedMsg seqnKey:%s\n", seqnKey.c_str());
#else
        LOG(INFO) << "GAddSyncedMsg seqnKey:" << seqnKey;
#endif
        SyncedMsgMapIt it = m_gSyncedMsgMap.find(seqnKey);
        if (it == m_gSyncedMsgMap.end())
        {
#if WEBRTC_ANDROID
            LOGI("GAddSyncedMsg seqnKey:%s insert into map\n", seqnKey.c_str());
#else
            LOG(INFO) << "GAddSyncedMsg seqnKey:" << seqnKey << " insert into map";
#endif
            m_gSyncedMsgMap.insert(make_pair(seqnKey, pmsMsg));
        }
        return true;
    }

    bool GUpdateUserSeqn(const std::string& storeid)
    {
        // storeid here should be one groupid
        UserSeqnMapIt  itCurSeqn = m_gUserSeqnMap.find(storeid);
#if WEBRTC_ANDROID
        LOGI("GUpdateUserSeqn get here, itCurSeqn is:%lld, storeid:%s, storeid.len:%u\n", itCurSeqn->second, storeid.c_str(), storeid.length());
#else
        LOG(INFO) << "GUpdateUserSeqn get here, itCurSeqn is:" << itCurSeqn->second << ", storeid:" << storeid << ", storeid.len:" << storeid.length();
#endif
        if (itCurSeqn->second<0)
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::GUpdateUserSeqn itCurSeqn is <0, so return\n");
#else
            LOG(INFO) << "XMsgClient::GUpdateUserSeqn itCurSeqn is <0, so return";
#endif
            return false;
        }
        while(1)
        {
#if WEBRTC_ANDROID
            LOGI("GUpdateUserSeqn m_gSyncedMsgMap.size:%u\n", m_gSyncedMsgMap.size());
#else
            LOG(INFO) << "GUpdateUserSeqn m_gSyncedMsgMap.size:" << m_gSyncedMsgMap.size();
#endif
            if (m_gSyncedMsgMap.size()==0) break;
            for (auto &it : m_gSyncedMsgMap)
            {
#if WEBRTC_ANDROID
                LOGI("GUpdateUserSeqn SyncedMsgMap key:%s\n", it.first.c_str());
#else
                LOG(INFO) << "GUpdateUserSeqn SyncedMsgMap:" << it.first;
#endif
            }
            char sk[256] = {0};
            sprintf(sk, "%s:%lld", storeid.c_str(), itCurSeqn->second +1);
            SyncedMsgMapIt it = m_gSyncedMsgMap.find(sk);
            if (it != m_gSyncedMsgMap.end())
            {
                pms::Entity en;
                if (!en.ParseFromString(it->second.content()))
                {
#if WEBRTC_ANDROID
                    LOGI("GUpdateUserSeqn en.ParseFromString error\n");
#else
                    LOG(INFO) << "GUpdateUserSeqn en.ParseFromString error";
#endif
                    m_gSyncedMsgMap.erase(sk);
                    continue;
                }
                // find curSeqn+1 msg, update curSeqn, and callback this msg, and continue
                itCurSeqn->second += 1;
                // in case of groupid or rom_id length is 0
                // and the groupid and rom_id MUST be equal
                // theoretically, this should not be happened
                if (it->second.groupid().length()==0 || en.rom_id().length()==0)
                {
#if WEBRTC_ANDROID
                    LOGI("GUpdateUserSeqn groupid or rom_id is 0, error\n");
#else
                    LOG(INFO) << "GUpdateUserSeqn groupid or rom_id is 0, error";
#endif
                    m_gSyncedMsgMap.erase(sk);
                    continue;
                }
#if WEBRTC_ANDROID
                LOGI("GUpdateUserSeqn update here, itCurSeqn is:%lld, ruserid:%s, fromid:%s, it.groupid:%s, en.romid:%s\n", itCurSeqn->second, it->second.ruserid().c_str(), en.usr_from().c_str(), it->second.groupid().c_str(), en.rom_id().c_str());
#else
                LOG(INFO) << "GUpdateUserSeqn update here, itCurSeqn:" << itCurSeqn->second << ", ruserid:" << it->second.ruserid() << ", fromid:" << en.usr_from() << ", it.groupid:" << it->second.groupid() << ", en.romid:" << en.rom_id();
#endif
                if(it->second.groupid().compare(en.rom_id())!=0)
                {
#if WEBRTC_ANDROID
                    LOGI("XMsgClient::GUpdateUserSeqn groupid and rom_id is not equal, so continue\n");
#else
                    LOG(INFO) << "XMsgClient::GUpdateUserSeqn groupid and rom_id is not equal, so continue";
#endif
                    m_gSyncedMsgMap.erase(sk);
                    continue;
                }
                if (m_uid.compare(en.usr_from())==0)
                {
                    // recv the msg you send to other
                    // erase the msg in Wait4AckMsgMap by entity.cmsg_id
                    m_gWait4AckMsgMap.erase(en.cmsg_id());
#if WEBRTC_ANDROID
                    LOGI("GUpdateUserSeqn update the msg you send to other, msgid is:%s\n", en.cmsg_id().c_str());
#else
                    LOG(INFO) << "GUpdateUserSeqn update the msg you send to other, msgid is:" << en.cmsg_id();
#endif
                } else {
                    // recv the msg other send to you
#if WEBRTC_ANDROID
                    LOGI("GUpdateUserSeqn update the msg other send to you, seqnkey is:%s\n", sk);
#else
                    LOG(INFO) << "GUpdateUserSeqn update the msg other send to you, sendkey is:" << sk;
#endif
                }
                CachedMsgInfo cmi;
                cmi.seqn = itCurSeqn->second;
                cmi.seqnid = it->second.groupid();
                cmi.msgcont = en.SerializeAsString();
                m_gRecvMsgList.push_back(cmi);
                m_gSyncedMsgMap.erase(sk);
                
                continue;
            } else {
#if WEBRTC_ANDROID
                LOGI("GUpdateUserSeqn break here, itCurSeqn is:%lld\n", itCurSeqn->second);
#else
                LOG(INFO) << "GUpdateUserSeqn break here, itCurSeqn is:" << itCurSeqn->second;
#endif
                break;
            }
        }
        return true;
    }
private:

    XMsgCallback*            m_pCallback;
    XTcpClientImpl*          m_pClientImpl;
    XMsgProcesser*           m_pMsgProcesser;
    uint32_t                 m_lastUpdateTime;
    std::string              m_uid;
    std::string              m_token;
    std::string              m_nname;
    std::string              m_uicon;
    std::string              m_server;
    std::string              m_version;
    int                      m_devType;
    int                      m_enablePush;
    int                      m_port;
    bool                     m_autoConnect;
    bool                     m_login;
    MSState                  m_msState;
    pms::EModuleType         m_module;


    Wait4AckMsgMap          m_uWait4AckMsgMap;
    SyncedMsgMap            m_uSyncedMsgMap;
    UserSeqnMap             m_uUserSeqnMap;
    RecvMsgList             m_uRecvMsgList;

    Wait4AckMsgMap          m_gWait4AckMsgMap;
    SyncedMsgMap            m_gSyncedMsgMap;
    UserSeqnMap             m_gUserSeqnMap;
    RecvMsgList             m_gRecvMsgList;

	rtc::CriticalSection	m_csuWait4AckMsg;
	rtc::CriticalSection	m_csuSyncedMsg;
	rtc::CriticalSection	m_csuUserSeqn;
	rtc::CriticalSection	m_csuRecvMsg;

	rtc::CriticalSection	m_csgWait4AckMsg;
	rtc::CriticalSection	m_csgSyncedMsg;
	rtc::CriticalSection	m_csgUserSeqn;
	rtc::CriticalSection	m_csgRecvMsg;


};

#endif /* defined(__dyncRTCMsgClient__XMsgClient__) */
