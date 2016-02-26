//
//  RTJson.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTJson.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "RTMsgCommon.h"

_information::_information()
: _userid("")
, _uname("")
, _upushtoken("")
, _ustatus(0)
, _uregtype(0)
, _ulogindev(0)
, _uactype(0)
, _uregtime(0){}

std::string Information::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    jDoc.SetObject();
    jDoc.AddMember("userid", Information::_userid.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("uname", Information::_uname.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("upushtoken", Information::_upushtoken.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ustatus", Information::_ustatus, jDoc.GetAllocator());
    jDoc.AddMember("uregtype", Information::_uregtype, jDoc.GetAllocator());
    jDoc.AddMember("ulogindev", Information::_ulogindev, jDoc.GetAllocator());
    jDoc.AddMember("uactype", Information::_uactype, jDoc.GetAllocator());
    jDoc.AddMember("uregtime", Information::_uregtime, jDoc.GetAllocator());
    
    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void Information::GetMsg(const std::string& str, std::string& err)
{
    if (str.empty() || str.length() == 0) {
        err.assign("bad params");
        return;
    }
    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)str.c_str()).HasParseError())
    {
        err.assign(INVALID_JSON_PARAMS);
        return;
    }
    if(!(jsonReqDoc.HasMember("userid") && jsonReqDoc["userid"].IsString()))
    {
        err.assign("parse userid error");
        return;
    }
    _userid = jsonReqDoc["userid"].GetString();
    if(!(jsonReqDoc.HasMember("uname") && jsonReqDoc["uname"].IsString()))
    {
        err.assign("parse uname error");
        return;
    }
    _uname = jsonReqDoc["uname"].GetString();
    if(!(jsonReqDoc.HasMember("upushtoken") && jsonReqDoc["upushtoken"].IsString()))
    {
        err.assign("parse upushtoken error");
        return;
    }
    _upushtoken = jsonReqDoc["upushtoken"].GetString();
    if(!(jsonReqDoc.HasMember("ustatus") && jsonReqDoc["ustatus"].IsInt()))
    {
        err.assign("parse ustatus error");
        return;
    }
    _ustatus = jsonReqDoc["ustatus"].GetInt();
    if(!(jsonReqDoc.HasMember("uregtype") && jsonReqDoc["uregtype"].IsInt()))
    {
        err.assign("parse uregtype error");
        return;
    }
    _uregtype = jsonReqDoc["uregtype"].GetInt();
    if(!(jsonReqDoc.HasMember("ulogindev") && jsonReqDoc["ulogindev"].IsInt()))
    {
        err.assign("parse ulogindev error");
        return;
    }
    _ulogindev = jsonReqDoc["ulogindev"].GetInt();
    if(!(jsonReqDoc.HasMember("uactype") && jsonReqDoc["uactype"].IsInt()))
    {
        err.assign("parse uactype error");
        return;
    }
    _uactype = jsonReqDoc["uactype"].GetInt();
    if(!(jsonReqDoc.HasMember("uregtime") && jsonReqDoc["uregtime"].IsInt64()))
    {
        err.assign("parse uregtime error");
        return;
    }
    _uregtime = jsonReqDoc["uregtime"].GetInt64();
}

_httprespuserinit::_httprespuserinit()
: _requestid(0)
, _code(0)
, _authorization("")
, _message(""){}

