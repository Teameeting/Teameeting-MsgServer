//
//  MsgClient.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MsgClient_h
#define MsgClientIos_MsgClient_h

#include <map>

#include "MSSubMessage.h"
#include "MSSubMessageDelegate.h"
#include "MSGroupDelegate.h"
#include "MSClientDelegate.h"
#include "MSSqlite3Manager.h"

#include "core/XMsgClient.h"
#include "core/XMsgCallback.h"

#include "proto/entity_msg.pb.h"
#include "proto/entity_msg_type.pb.h"

#include "RTSingleton.h"

#include "webrtc/base/criticalsection.h"
#include "rtklog.h"

class MsgClient : public XMsgClient, public XMsgCallback, public RTSingleton<MsgClient>{
    friend class RTSingleton<MsgClient>;
public:
    int MCInit(const std::string& uid, const std::string& token, const std::string& nname);
    int MCUnin();

    void MCSetTxtMsgDelegate(const MSSubMessageDelegate* subMsgDelegate)
    {
        m_subMsgDelegate = (MSSubMessageDelegate*)subMsgDelegate;
    }

    void MCSetGroupDelegate(const MSGroupDelegate* groupDelegate)
    {
        m_groupDelegate = (MSGroupDelegate*)groupDelegate;
    }

    void MCSetClientDelegate(const MSClientDelegate* clientDelegate)
    {
        m_clientDelegate = (MSClientDelegate*)clientDelegate;
    }

    int MCRegisterMsgCb(XMsgCallback* cb);
    int MCUnRegisterMsgCb(XMsgCallback* cb);
    int MCConnToServer(const std::string& server="", int port=0);

    int MCAddGroup(const std::string& groupid);
    int MCRmvGroup(const std::string& groupid);

    int MCSyncMsg();
    int MCSync2Db();
    int MCSendTxtMsg(std::string& outmsgid, MSTxtMessage* txtMsg);
    int MCSendTxtMsgTos(std::string& outmsgid, MSTxtMessage* txtMsg, const std::vector<std::string>& vusrs);
    int MCSendTxtMsgToUsr(std::string& outmsgid, MSTxtMessage *txtMsg);
    int MCSendTxtMsgToUsrs(std::string& outmsgid, MSTxtMessage *txtMsg, const std::vector<std::string>& vusrs);

    int MCNotifyLive(std::string& outmsgid, MSLivMessage *livMsg);
    int MCNotifyRedEnvelope(std::string& outmsgid, MSRenMessage *renMsg);
    int MCNotifyBlacklist(std::string& outmsgid, MSBlkMessage *blkMsg, const std::vector<std::string>& notifys);
    int MCNotifyForbidden(std::string& outmsgid, MSFbdMessage *fbdMsg, const std::vector<std::string>& notifys);
    int MCNotifySettedMgr(std::string& outmsgid, MSMgrMessage *mgrMsg, const std::vector<std::string>& notifys);

    int MCConnStatus() { return MSStatus(); }
    void MCSetUserId(const std::string& userid) {
        m_nsUserId = userid;
        SetUserId(userid);
    }
    void MCSetToken(const std::string& token) {
        m_nsToken = token;
        SetToken(token);

        int64 setType = 2;// token is 2
        //NSMutableDictionary *dicSetting = [[NSMutableDictionary alloc] init];
        //[dicSetting setValue:m_nsToken forKey:@"token"];
        //NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dicSetting options:NSJSONWritingPrettyPrinted error:nil];
        //NSString *jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];

        //UpdateSetting(setType, [jsonStr cStringUsingEncoding:NSASCIIStringEncoding]);
        //LI("UpdateSetting token jsonStr is:%@", jsonStr);
    }
    void MCSetNickName(const std::string& nickname) {
        m_nsNname = nickname;
        SetNickName(nickname);
    }
    void MCSetUIconUrl(const std::string& uiconurl) {
        m_nsUIcon = uiconurl;
        SetUIconUrl(uiconurl);
    }

    void MCEnablePush(int push) {
        int64 setType = 1;// push is 1

        char val[4] = {0};
        sprintf(val, "%d", push);
        //NSString *nsEnablePush = [NSString stringWithUTF8String:val];

        //NSMutableDictionary *dicSetting = [[NSMutableDictionary alloc] init];
        //[dicSetting setValue:nsEnablePush forKey:@"enablepush"];
        //NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dicSetting options:NSJSONWritingPrettyPrinted error:nil];
        //NSString *jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];

        //SetEnablePush(push);
        //UpdateSetting(setType, [jsonStr cStringUsingEncoding:NSASCIIStringEncoding]);
        //LI("UpdateSetting push jsonStr is:%@", jsonStr);
    }

