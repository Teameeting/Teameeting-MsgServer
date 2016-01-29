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
#include "RTMsgCommon.h"
#include "RTSignalMsg.h"

_meetmsg::_meetmsg()
    : _mtype(msgtype_invalid)
      , _messagetype(messagetype_invalid)
      , _signaltype(signaltype_invalid)
      , _cmd(meetcmd_invalid)
      , _action(dcommaction_invalid)
      , _tags(sendtags_invalid)
      , _type(sendtype_invalid)
      , _nmem(0)
      , _ntime(0)
      , _mseq(0)
      , _from("")
      , _room("")
      , _to("")
      , _cont("")
      , _pass("")
      , _nname("")
      ,_rname("")
      , _code(0){}

std::string MEETMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("mtype", MEETMSG::_mtype, jDoc.GetAllocator());
    jDoc.AddMember("messagetype", MEETMSG::_messagetype, jDoc.GetAllocator());
    jDoc.AddMember("signaltype", MEETMSG::_signaltype, jDoc.GetAllocator());
    jDoc.AddMember("cmd", MEETMSG::_cmd, jDoc.GetAllocator());
    jDoc.AddMember("action", MEETMSG::_action, jDoc.GetAllocator());
    jDoc.AddMember("tags", MEETMSG::_tags, jDoc.GetAllocator());
    jDoc.AddMember("type", MEETMSG::_type, jDoc.GetAllocator());
    jDoc.AddMember("nmem", MEETMSG::_nmem, jDoc.GetAllocator());
    jDoc.AddMember("ntime", MEETMSG::_ntime, jDoc.GetAllocator());
    jDoc.AddMember("mseq", MEETMSG::_mseq, jDoc.GetAllocator());
    jDoc.AddMember("from", MEETMSG::_from.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("room", MEETMSG::_room.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("to", MEETMSG::_to.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("cont", MEETMSG::_cont.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("pass", MEETMSG::_pass.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("nname", MEETMSG::_nname.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("rname", MEETMSG::_rname.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("code", MEETMSG::_code, jDoc.GetAllocator());
    

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void MEETMSG::GetMsg(const std::string& str, std::string& err)
{
    if (str.empty() || str.length() == 0) {
        err = "bad params";
        return;
    }
    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)str.c_str()).HasParseError())
    {
        err = INVALID_JSON_PARAMS;
        return;
    }
    if(!(jsonReqDoc.HasMember("mtype") && jsonReqDoc["mtype"].IsInt()))
    {
        err = "parse mtype error";
        return;
    }
    _mtype = (MSGTYPE)jsonReqDoc["mtype"].GetInt();
    if(!(jsonReqDoc.HasMember("messagetype") && jsonReqDoc["messagetype"].IsInt()))
    {
        err = "parse messagetype error";
        return;
    }
    _messagetype = (MESSAGETYPE)jsonReqDoc["messagetype"].GetInt();
    if(!(jsonReqDoc.HasMember("signaltype") && jsonReqDoc["signaltype"].IsInt()))
    {
        err = "parse signaltype error";
        return;
    }
    _signaltype = jsonReqDoc["signaltype"].GetInt();
    if(!(jsonReqDoc.HasMember("cmd") && jsonReqDoc["cmd"].IsInt()))
    {
        err = "parse cmd error";
        return;
    }
    _cmd = jsonReqDoc["cmd"].GetInt();
    if(!(jsonReqDoc.HasMember("action") && jsonReqDoc["action"].IsInt()))
    {
        err = "parse action error";
        return;
    }
    _action = jsonReqDoc["action"].GetInt();
    if(!(jsonReqDoc.HasMember("tags") && jsonReqDoc["tags"].IsInt()))
    {
        err = "parse tags error";
        return;
    }
    _tags = jsonReqDoc["tags"].GetInt();
    if(!(jsonReqDoc.HasMember("type") && jsonReqDoc["type"].IsInt()))
    {
        err = "parse type error";
        return;
    }
    _type = jsonReqDoc["type"].GetInt();


    if(!(jsonReqDoc.HasMember("nmem") && jsonReqDoc["nmem"].IsInt()))
    {
        err = "parse nmem error";
        return;
    }
    _nmem = jsonReqDoc["nmem"].GetInt();
    if(!(jsonReqDoc.HasMember("ntime") && jsonReqDoc["ntime"].IsInt64()))
    {
        err = "parse ntime error";
        return;
    }
    _ntime = jsonReqDoc["ntime"].GetInt64();
    if(!(jsonReqDoc.HasMember("mseq") && jsonReqDoc["mseq"].IsUint64()))
    {
        err = "parse mseq error";
        return;
    }
    _mseq = jsonReqDoc["mseq"].GetUint64();
    if(!(jsonReqDoc.HasMember("from") && jsonReqDoc["from"].IsString()))
    {
        err = "parse from error";
        return;
    }
    _from = jsonReqDoc["from"].GetString();
    if(!(jsonReqDoc.HasMember("room") && jsonReqDoc["room"].IsString()))
    {
        err = "parse room error";
        return;
    }
    _room = jsonReqDoc["room"].GetString();
    if(!(jsonReqDoc.HasMember("to") && jsonReqDoc["to"].IsString()))
    {
        err = "parse to error";
        return;
    }
    _to = jsonReqDoc["to"].GetString();
    if(!(jsonReqDoc.HasMember("cont") && jsonReqDoc["cont"].IsString()))
    {
        err = "parse cont error";
        return;
    }
    _cont = jsonReqDoc["cont"].GetString();
    if(!(jsonReqDoc.HasMember("pass") && jsonReqDoc["pass"].IsString()))
    {
        err = "parse pass error";
        return;
    }
    _pass = jsonReqDoc["pass"].GetString();
    if(!(jsonReqDoc.HasMember("nname") && jsonReqDoc["nname"].IsString()))
    {
        err = "parse nname error";
        return;
    }
    _nname = jsonReqDoc["nname"].GetString();
    if(!(jsonReqDoc.HasMember("rname") && jsonReqDoc["rname"].IsString()))
    {
        err = "parse rname error";
        return;
    }
    _rname = jsonReqDoc["rname"].GetString();
    if(!(jsonReqDoc.HasMember("code") && jsonReqDoc["code"].IsInt()))
    {
        err = "parse code error";
        return;
    }
    _code = jsonReqDoc["code"].GetInt();
}


