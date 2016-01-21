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

typedef struct _meetingInfo MEETINGINFO;
struct _meetingInfo{
    std::string meetingid;
    std::string userid;
    std::string meetname;
    std::string meetdesc;
    int         meetusable;
    int         pushable;
    int         meettype1;
    int         memnumber;
    int64_t     crttime;
    _meetingInfo();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _meetingMemberList MEETINGMEMBERLIST;
struct _meetingMemberList{
    std::list<std::string> _uslist;
    _meetingMemberList();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};


#endif /* RTHttpMsg_h */
