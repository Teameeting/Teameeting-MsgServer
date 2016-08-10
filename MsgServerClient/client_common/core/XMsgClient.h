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

    int Init(const std::string& uid, const std::string& token, const std::string& nname, int module);
    int Unin();

    int RegisterMsgCb(XMsgCallback* cb);
    int UnRegisterMsgCb(XMsgCallback* cb);

    int ConnToServer(const std::string& server="", int port=0, bool bAutoConnect=true);
    
    int AddGroup(const std::string& groupid);
    int RmvGroup(const std::string& groupid);

    int SndMsg(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag);
    int SndMsgTo(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, const std::vector<std::string>& uvec);
    
    int FetchSeqn();
    int SyncSeqn(int64 seqn, int role);
    int SyncData(int64 seqn);
    int FetchGroupSeqn(const std::string& groupid);
    int SyncGroupSeqn(const std::string& groupid, int64 seqn, int role);
    int SyncGroupData(const std::string& gropuid, int64 seqn);

    MSState MSStatus() { return m_msState; }
    void SetUserId(const std::string& userid) { m_uid = userid; }
    void SetToken(const std::string& token) { m_token = token; }
    void SetNickName(const std::string& nickname) { m_nname = nickname; }

    void SetUIconUrl(const std::string& uiconurl) { m_uicon = uiconurl; }

    void InitUserSeqns(const std::string& seqnid, int64 seqn)
    {
        printf("InitUserSeqns seqnid:%s, seqn:%lld\n", seqnid.c_str(), seqn);
        if (seqnid.compare(m_uid)==0)
        {
            m_uUserSeqnMap.insert(make_pair(seqnid, seqn));
        } else {
            m_gUserSeqnMap.insert(make_pair(seqnid, seqn));
        }
    }

    void UpdateUserSeqns(const std::string& seqnid, int64 seqn)
    {
        printf("UpdateUserSeqns seqnid:%s, seqn:%lld\n", seqnid.c_str(), seqn);
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

    //<msgid, str_msg>
    typedef std::unordered_map<std::string, std::string>        Wait4AckMsgMap;
    typedef Wait4AckMsgMap::iterator                            Wait4AckMsgMapIt;

    //<seqnkey, storage_msg>
    typedef std::unordered_map<std::string, pms::StorageMsg>    SyncedMsgMap;
    typedef SyncedMsgMap::iterator                              SyncedMsgMapIt;

    //<seqnid, seqn>
    typedef std::unordered_map<std::string, int64>              UserSeqnMap;
    typedef UserSeqnMap::iterator                          UserSeqnMapIt;

    //<StorageMsg>
    typedef std::list<pms::StorageMsg>                          RecvMsgList;
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
        printf("UUpdateUserSeqn get here, itCurSeqn is:%lld\n", itCurSeqn->second);
        assert(itCurSeqn->second>0);
        while(1)
        {
            printf("UUpdateUserSeqn m_gSyncedMsgMap.size:%lu\n", m_gSyncedMsgMap.size());
            if (m_uSyncedMsgMap.size()==0) break;
            char sk[256] = {0};
            sprintf(sk, "%s:%lld", m_uid.c_str(), itCurSeqn->second +1);
            SyncedMsgMapIt it = m_uSyncedMsgMap.find(sk);
            if (it != m_uSyncedMsgMap.end())
            {
                // find curSeqn+1 msg, update curSeqn, and callback this msg, and continue
                itCurSeqn->second += 1;
                printf("UUpdateUserSeqn update here, itCurSeqn is:%lld\n", itCurSeqn->second);
                if (m_uid.compare(it->second.ruserid())==0)
                {
                    // recv the msg you send to other
                    // erase the msg in Wait4AckMsgMap by msgid
                    m_uWait4AckMsgMap.erase(it->second.msgid());
                    printf("UUpdateUserSeqn update the msg you send to other, msgid is:%s\n", it->second.msgid().c_str());
                } else {
                    // recv the msg other send to you
                    m_uRecvMsgList.push_back(it->second);
                    m_uSyncedMsgMap.erase(sk);
                    printf("UUpdateUserSeqn update the msg other send to you, seqnkey is:%s\n", sk);
                }
                continue;
            } else {
                printf("UUpdateUserSeqn break here, itCurSeqn is:%lld\n", itCurSeqn->second);
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
        SyncedMsgMapIt it = m_gSyncedMsgMap.find(seqnKey);
        if (it == m_gSyncedMsgMap.end())
        {
            m_gSyncedMsgMap.insert(make_pair(seqnKey, pmsMsg));
        }
        return true;
    }

    bool GUpdateUserSeqn(const std::string& storeid)
    {
        // storeid here should be one groupid
        UserSeqnMapIt  itCurSeqn = m_gUserSeqnMap.find(storeid);
        printf("GUpdateUserSeqn get here, itCurSeqn is:%lld\n", itCurSeqn->second);
        assert(itCurSeqn->second >0);
        while(1)
        {
            printf("GUpdateUserSeqn m_gSyncedMsgMap.size:%lu\n", m_gSyncedMsgMap.size());
            if (m_gSyncedMsgMap.size()==0) break;
            char sk[256] = {0};
            sprintf(sk, "%s:%lld", storeid.c_str(), itCurSeqn->second +1);
            SyncedMsgMapIt it = m_gSyncedMsgMap.find(sk);
            if (it != m_gSyncedMsgMap.end())
            {
                // find curSeqn+1 msg, update curSeqn, and callback this msg, and continue
                itCurSeqn->second += 1;
                printf("GUpdateUserSeqn update here, itCurSeqn is:%lld\n", itCurSeqn->second);
                if (m_uid.compare(it->second.ruserid())==0)
                {
                    // recv the msg you send to other
                    // erase the msg in Wait4AckMsgMap by msgid
                    m_gWait4AckMsgMap.erase(it->second.msgid());
                    printf("GUpdateUserSeqn update the msg you send to other, msgid is:%s\n", it->second.msgid().c_str());
                } else {
                    // recv the msg other send to you
                    m_gRecvMsgList.push_back(it->second);
                    m_gSyncedMsgMap.erase(sk);
                    printf("GUpdateUserSeqn update the msg other send to you, seqnkey is:%s\n", sk);
                }
                continue;
            } else {
                printf("GUpdateUserSeqn break here, itCurSeqn is:%lld\n", itCurSeqn->second);
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