    void MCMuteNotify(int mute) {
        int64 setType = 1;// mute is 4

        char val[4] = {0};
        sprintf(val, "%d", mute);
        //NSString *nsMuteNotify = [NSString stringWithUTF8String:val];

        //NSMutableDictionary *dicSetting = [[NSMutableDictionary alloc] init];
        //[dicSetting setValue:nsMuteNotify forKey:@"mutenotify"];
        //NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dicSetting options:NSJSONWritingPrettyPrinted error:nil];
        //NSString *jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];

        //SetMuteNotify(mute);
        //UpdateSetting(setType, [jsonStr cStringUsingEncoding:NSASCIIStringEncoding]);
        //LI("UpdateSetting mute jsonStr is:%@", jsonStr);
    }

    std::string& MCGetUserId() { return m_nsUserId; }
    std::string& MCGetToken() { return m_nsToken; }
    std::string& MCGetNickName() { return m_nsNname; }
    std::string& MCGetUIconUrl() { return m_nsUIcon; }

//for XMsgCallback
public:
    virtual void OnSndMsg(int code, const std::string& msgid);
    virtual void OnCmdCallback(int code, int cmd, const std::string& groupid, const MSCbData& data);
    virtual void OnRecvMsg(int64 seqn, const std::string& msg);
    virtual void OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg);

    virtual void OnSyncSeqn(int64 seqn, int role);
    virtual void OnSyncGroupSeqn(const std::string& groupid, int64 seqn);

    virtual void OnGroupNotify(int code, const std::string& seqnid);
    virtual void OnNotifySeqn(int code, const std::string& seqnid);
    virtual void OnNotifyData(int code, const std::string& seqnid);
    virtual void OnNotifyOtherLogin(int code);

    virtual void OnMsgServerConnected();
    virtual void OnMsgServerConnecting();
    virtual void OnMsgServerDisconnect();
    virtual void OnMsgServerConnectionFailure();

