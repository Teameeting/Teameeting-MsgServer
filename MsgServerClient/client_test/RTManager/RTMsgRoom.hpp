//
//  RTMsgRoom.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/25/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTMsgRoom_hpp
#define RTMsgRoom_hpp

#include <stdio.h>
#include <list>
#include "RTMsgClient.hpp"

class RTMsgRoom{
public:
    RTMsgRoom(int num, std::string& id);
    ~RTMsgRoom();

    void Before();
    void After();
    void RunRun();
    void RunOnce();
    void RunThread();
    void RunApplyRoom();
    void RunLoginout();
    void RunSendMsg();


    void TestMsg(long long flag);
    void TestLogin();
    void TestLogout();
    void TestInit();
    void TestUnin();

private:
    typedef std::list<RTMsgClient*> MsgClientList;
    typedef MsgClientList::iterator MsgClientListIt;

    MsgClientList       mClientList;
    int                 mRoomNum;
    std::string         mRoomId;
    bool                mIsRun;
};

#endif /* RTMsgRoom_hpp */
