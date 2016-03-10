//
//  RTMsgThreadPool.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/25/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTMsgThreadPool_hpp
#define RTMsgThreadPool_hpp

#include <stdio.h>
#include <unordered_set>
#include <vector>
#include "webrtc/base/thread.h"
#include "webrtc/base/messagequeue.h"
#include "RTMsgRoom.hpp"

typedef struct _RoomInfo{
    int _roomNum;
    std::string _roomId;
}RoomInfo;

typedef rtc::TypedMessageData<RoomInfo> MsgData;

class RTMsgThread
: public rtc::MessageHandler
, public rtc::Thread{
public:
    RTMsgThread();
    ~RTMsgThread();
    
    // from rtc::MessageHandler
    virtual void OnMessage(rtc::Message* msg);
      
    void MessageRun(RoomInfo& info);
    void MessageOnce(RoomInfo& info);
    void MessageThread(RoomInfo& info);
    void MessageApplyRoom(RoomInfo& info);
    void MessageLoginout(RoomInfo& info);
    
private:
                      
};

class RTMsgThreadManager
: public rtc::MessageHandler
, public rtc::Thread{
public:
    static RTMsgThreadManager* Instance() {
        static RTMsgThreadManager sInstance;
        return &sInstance;
    }
    
    virtual void OnMessage(rtc::Message* msg);
    
    void AddThread(int num);
    void DelThread();
    void RunThread(int flag);
    void RunThread(int flag, RoomInfo& info);
    
    void RunTest(int flag);
    
    void GetRoomIds();
    void ShowRoomIds();
    
    void NotifyStopThread();
private:
    RTMsgThreadManager()
    : mIsRun(false)
    , mTestThreadNum(0){
        
    }
    
    ~RTMsgThreadManager() {
        
    }
    
    typedef std::unordered_set<RTMsgThread*> ThreadPoolSet;
    typedef ThreadPoolSet::iterator ThreadPoolSetIt;
    typedef std::vector<std::string> RoomIdSet;
    typedef RoomIdSet::iterator RoomIdSetIt;
    
    rtc::ThreadManager  mThreadManager;
    ThreadPoolSet       mThreadPoolSet;
    RoomIdSet           mRoomIds;
    bool                mIsRun;
    int                 mTestThreadNum;
};

#endif /* RTMsgThreadPool_hpp */