std::string _httprespuserinit::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    rapidjson::Value info(rapidjson::kObjectType);
    info.AddMember("userid", HttpRespUserInit::_information._userid.c_str(), jDoc.GetAllocator());
    info.AddMember("uname", HttpRespUserInit::_information._uname.c_str(), jDoc.GetAllocator());
    info.AddMember("upushtoken", HttpRespUserInit::_information._upushtoken.c_str(), jDoc.GetAllocator());
    info.AddMember("ustatus", HttpRespUserInit::_information._ustatus, jDoc.GetAllocator());
    info.AddMember("uregtype", HttpRespUserInit::_information._uregtype, jDoc.GetAllocator());
    info.AddMember("ulogindev", HttpRespUserInit::_information._ulogindev, jDoc.GetAllocator());
    info.AddMember("uactype", HttpRespUserInit::_information._uactype, jDoc.GetAllocator());
    info.AddMember("uregtime", HttpRespUserInit::_information._uregtime, jDoc.GetAllocator());
    
    jDoc.SetObject();
    jDoc.AddMember("requestid", HttpRespUserInit::_requestid, jDoc.GetAllocator());
    jDoc.AddMember("code", HttpRespUserInit::_code, jDoc.GetAllocator());
    jDoc.AddMember("authorization", HttpRespUserInit::_authorization.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("information", info, jDoc.GetAllocator());
    jDoc.AddMember("message", HttpRespUserInit::_message.c_str(), jDoc.GetAllocator());
    
    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void _httprespuserinit::GetMsg(const std::string& str, std::string& err)
{
    if (str.empty() || str.length() == 0) {
        err.assign("bad params");
        return;
    }
    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)str.c_str()).HasParseError())
    {
        err.assign(INVALID_JSON_PARAMS);
        return;
    }
    if(!(jsonReqDoc.HasMember("requestid") && jsonReqDoc["requestid"].IsInt64()))
    {
        err.assign("parse requestid error");
        return;
    }
    _requestid = jsonReqDoc["requestid"].GetInt64();
    if(!(jsonReqDoc.HasMember("code") && jsonReqDoc["code"].IsInt()))
    {
        err.assign("parse code error");
        return;
    }
    _code = jsonReqDoc["code"].GetInt();
    if(!(jsonReqDoc.HasMember("authorization") && jsonReqDoc["authorization"].IsString()))
    {
        err.assign("parse authorization error");
        return;
    }
    _authorization = jsonReqDoc["authorization"].GetString();
    if(!(jsonReqDoc.HasMember("information") && jsonReqDoc["information"].IsObject()))
    {
        err.assign("parse information error");
        return;
    }
   
    rapidjson::Value info(rapidjson::kObjectType);
    info = jsonReqDoc["information"];
    if(!(info.HasMember("userid") && info["userid"].IsString()))
    {
        err.assign("parse info userid error");
        return;
    }
    _information._userid = info["userid"].GetString();
    if(!(info.HasMember("uname") && info["uname"].IsString()))
    {
        err.assign("parse info uname error");
        return;
    }
    _information._uname = info["uname"].GetString();
    if(!(info.HasMember("upushtoken") && info["upushtoken"].IsString()))
    {
        err.assign("parse info upushtoken error");
        return;
    }
    _information._upushtoken = info["upushtoken"].GetString();
    if(!(info.HasMember("ustatus") && info["ustatus"].IsInt()))
    {
        err.assign("parse info ustatus error");
        return;
    }
    _information._ustatus = info["ustatus"].GetInt();
    if(!(info.HasMember("uregtype") && info["uregtype"].IsInt()))
    {
        err.assign("parse info uregtype error");
        return;
    }
    _information._uregtype = info["uregtype"].GetInt();
    if(!(info.HasMember("ulogindev") && info["ulogindev"].IsInt()))
    {
        err.assign("parse info ulogindev error");
        return;
    }
    _information._ulogindev = info["ulogindev"].GetInt();
    if(!(info.HasMember("uactype") && info["uactype"].IsInt()))
    {
        err.assign("parse info uactype error");
        return;
    }
    _information._uactype = info["uactype"].GetInt();
    if(!(info.HasMember("uregtime") && info["uregtime"].IsInt64()))
    {
        err.assign("parse info uregtime error");
        return;
    }
    _information._uregtime = info["uregtime"].GetInt64();
    if(!(jsonReqDoc.HasMember("message") && jsonReqDoc["message"].IsString()))
    {
        err.assign("parse message error");
        return;
    }
    _message = jsonReqDoc["message"].GetString();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////


