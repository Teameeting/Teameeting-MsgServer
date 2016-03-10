//
//  RTJson.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTJson_hpp
#define RTJson_hpp

#include <stdio.h>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"

typedef struct _information Information;
struct _information{
    std::string _userid;
    std::string _uname;
    std::string _upushtoken;
    int _ustatus;
    int _uregtype;
    int _ulogindev;
    int _uactype;
    int64_t _uregtime;
    _information();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _httprespuserinit HttpRespUserInit;
struct _httprespuserinit{
    int64_t _requestid;
    int _code;
    std::string _authorization;
    Information _information;
    std::string _message;
    _httprespuserinit();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _meetinginfo MeetingInfo;
struct _meetinginfo{
    int64_t _meetingid;
    int64_t _anyrtcid;
    std::string _meetname;
    std::string _meetdesc;
    std::string _meetenable;
    std::string _pushable;
    std::string _meettype;
    int64_t   _jointime;
    _meetinginfo();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

typedef struct _httprespapplyroom HttpRespApplyRoom;
struct _httprespapplyroom{
    int64_t _requestid;
    int _code;
    MeetingInfo _meetingInfo;
    std::string _message;
    _httprespapplyroom();
    std::string ToJson();
    void GetMsg(const std::string& str, std::string& err);
};

#endif /* RTJson_hpp */