private:

    void ProcessFetchResult()
    {
        if (IsFetchedAll())
        {
            // stop timer here
            m_isFetched = true;
            m_clientDelegate->OnMsgClientInitialized();
        } else {
            FetchAllSeqns();
        }
    }

    bool IsFetchedAll()
    {
        bool yes = true;
        for (DbFieldMap item : m_nsGroupInfo) {
            std::string key1("seqnId");
            std::string key2("isfetched");
            std::string seqnId = item.find(key1)->second;
            std::string isfetched = item.find(key2)->second;
            LI("IsFetchedAll seqnid:%s, isfetched:%s\n", seqnId.c_str(), isfetched.c_str());
            if (isfetched.compare("0")==0)
            {
                yes = false;
                break;
            } else {
                continue;
            }
        }
        return yes;
    }

    void FetchAllSeqns()
    {
        for (DbFieldMap item : m_nsGroupInfo) {
            std::string key1("seqnId");
            std::string key2("isfetched");
            std::string seqnId = item.find(key1)->second;
            std::string isfetched = item.find(key2)->second;
            LI("FetchAllSeqns seqnid:%s, isfetched:%s\n", seqnId.c_str(), isfetched.c_str());
            if (isfetched.compare("0")==0)
            {
                if (seqnId.compare(m_nsUserId)==0)
                {
                    FetchSeqn();
                } else {
                    FetchGroupSeqn(seqnId);
                }
            }
        }
    }

    void SyncAllSeqns()
    {
        for (DbFieldMap item : m_nsGroupInfo) {
            std::string key1("seqnId");
            std::string key2("seqn");
            std::string seqnId = item.find(key1)->second;
            std::string seqn = item.find(key2)->second;
            LI("SyncAllSeqns seqnid:%s, seqn:%s", seqnId.c_str(), seqn.c_str());
            if (seqnId.compare(m_nsUserId)==0)
            {
                SyncSeqn(std::atoll(seqn.c_str()), pms::EMsgRole::RSENDER);
            } else {
                SyncGroupSeqn(seqnId, std::atoll(seqn.c_str()), pms::EMsgRole::RSENDER);
            }
        }
    }

    void FetchUserSeqn()
    {
        FetchSeqn();
    }

    void FetchGroupSeqn(const std::string& groupid)
    {
        FetchGroupSeqn(groupid);
    }

    void UpdateGroupInfoToDb(const std::string& nsSeqnId, ::google::protobuf::int64 nsSeqn, int nsIsFetched)
    {
        LI("UpdateGroupInfoToDb nsSeqnId:%s, nsSeqn:%lld, nsIsFetched:%d", nsSeqnId.c_str(), nsSeqn, nsIsFetched);
        // update NSMutableArray
        DbResultVectorIt it;
        for (it=m_nsGroupInfo.begin();it!=m_nsGroupInfo.end();it++)
        {
            DbFieldMap map = *it;
            std::string key1("seqnId");
            DbFieldMapIt mapIt = map.find(key1);
            if (mapIt!=map.end())
            {
                 if (mapIt->second.compare(nsSeqnId)==0)
                 {
                     std::string strSeqn = std::to_string(nsSeqn);
                     std::string strIsFetched = std::to_string(nsIsFetched);
                    it->insert(std::make_pair("seqn", strSeqn));
                    it->insert(std::make_pair("isfetched", strIsFetched));
                 }
            }
        }

        //GroupSeqnMap
        UpdateLocalSeqn(nsSeqnId, nsSeqn);

        // update Database
        m_sqlite3Manager->UpdateGroupInfo(m_nsUserId, nsSeqnId, nsSeqn, nsIsFetched);
    }

    void CheckUserOrInit(const std::string& nsUid)
    {
        if (!m_sqlite3Manager->IsUserExists(nsUid, nsUid))
        {
            m_sqlite3Manager->AddUser(nsUid);
        }
    }

    void GetLocalSeqnsFromDb()
    {
        if (m_sqlite3Manager)
        {
            DbResultVector info;
            m_sqlite3Manager->GetGroupInfo(m_nsUserId, info);
            for (DbFieldMap item : info) {
                std::string key1("seqnId");
                std::string key2("seqn");
                std::string key3("isfetched");
                std::string seqnId = item.find(key1)->second;
                std::string seqn = item.find(key2)->second;
                std::string isfetched = item.find(key3)->second;
                LI("get group info seqnid:%s, seqn:%s, isfetch:%s\n", seqnId.c_str(), seqn.c_str(), isfetched.c_str());
                //assert([isfetched intValue]);
                m_groupSeqn.insert(make_pair(seqnId, std::atoll(seqn.c_str())));
                m_nsGroupInfo.push_back(item);
            }
        }
    }

    void PutLocalSeqnsToDb()
    {
        LI("PutLocalSeqnsToDb was called\n");
        if (m_sqlite3Manager)
        {
            LI("updateGroupSeqnUserId will be call...\n");
            for (auto &item : m_groupSeqn)
            {
                m_sqlite3Manager->UpdateGroupSeqn(m_nsUserId, item.first, item.second);
            }
        }
    }

    void UpdateLocalSeqn(const std::string& seqnId, int64_t seqn)
    {
        //[m_recurLock lock];
        GroupSeqnMapIt it = m_groupSeqn.find(seqnId);
        if (it != m_groupSeqn.end())
        {
            printf("UpdateLocalSeqn call., find seqnid:%s, seqn:%lld, befreo seqn:%lld\n", seqnId.c_str(), seqn, it->second);
            if (seqn > it->second)
                it->second = seqn;
        } else {
            printf("UpdateLocalSeqn call., not find seqnid:%s, seqn:%lld so insert\n", seqnId.c_str(), seqn);
            m_groupSeqn.insert(make_pair(seqnId, seqn));
        }
        //[m_recurLock unlock];
    }

    void UpdateSeqnToDb(const std::string& seqnid, int64_t seqn)
    {
        if (m_sqlite3Manager)
        {
            m_sqlite3Manager->UpdateGroupSeqn(m_nsUserId, seqnid, seqn);
        }
    }

    void RemoveLocalSeqn(const std::string& seqnId)
    {
        //[m_recurLock lock];
        m_groupSeqn.erase(seqnId);
        //[m_recurLock unlock];
    }

    int64 GetLocalSeqnFromId(const std::string& seqnId)
    {
        int64 lseqn = -1;
        {
            //[m_recurLock lock];
            GroupSeqnMapIt it = m_groupSeqn.find(seqnId);
            if (it != m_groupSeqn.end())
            {
                printf("UpdateLocalSeqn call., find seqnid:%s, seqn:%lld\n", seqnId.c_str(), it->second);
                lseqn = it->second;
            } else {
                printf("UpdateLocalSeqn call., not find seqnid:%s\n", seqnId.c_str());
            }
            //[m_recurLock unlock];
        }
        return lseqn;
    }

    void SyncSeqnFromDb2Core()
    {
        for (auto &item : m_groupSeqn)
        {
            LI("SyncSeqnFromDb2Core will call InitUserSeqns...\n");
            InitUserSeqns(item.first, item.second);
        }
    }

    void UpdateSeqnFromDb2Core()
    {
        for (auto &item : m_groupSeqn)
        {
            LI("UpdateSeqnFromDb2Core will call UpdateUserSeqns...\n");
            UpdateUserSeqns(item.first, item.second);
        }
    }

protected:
    MsgClient():m_subMsgDelegate(nullptr)
    , m_groupDelegate(nullptr)
    , m_clientDelegate(nullptr)
    , m_sqlite3Manager(nullptr){}
    ~MsgClient(){}

protected:
    typedef std::map<std::string, int64_t>      GroupSeqnMap;
    typedef GroupSeqnMap::iterator          GroupSeqnMapIt;

private:
    MSSubMessageDelegate*       m_subMsgDelegate;
    MSGroupDelegate*            m_groupDelegate;
    MSClientDelegate*           m_clientDelegate;

    MSSqlite3Manager*           m_sqlite3Manager;

    std::string                 m_nsUserId;
    std::string                 m_nsToken;
    std::string                 m_nsNname;
    std::string                 m_nsUIcon;

    DbResultVector              m_nsGroupInfo;

    std::string                 m_strUserId;
    std::string                 m_strToken;
    std::string                 m_strNname;;

    bool                        m_isFetched;
    GroupSeqnMap                m_groupSeqn;
    //NSRecursiveLock*            m_recurLock;
    rtc::CriticalSection        m_cs;

};

#endif