_meetinginfo::_meetinginfo()
: _meetingid(0)
, _anyrtcid(0)
, _meetname("")
, _meetdesc("")
, _meetenable("")
, _pushable("")
, _meettype("")
, _jointime(0){}

std::string MeetingInfo::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    jDoc.SetObject();
    jDoc.AddMember("meetingid", MeetingInfo::_meetingid, jDoc.GetAllocator());
    jDoc.AddMember("aynrtcid", MeetingInfo::_anyrtcid, jDoc.GetAllocator());
    jDoc.AddMember("meetname", MeetingInfo::_meetname.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("meetdesc", MeetingInfo::_meetdesc.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("meetenable", MeetingInfo::_meetenable.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("pushable", MeetingInfo::_pushable.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("meettype", MeetingInfo::_meettype.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("jointime", MeetingInfo::_jointime, jDoc.GetAllocator());
    
    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void MeetingInfo::GetMsg(const std::string& str, std::string& err)
{
    if (str.empty() || str.length() == 0) {
        err.assign("bad params");
        return;
    }
    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)str.c_str()).HasParseError())
    {
        err.assign(INVALID_JSON_PARAMS);
        return;
    }
    if(!(jsonReqDoc.HasMember("meetingid") && jsonReqDoc["meetingid"].IsInt64()))
    {
        err.assign("parse meetingid error");
        return;
    }
    _meetingid = jsonReqDoc["meetingid"].GetInt64();
    if(!(jsonReqDoc.HasMember("anyrtcid") && jsonReqDoc["anyrtcid"].IsInt64()))
    {
        err.assign("parse anyrtcid error");
        return;
    }
    _anyrtcid = jsonReqDoc["anyrtcid"].GetInt64();
    if(!(jsonReqDoc.HasMember("meetname") && jsonReqDoc["meetname"].IsString()))
    {
        err.assign("parse meetname error");
        return;
    }
    _meetname = jsonReqDoc["meetname"].GetString();
    if(!(jsonReqDoc.HasMember("meetdesc") && jsonReqDoc["meetdesc"].IsString()))
    {
        err.assign("parse meetdesc error");
        return;
    }
    _meetdesc = jsonReqDoc["meetdesc"].GetString();
    if(!(jsonReqDoc.HasMember("meetenable") && jsonReqDoc["meetenable"].IsString()))
    {
        err.assign("parse meetenable error");
        return;
    }
    _meetenable = jsonReqDoc["meetenable"].GetString();
    if(!(jsonReqDoc.HasMember("pushable") && jsonReqDoc["pushable"].IsString()))
    {
        err.assign("parse pushable error");
        return;
    }
    _pushable = jsonReqDoc["pushable"].GetString();
    if(!(jsonReqDoc.HasMember("meettype") && jsonReqDoc["meettype"].IsString()))
    {
        err.assign("parse meettype error");
        return;
    }
    _meettype = jsonReqDoc["meettype"].GetString();
    if(!(jsonReqDoc.HasMember("jointime") && jsonReqDoc["jointime"].IsInt64()))
    {
        err.assign("parse jointime error");
        return;
    }
    _jointime = jsonReqDoc["jointime"].GetInt64();
}

_httprespapplyroom::_httprespapplyroom()
: _requestid(0)
, _code(0)
, _message(""){}

std::string _httprespapplyroom::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    rapidjson::Value info(rapidjson::kObjectType);
    info.AddMember("meetingid", HttpRespApplyRoom::_meetingInfo._meetingid, jDoc.GetAllocator());
    info.AddMember("aynrtcid", HttpRespApplyRoom::_meetingInfo._anyrtcid, jDoc.GetAllocator());
    info.AddMember("meetname", HttpRespApplyRoom::_meetingInfo._meetname.c_str(), jDoc.GetAllocator());
    info.AddMember("meetdesc", HttpRespApplyRoom::_meetingInfo._meetdesc.c_str(), jDoc.GetAllocator());
    info.AddMember("meetenable", HttpRespApplyRoom::_meetingInfo._meetenable.c_str(), jDoc.GetAllocator());
    info.AddMember("pushable", HttpRespApplyRoom::_meetingInfo._pushable.c_str(), jDoc.GetAllocator());
    info.AddMember("meettype", HttpRespApplyRoom::_meetingInfo._meettype.c_str(), jDoc.GetAllocator());
    info.AddMember("jointime", HttpRespApplyRoom::_meetingInfo._jointime, jDoc.GetAllocator());
    
    jDoc.SetObject();
    jDoc.AddMember("requestid", HttpRespApplyRoom::_requestid, jDoc.GetAllocator());
    jDoc.AddMember("code", HttpRespApplyRoom::_code, jDoc.GetAllocator());
    jDoc.AddMember("meetingInfo", info, jDoc.GetAllocator());
    jDoc.AddMember("message", HttpRespApplyRoom::_message.c_str(), jDoc.GetAllocator());
    
    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void _httprespapplyroom::GetMsg(const std::string& str, std::string& err)
{
    if (str.empty() || str.length() == 0) {
        err.assign("bad params");
        return;
    }
    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)str.c_str()).HasParseError())
    {
        err.assign(INVALID_JSON_PARAMS);
        return;
    }
    if(!(jsonReqDoc.HasMember("requestid") && jsonReqDoc["requestid"].IsInt64()))
    {
        err.assign("parse requestid error");
        return;
    }
    _requestid = jsonReqDoc["requestid"].GetInt64();
    if(!(jsonReqDoc.HasMember("code") && jsonReqDoc["code"].IsInt()))
    {
        err.assign("parse code error");
        return;
    }
    _code = jsonReqDoc["code"].GetInt();
    if(!(jsonReqDoc.HasMember("meetingInfo") && jsonReqDoc["meetingInfo"].IsObject()))
    {
        err.assign("parse meetingInfo error");
        return;
    }
    
    rapidjson::Value info(rapidjson::kObjectType);
    info = jsonReqDoc["meetingInfo"];
    if(!(info.HasMember("meetingid") && info["meetingid"].IsInt64()))
    {
        err.assign("parse meetingid error");
        return;
    }
    _meetingInfo._meetingid = info["meetingid"].GetInt64();
    if(!(info.HasMember("anyrtcid") && info["anyrtcid"].IsInt64()))
    {
        err.assign("parse anyrtcid error");
        return;
    }
    _meetingInfo._anyrtcid = info["anyrtcid"].GetInt64();
    if(!(info.HasMember("meetname") && info["meetname"].IsString()))
    {
        err.assign("parse meetname error");
        return;
    }
    _meetingInfo._meetname = info["meetname"].GetString();
    if(!(info.HasMember("meetdesc") && info["meetdesc"].IsString()))
    {
        err.assign("parse meetdesc error");
        return;
    }
    _meetingInfo._meetdesc = info["meetdesc"].GetString();
    if(!(info.HasMember("meetenable") && info["meetenable"].IsString()))
    {
        err.assign("parse meetenable error");
        return;
    }
    _meetingInfo._meetenable = info["meetenable"].GetString();
    if(!(info.HasMember("pushable") && info["pushable"].IsString()))
    {
        err.assign("parse pushable error");
        return;
    }
    _meetingInfo._pushable = info["pushable"].GetString();
    if(!(info.HasMember("meettype") && info["meettype"].IsString()))
    {
        err.assign("parse meettype error");
        return;
    }
    _meetingInfo._meettype = info["meettype"].GetString();
    if(!(info.HasMember("jointime") && info["jointime"].IsInt64()))
    {
        err.assign("parse jointime error");
        return;
    }
    _meetingInfo._jointime = info["jointime"].GetInt64();
    if(!(jsonReqDoc.HasMember("message") && jsonReqDoc["message"].IsString()))
    {
        err.assign("parse message error");
        return;
    }
    _message = jsonReqDoc["message"].GetString();
}