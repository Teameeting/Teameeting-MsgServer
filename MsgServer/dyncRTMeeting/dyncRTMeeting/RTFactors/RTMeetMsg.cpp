//
//  RTMeetMsg.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/7/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <stdio.h>
#include "RTMeetMsg.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"

std::string MEETMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    jDoc.SetObject();
    jDoc.AddMember("mtype", MEETMSG::_mtype, jDoc.GetAllocator());
    jDoc.AddMember("cmd", MEETMSG::_cmd, jDoc.GetAllocator());
    jDoc.AddMember("action", MEETMSG::_action, jDoc.GetAllocator());
    jDoc.AddMember("tags", MEETMSG::_tags, jDoc.GetAllocator());
    jDoc.AddMember("type", MEETMSG::_type, jDoc.GetAllocator());
    jDoc.AddMember("mseq", MEETMSG::_mseq, jDoc.GetAllocator());
    jDoc.AddMember("from", MEETMSG::_from.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("room", MEETMSG::_room.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("sess", MEETMSG::_sess.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("to", MEETMSG::_to.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("cont", MEETMSG::_cont.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("pass", MEETMSG::_pass.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("code", MEETMSG::_code, jDoc.GetAllocator());
    jDoc.AddMember("status", MEETMSG::_status.c_str(), jDoc.GetAllocator());
    
    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void MEETMSG::GetMsg(const std::string& str, std::string& err)
{
    if (str.empty() || str.length() == 0) {
        err.assign("bad params");
        return;
    }
    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)str.c_str()).HasParseError())
    {
        err.assign("parse error");
        return;
    }
    if(!(jsonReqDoc.HasMember("mtype") && jsonReqDoc["mtype"].IsInt()))
    {
        err.assign("parse mtype error");
        return;
    }
    _mtype = (MSGTYPE)jsonReqDoc["mtype"].GetInt();
    if(!(jsonReqDoc.HasMember("cmd") && jsonReqDoc["cmd"].IsInt()))
    {
        err.assign("parse cmd error");
        return;
    }
    _cmd = jsonReqDoc["cmd"].GetInt();
    if(!(jsonReqDoc.HasMember("action") && jsonReqDoc["action"].IsInt()))
    {
        err.assign("parse action error");
        return;
    }
    _action = jsonReqDoc["action"].GetInt();
    if(!(jsonReqDoc.HasMember("tags") && jsonReqDoc["tags"].IsInt()))
    {
        err.assign("parse tags error");
        return;
    }
    _tags = jsonReqDoc["tags"].GetInt();
    if(!(jsonReqDoc.HasMember("type") && jsonReqDoc["type"].IsInt()))
    {
        err.assign("parse type error");
        return;
    }
    _type = jsonReqDoc["type"].GetInt();
    if(!(jsonReqDoc.HasMember("mseq") && jsonReqDoc["mseq"].IsInt64()))
    {
        err.assign("parse mseq error");
        return;
    }
    _mseq = jsonReqDoc["mseq"].GetInt64();
    if(!(jsonReqDoc.HasMember("from") && jsonReqDoc["from"].IsString()))
    {
        err.assign("parse from error");
        return;
    }
    _from = jsonReqDoc["from"].GetString();
    if(!(jsonReqDoc.HasMember("room") && jsonReqDoc["room"].IsString()))
    {
        err.assign("parse room error");
        return;
    }
    _room = jsonReqDoc["room"].GetString();
    if(!(jsonReqDoc.HasMember("sess") && jsonReqDoc["sess"].IsString()))
    {
        err.assign("parse sess error");
        return;
    }
    _sess = jsonReqDoc["sess"].GetString();
    if(!(jsonReqDoc.HasMember("to") && jsonReqDoc["to"].IsString()))
    {
        err.assign("parse to error");
        return;
    }
    _to = jsonReqDoc["to"].GetString();
    if(!(jsonReqDoc.HasMember("cont") && jsonReqDoc["cont"].IsString()))
    {
        err.assign("parse cont error");
        return;
    }
    _cont = jsonReqDoc["cont"].GetString();
    if(!(jsonReqDoc.HasMember("pass") && jsonReqDoc["pass"].IsString()))
    {
        err.assign("parse pass error");
        return;
    }
    _pass = jsonReqDoc["pass"].GetString();
    if(!(jsonReqDoc.HasMember("code") && jsonReqDoc["code"].IsInt()))
    {
        err.assign("parse code error");
        return;
    }
    _code = jsonReqDoc["code"].GetInt();
    if(!(jsonReqDoc.HasMember("status") && jsonReqDoc["status"].IsString()))
    {
        err.assign("parse status error");
        return;
    }
    _status = jsonReqDoc["status"].GetString();
}


