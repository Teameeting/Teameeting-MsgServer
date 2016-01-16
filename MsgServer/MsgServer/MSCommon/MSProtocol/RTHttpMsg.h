//
//  RTHttpMsg.h
//  MsgServerMeeting
//
//  Created by hp on 1/6/16.
//  Copyright © 2016 hp. All rights reserved.
//

#ifndef RTHttpMsg_h
#define RTHttpMsg_h
#include <iostream>
#include <string>
#include <list>

typedef struct _meetingInfo{
    std::string meetingid;
    std::string userid;
    std::string meetname;
    std::string meetdesc;
    int         meetusable;
    int         pushable;
    int         meettype1;
    int         memnumber;
    long long   crttime;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}MEETINGINFO;

typedef struct _meetingMemberList{
    std::list<const std::string> _uslist;
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
}MEETINGMEMBERLIST;


#endif /* RTHttpMsg_h */
