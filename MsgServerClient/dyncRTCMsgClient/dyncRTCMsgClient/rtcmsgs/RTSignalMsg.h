//
//  RTSignalMsg.h
//  dyncRTMsgClient
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMsgClient__RTSignalMsg__
#define __dyncRTMsgClient__RTSignalMsg__

#include <stdio.h>
#include <iostream>
#include "RTMeetMsg.h"

typedef enum _signaltype{
    login = 1,
    sndmsg,
    getmsg,
    logout,
    keepalive,
    signaltype_invalid
}SIGNALTYPE;

typedef struct _signalmsg{
    SIGNALTYPE _stype;
    std::string _scont;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}SIGNALMSG;

#endif /* defined(__dyncRTMsgClient__RTSignalMsg__) */
