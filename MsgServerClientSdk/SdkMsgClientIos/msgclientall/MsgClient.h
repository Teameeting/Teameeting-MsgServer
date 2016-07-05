//
//  MsgClient.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MsgClient_h
#define MsgClientIos_MsgClient_h

#import <Foundation/Foundation.h>
#import "MSTxtMessageDelegate.h"
#import "MSGroupDelegate.h"
#import "MSClientDelegate.h"
#import "MSSqlite3Manager.h"

#include <map>
#include "XMsgClient.h"
#include "XMsgCallback.h"

#include "RTSingleton.h"

class MsgClient : public XMsgClient, public XMsgCallback, public RTSingleton<MsgClient>{
    friend class RTSingleton<MsgClient>;
public:
    int MCInit(const std::string& uid, const std::string& token, const std::string& nname);
    int MCUnin();
    
    void MCSetTxtMsgDelegate(id<MSTxtMessageDelegate> txtMsgDelegate)
    {
        m_txtMsgDelegate = txtMsgDelegate;
    }
    
    void MCSetGroupDelegate(id<MSGroupDelegate> groupDelegate)
    {
        m_groupDelegate = groupDelegate;
    }
    
    void MCSetClientDelegate(id<MSClientDelegate> clientDelegate)
    {
        m_clientDelegate = clientDelegate;
    }
    
    int MCRegisterMsgCb(XMsgCallback* cb);
    int MCUnRegisterMsgCb(XMsgCallback* cb);
    int MCConnToServer(const std::string& server="", int port=0);
    
    int MCAddGroup(const std::string& groupid);
    int MCRmvGroup(const std::string& groupid);
    
    int MCSyncMsg();
    int MCSendTxtMsg(std::string& outmsgid, const std::string& groupid, const std::string& content);
    int MCSendTxtMsgToUsr(std::string& outmsgid, const std::string& userid, const std::string& content);
    int MCSendTxtMsgToUsrs(std::string& outmsgid, const std::vector<std::string>& vusrs, const std::string& content);
    
    int MCNotifyLive(std::string& outmsgid, const std::string& groupid, const std::string& hostid);
    int MCNotifyRedEnvelope(std::string& outmsgid, const std::string& groupid, const std::string& hostid);
    int MCNotifyBlacklist(std::string& outmsgid, const std::string& groupid, const std::string& userid);
    int MCNotifyForbidden(std::string& outmsgid, const std::string& groupid, const std::string& userid);
    int MCNotifySettedMgr(std::string& outmsgid, const std::string& groupid, const std::string& userid);
    
    int MCConnStatus() { return MSStatus(); }
    void MCSetNickName(const std::string& nickname) { SetNickName(nickname); }
    
//for XMsgCallback
public:
    virtual void OnSndMsg(int code, const std::string& msgid);
    virtual void OnCmdGroup(int code, int cmd, const std::string& groupid, const MSCbData& data);
    virtual void OnRecvMsg(int64 seqn, const std::string& msg);
    virtual void OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg);
    
    virtual void OnSyncSeqn(int64 seqn, int role);
    virtual void OnSyncGroupSeqn(const std::string& groupid, int64 seqn);
    
    virtual void OnGroupNotify(int code, const std::string& seqnid);
    virtual void OnNotifySeqn(int code, const std::string& seqnid);
    virtual void OnNotifyData(int code, const std::string& seqnid);
    
    virtual void OnMsgServerConnected();
    virtual void OnMsgServerConnecting();
    virtual void OnMsgServerDisconnect();
    virtual void OnMsgServerConnectionFailure();
    
protected:
    void CheckUserOrInit(NSString* nsUid)
    {
        if (m_sqlite3Manager)
        {
            if (![m_sqlite3Manager isUserExists:nsUid])
            {
                [m_sqlite3Manager addUserId:nsUid];
            }
        }
    }
    
    void GetLocalSeqnsFromDb()
    {
        if (m_sqlite3Manager)
        {
            m_userSeqn = [[m_sqlite3Manager getUserSeqnUserId:m_nsUserId] longLongValue];
            NSArray *arr = [m_sqlite3Manager getGroupIdSeqns];
            if (!arr) {
                NSLog(@"m_sqllite3Manager getGroupIds is nil");
                return;
            }
            for (NSDictionary *item in arr) {
                NSString *key = [[item allKeys] firstObject];
                NSNumber *value = [item objectForKey:key];
                m_groupSeqn.insert(make_pair([key cStringUsingEncoding:NSUTF8StringEncoding], [value longLongValue]));
            }
        }
    }
    
    void PutLocalSeqnsToDb()
    {
        NSLog(@"PutLocalSeqnsToDb was called");
        if (m_sqlite3Manager)
        {
            NSLog(@"updateUserSeqnUserId will be call...");
            [m_sqlite3Manager updateUserSeqnUserId:m_nsUserId seqn:[NSNumber numberWithLongLong:m_userSeqn]];
            for (auto &item : m_groupSeqn)
            {
                NSLog(@"updateGroupSeqnGrpId will be call...");
                [m_sqlite3Manager updateGroupSeqnGrpId:[NSString stringWithUTF8String:item.first.c_str()] seqn:[NSNumber numberWithLongLong:item.second]];
            }
        }
    }
    
    void UpdateLocalSeqn(const std::string& seqnId, int64_t seqn)
    {
        [m_recurLock lock];
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
        [m_recurLock unlock];
    }
    
    void RemoveLocalSeqn(const std::string& seqnId)
    {
        [m_recurLock lock];
        m_groupSeqn.erase(seqnId);
        [m_recurLock unlock];
    }
    
    int64 GetLocalSeqnFromId(const std::string& seqnId)
    {
        int64 lseqn = -1;
        {
            [m_recurLock lock];
            GroupSeqnMapIt it = m_groupSeqn.find(seqnId);
            if (it != m_groupSeqn.end())
            {
                printf("UpdateLocalSeqn call., find seqnid:%s, seqn:%lld\n", seqnId.c_str(), it->second);
                lseqn = it->second;
            } else {
                printf("UpdateLocalSeqn call., not find seqnid:%s\n", seqnId.c_str());
            }
            [m_recurLock unlock];    
        }
        return lseqn;
    }
    
protected:
    MsgClient():m_txtMsgDelegate(nullptr)
    , m_groupDelegate(nullptr)
    , m_clientDelegate(nullptr)
    , m_sqlite3Manager(nullptr){}
    ~MsgClient(){}
    
protected:
    typedef std::map<std::string, int64_t>      GroupSeqnMap;
    typedef GroupSeqnMap::iterator          GroupSeqnMapIt;
    
private:
    id<MSTxtMessageDelegate>    m_txtMsgDelegate;
    id<MSGroupDelegate>         m_groupDelegate;
    id<MSClientDelegate>        m_clientDelegate;
    
    MSSqlite3Manager*           m_sqlite3Manager;
    
    NSString*                   m_nsUserId;
    NSString*                   m_nsToken;
    NSString*                   m_nsNname;
    
    std::string                 m_strUserId;
    std::string                 m_strToken;
    std::string                 m_strNname;;
    
    int64_t                     m_userSeqn;
    GroupSeqnMap                m_groupSeqn;
    NSRecursiveLock*            m_recurLock;
    
};
#endif
