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
#include "XMsgClient.h"
#include "XMsgCallback.h"
#include "RTHttpClient.hpp"

class RTMsgClient : public XMsgCallback{
public:
    RTMsgClient(const std::string& uid);
    virtual ~RTMsgClient();
public:
    // from XMsgCallback
    virtual void OnSndMsg(const std::string& msg);
    virtual void OnGetMsg(const std::string& msg);
    virtual void OnMsgServerConnected();
    virtual void OnMsgServerDisconnect();
    virtual void OnMsgServerConnectionFailure();
    virtual void OnMsgServerState(MSState state);
    
public:
    void SetRoomId(const std::string& roomid) { mCurRoomId = roomid; }
    int Register();
    int ApplyRoom();
    
    void Init();
    void Unin();
    bool Connecting();
    void EnterRoom();
    void LeaveRoom();
    void SendMsg(const std::string& msg);
public:
    const std::string& GetUserId() { return mUserid; }
    int GetRecvNums() { return mRecvNum; }
    int GetConnNums() { return mConnNum; }
    int GetDisconnNums() { return mDisconnNum; }
    int GetConnFailNums() { return mConnFailNum; }
    void ShowRecvMsg();
private:
    XMsgClient      mMsgClient;
    RTHttpClient    mHttpClient;
    std::string     mUserid;
    std::string     mAuth;
    std::string     mUname;
    std::string     mPushToken;
    std::string     mMsgServer;
    int             mMsgPort;
    std::string     mCurRoomId;
    std::unordered_set<std::string>     mRoomSet;
    std::list<std::string>              mMsgList;
    
    bool            mIsOnline;
    int             mRecvNum;
    int             mConnNum;
    int             mDisconnNum;
    int             mConnFailNum;
    pthread_cond_t          mCond;
    pthread_mutex_t         mMutex;
};

#endif /* RTMsgClient_hpp */
