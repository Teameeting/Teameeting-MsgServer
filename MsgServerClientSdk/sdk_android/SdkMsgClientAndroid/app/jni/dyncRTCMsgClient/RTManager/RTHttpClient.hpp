//
//  RTHttpClient.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTHttpClient_hpp
#define RTHttpClient_hpp

#include <stdio.h>
#include <unordered_set>
#include "RTCurlClient.hpp"

class RTHttpClient{
public:
    RTHttpClient();
    ~RTHttpClient();
    std::string&    GetUserid() { return mUserid; }
    std::string&    GetAuth() { return mAuth; }
    std::string&    GetUname() { return mUname; }
    std::string&    GetPushToken() { return mPushToken; }
    void            SyncRoomSet(std::unordered_set<std::string>& roomSet);
public:
    int UserInit(const std::string& uid);
    int ApplyRoom();
    
private:
    RTCurlClient    mCurlClient;
    std::string     mUserid;
    std::string     mAuth;
    std::string     mUname;
    std::string     mPushToken;
    std::unordered_set<std::string> mRoomSet;
};

#endif /* RTHttpClient_hpp */
