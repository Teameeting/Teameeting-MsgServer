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
#import <map>

#import "msgclient/MSSubMessage.h"
#import "msgclient/MSSubMessageDelegate.h"
#import "msgclient/MSGroupDelegate.h"
#import "msgclient/MSClientDelegate.h"
#import "msgclient/MSSqlite3Manager.h"

#include "msgclient/client_common/core/XMsgClient.h"
#include "msgclient/client_common/core/XMsgCallback.h"

#include "msgclient/client_common/proto/entity_msg.pb.h"
#include "msgclient/client_common/proto/entity_msg_type.pb.h"

#include "msgclient/RTSingleton.h"

class MsgClient : public XMsgClient, public XMsgCallback, public RTSingleton<MsgClient>{
    friend class RTSingleton<MsgClient>;
public:
    int MCInit(const std::string& uid, const std::string& token, const std::string& nname);
    int MCUnin();
    
    void MCSetTxtMsgDelegate(id<MSSubMessageDelegate> subMsgDelegate)
    {
        m_subMsgDelegate = subMsgDelegate;
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
        m_nsUserId = [NSString stringWithCString:userid.c_str() encoding:NSUTF8StringEncoding];
        SetUserId(userid);
    }
    void MCSetToken(const std::string& token) {
        m_nsToken = [NSString stringWithCString:token.c_str() encoding:NSUTF8StringEncoding];
        SetToken(token);
        
        int64 setType = 2;// token is 2
        NSMutableDictionary *dicSetting = [[NSMutableDictionary alloc] init];
        [dicSetting setValue:m_nsToken forKey:@"token"];
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dicSetting options:NSJSONWritingPrettyPrinted error:nil];
        NSString *jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        
        UpdateSetting(setType, [jsonStr cStringUsingEncoding:NSASCIIStringEncoding]);
        NSLog(@"UpdateSetting token jsonStr is:%@", jsonStr);
    }
    void MCSetNickName(const std::string& nickname) {
        m_nsNname = [NSString stringWithCString:nickname.c_str() encoding:NSUTF8StringEncoding];
        SetNickName(nickname);
    }
    void MCSetUIconUrl(const std::string& uiconurl) {
        m_nsUIcon = [NSString stringWithCString:uiconurl.c_str() encoding:NSUTF8StringEncoding];
        SetUIconUrl(uiconurl);
    }
    
    void MCEnablePush(int push) {
        int64 setType = 1;// push is 1
        
        char val[4] = {0};
        sprintf(val, "%d", push);
        NSString *nsEnablePush = [NSString stringWithUTF8String:val];
        
        NSMutableDictionary *dicSetting = [[NSMutableDictionary alloc] init];
        [dicSetting setValue:nsEnablePush forKey:@"enablepush"];
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dicSetting options:NSJSONWritingPrettyPrinted error:nil];
        NSString *jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        
        SetEnablePush(push);
        UpdateSetting(setType, [jsonStr cStringUsingEncoding:NSASCIIStringEncoding]);
        NSLog(@"UpdateSetting push jsonStr is:%@", jsonStr);
    }
    
    void MCMuteNotify(int mute) {
        int64 setType = 1;// mute is 4
        
        char val[4] = {0};
        sprintf(val, "%d", mute);
        NSString *nsMuteNotify = [NSString stringWithUTF8String:val];
        
        NSMutableDictionary *dicSetting = [[NSMutableDictionary alloc] init];
        [dicSetting setValue:nsMuteNotify forKey:@"mutenotify"];
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dicSetting options:NSJSONWritingPrettyPrinted error:nil];
        NSString *jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        
        SetMuteNotify(mute);
        UpdateSetting(setType, [jsonStr cStringUsingEncoding:NSASCIIStringEncoding]);
        NSLog(@"UpdateSetting mute jsonStr is:%@", jsonStr);
    }
    
    NSString* MCGetNsUserId() { return m_nsUserId; }
    NSString* MCGetNsToken() { return m_nsToken; }
    NSString* MCGetNsNickName() { return m_nsNname; }
    NSString* MCGetNsUIconUrl() { return m_nsUIcon; }
    
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
            [m_clientDelegate OnMsgClientInitialized];
        } else {
            FetchAllSeqns();
        }
    }
    
    bool IsFetchedAll()
    {
        bool yes = true;
        for (NSMutableDictionary *item in m_nsGroupInfo) {
            NSString *seqnId = [item objectForKey:@"seqnId"];
            NSNumber *isfetched = [item objectForKey:@"isfetched"];
            NSLog(@"IsFetchedAll seqnid:%@, isfetched:%d", seqnId, [isfetched intValue]);
            if ([isfetched intValue]==0)
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
        for (NSMutableDictionary *item in m_nsGroupInfo) {
            NSString *seqnId = [item objectForKey:@"seqnId"];
            NSNumber *isfetched = [item objectForKey:@"isfetched"];
            NSLog(@"FetchAllSeqns seqnid:%@, isfetched:%d", seqnId, [isfetched intValue]);
            if ([isfetched intValue]==0)
            {
                if ([seqnId compare:m_nsUserId]==0)
                {
                    FetchSeqn();
                } else {
                    FetchGroupSeqn([seqnId cStringUsingEncoding:NSASCIIStringEncoding]);
                }
            }
        }
    }
    
    void SyncAllSeqns()
    {
        for (NSMutableDictionary *item in m_nsGroupInfo) {
            NSString *seqnId = [item objectForKey:@"seqnId"];
            NSNumber *seqn = [item objectForKey:@"seqn"];
            NSLog(@"SyncAllSeqns seqnid:%@, isfetched:%lld", seqnId, [seqn longLongValue]);
            if ([seqnId compare:m_nsUserId]==0)
            {
                SyncSeqn([seqn longLongValue], pms::EMsgRole::RSENDER);
            } else {
                SyncGroupSeqn([seqnId cStringUsingEncoding:NSASCIIStringEncoding], [seqn longLongValue], pms::EMsgRole::RSENDER);
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
    
    void UpdateGroupInfoToDb(NSString* nsSeqnId, NSNumber* nsSeqn, NSNumber* nsIsFetched)
    {
        NSLog(@"UpdateGroupInfoToDb nsSeqnId:%@, nsSeqn:%lld, nsIsFetched:%d", nsSeqnId, [nsSeqn longLongValue], [nsIsFetched intValue]);
        // update NSMutableArray
        for (int i=0;i < [m_nsGroupInfo count];i++)
        {
            NSMutableDictionary *it = [m_nsGroupInfo objectAtIndex:i];
            if (it && [[it objectForKey:@"seqnId"] compare:nsSeqnId]==0)
            {
                [it setObject:nsSeqn forKey:@"seqn"];
                [it setObject:nsIsFetched forKey:@"isfetched"];
            }
        }
        
        //GroupSeqnMap
        UpdateLocalSeqn([nsSeqnId cStringUsingEncoding:NSASCIIStringEncoding], [nsSeqn longLongValue]);
        
        // update Database
        [m_sqlite3Manager updateGroupInfoUserId:m_nsUserId GrpId:nsSeqnId seqn:nsSeqn isfetched:nsIsFetched];
    }
    
    void CheckUserOrInit(NSString* nsUid)
    {
        if (![m_sqlite3Manager isUserExistsUserId:nsUid seqnId:nsUid])
        {
            [m_sqlite3Manager addUserId:nsUid];
        }
    }
    
    void GetLocalSeqnsFromDb()
    {
        if (m_sqlite3Manager)
        {
            //m_userSeqn = [[m_sqlite3Manager getUserSeqnUserId:m_nsUserId] longLongValue];
            NSArray *arr = [m_sqlite3Manager getGroupInfoUserId:m_nsUserId];
            if (!arr) {
                NSLog(@"m_sqllite3Manager getGroupIds is nil");
                return;
            }
            for (NSMutableDictionary *item in arr) {
                NSString *seqnId = [item objectForKey:@"seqnId"];
                NSNumber *seqn = [item objectForKey:@"seqn"];
                NSNumber *isfetched = [item objectForKey:@"isfetched"];
                NSLog(@"get group info seqnid:%@, seqn:%lld, isfetch:%d", seqnId, [seqn longLongValue], [isfetched intValue]);
                //assert([isfetched intValue]);
                m_groupSeqn.insert(make_pair([seqnId cStringUsingEncoding:NSUTF8StringEncoding], [seqn longLongValue]));
                [m_nsGroupInfo addObject:item];
            }
        }
    }
    
    void PutLocalSeqnsToDb()
    {
        NSLog(@"PutLocalSeqnsToDb was called");
        if (m_sqlite3Manager)
        {
            NSLog(@"updateGroupSeqnUserId will be call...");
            for (auto &item : m_groupSeqn)
            {
                NSLog(@"updateGroupSeqnGrpId will be call...");
                [m_sqlite3Manager updateGroupSeqnUserId:m_nsUserId GrpId:[NSString stringWithUTF8String:item.first.c_str()] seqn:[NSNumber numberWithLongLong:item.second]];
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
    
    void UpdateSeqnToDb(const std::string& seqnid, int64_t seqn)
    {
        if (m_sqlite3Manager)
        {
            [m_sqlite3Manager updateGroupSeqnUserId:m_nsUserId GrpId:[NSString stringWithUTF8String:seqnid.c_str()] seqn:[NSNumber numberWithLongLong:seqn]];
        }
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
    
    void SyncSeqnFromDb2Core()
    {
        for (auto &item : m_groupSeqn)
        {
            NSLog(@"SyncSeqnFromDb2Core will call InitUserSeqns...");
            InitUserSeqns(item.first, item.second);
        }
    }
    
    void UpdateSeqnFromDb2Core()
    {
        for (auto &item : m_groupSeqn)
        {
            NSLog(@"UpdateSeqnFromDb2Core will call UpdateUserSeqns...");
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
    id<MSSubMessageDelegate>    m_subMsgDelegate;
    id<MSGroupDelegate>         m_groupDelegate;
    id<MSClientDelegate>        m_clientDelegate;
    
    MSSqlite3Manager*           m_sqlite3Manager;
    
    NSString*                   m_nsUserId;
    NSString*                   m_nsToken;
    NSString*                   m_nsNname;
    NSString*                   m_nsUIcon;
    
    NSMutableArray*             m_nsGroupInfo;
    
    std::string                 m_strUserId;
    std::string                 m_strToken;
    std::string                 m_strNname;;
    
    bool                        m_isFetched;
    GroupSeqnMap                m_groupSeqn;
    NSRecursiveLock*            m_recurLock;
    
};
#endif
