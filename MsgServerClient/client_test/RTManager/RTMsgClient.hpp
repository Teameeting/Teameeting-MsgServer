//
//  RTMsgClient.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/24/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTMsgClient_hpp
#define RTMsgClient_hpp

#include <stdio.h>
#include <unordered_set>
#include "core/XMsgClient.h"
#include "core/XMsgCallback.h"
#include "RTMsgCommon.h"

#include "RTHttpClient.hpp"
#include "XGrpMsgClient.h"

class RTMsgClient : public XMsgCallback{
public:
    RTMsgClient(const std::string& uid);
    virtual ~RTMsgClient();
public:

    typedef void (*DataCallback)(void* pd, int data);

    // from XMsgCallback
    virtual void OnSndMsg(int code, const std::string& msg);
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

public:
    void SetRoomId(const std::string& roomid) { mCurRoomId = roomid; }
    int Register();
    int ApplyRoom();

    void SetPData(void* pd) { mPData = pd; }
    void SetDataCallback(DataCallback cb) { mDataCallback = cb; }

    void Init(int module);
    void Unin();
    bool Connecting();
    void SendMsg(const std::string& msg);

    void InitSync();

    void SyncSeqn();
    void SyncData();
    void SendMessage(const std::string& msg);
    void SendMessageTo(const std::string& msg, const std::string& name);


    void AddGroup();


    // for group
    void GrpInit(int module);
    void GrpUnin();
    bool GrpConnecting();
    void GrpInitSync();
    void GrpSyncGroupData(const std::string& userid, const std::string groupid, int64 curseqn);
    void CreateGroupSeqn();
    void DeleteGroupSeqn();

public:
    const std::string& GetUserId() { return mUserid; }
    int GetRecvNums() { return mRecvNum; }
    int GetConnNums() { return mConnNum; }
    int GetDisconnNums() { return mDisconnNum; }
    int GetConnFailNums() { return mConnFailNum; }
    void ShowRecvMsg();
private:
    XMsgClient      mMsgClient;
    XGrpMsgClient   mGrpMsgClient;
    RTHttpClient    mHttpClient;
    std::string     mUserid;
    std::string     mAuth;
    std::string     mUname;
    std::string     mPushToken;
    std::string     mMsgServer;
    std::string     mCurRoomId;
    int             mMsgPort;
    std::unordered_set<std::string>     mRoomSet;
    std::list<std::string>              mMsgList;

    bool            mIsOnline;
    int             mRecvNum;
    int             mConnNum;
    int             mDisconnNum;
    int             mConnFailNum;
    pthread_cond_t          mCond;
    pthread_mutex_t         mMutex;

    void*                   mPData;
    DataCallback            mDataCallback;

};

#endif /* RTMsgClient_hpp */
