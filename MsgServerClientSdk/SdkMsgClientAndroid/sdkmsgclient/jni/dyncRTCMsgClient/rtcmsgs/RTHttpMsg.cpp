//
//  RTHttpMsg.cpp
//  dyncRTMeeting
//
//  Created by hp on 1/6/16.
//  Copyright © 2016 hp. All rights reserved.
//

#include <stdio.h>
#include "RTHttpMsg.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "RTMsgCommon.h"

_meetingInfo::_meetingInfo()
    : meetingid("")
      , userid("")
      , meetname("")
      , meetdesc("")
      , meetusable(0)
      , pushable(0)
      , meettype1(0)
      , memnumber(0)
      , crttime(0){}

std::string MEETINGINFO::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("meetingid", MEETINGINFO::meetingid.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("userid", MEETINGINFO::userid.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("meetname", MEETINGINFO::meetname.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("meetdesc", MEETINGINFO::meetdesc.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("meetusable", MEETINGINFO::meetusable, jDoc.GetAllocator());
    jDoc.AddMember("pushable", MEETINGINFO::pushable, jDoc.GetAllocator());
    jDoc.AddMember("meettype1", MEETINGINFO::meettype1, jDoc.GetAllocator());
    jDoc.AddMember("memnumber", MEETINGINFO::memnumber, jDoc.GetAllocator());
    jDoc.AddMember("crttime", MEETINGINFO::crttime, jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void MEETINGINFO::GetMsg(const std::string& str, std::string& err)
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

    if(!(jsonReqDoc.HasMember("meetingid") && jsonReqDoc["meetingid"].IsString()))
    {
        err = "parse meetingid error";
        return;
    }
    meetingid = jsonReqDoc["meetingid"].GetString();
    if(!(jsonReqDoc.HasMember("userid") && jsonReqDoc["userid"].IsString()))
    {
        err = "parse userid error";
        return;
    }
    userid = jsonReqDoc["userid"].GetString();
    if(!(jsonReqDoc.HasMember("meetname") && jsonReqDoc["meetname"].IsString()))
    {
        err = "parse meetname error";
        return;
    }
    meetname = jsonReqDoc["meetname"].GetString();
    if(!(jsonReqDoc.HasMember("meetdesc") && jsonReqDoc["meetdesc"].IsString()))
    {
        err = "parse meetdesc error";
        return;
    }
    meetdesc = jsonReqDoc["meetdesc"].GetString();
    if(!(jsonReqDoc.HasMember("meetusable") && jsonReqDoc["meetusable"].IsInt()))
    {
        err = "parse meetusable error";
        return;
    }
    meetusable = jsonReqDoc["meetusable"].GetInt();
    if(!(jsonReqDoc.HasMember("pushable") && jsonReqDoc["pushable"].IsInt()))
    {
        err = "parse pushable error";
        return;
    }
    pushable = jsonReqDoc["pushable"].GetInt();
    if(!(jsonReqDoc.HasMember("meettype1") && jsonReqDoc["meettype1"].IsInt()))
    {
        err = "parse meettype1 error";
        return;
    }
    meettype1 = jsonReqDoc["meettype1"].GetInt();
    if(!(jsonReqDoc.HasMember("memnumber") && jsonReqDoc["memnumber"].IsInt()))
    {
        err = "parse memnumber error";
        return;
    }
    memnumber = jsonReqDoc["memnumber"].GetInt();
    if(!(jsonReqDoc.HasMember("crttime") && jsonReqDoc["crttime"].IsInt64()))
    {
        err = "parse crttime error";
        return;
    }
    crttime = jsonReqDoc["crttime"].GetInt64();
}

_meetingMemberList::_meetingMemberList()
    : _uslist(){}

std::string MEETINGMEMBERLIST::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    rapidjson::Value mems(rapidjson::kArrayType);
    std::list<std::string>::iterator it = MEETINGMEMBERLIST::_uslist.begin();
    for (; it!=MEETINGMEMBERLIST::_uslist.end(); it++) {
        mems.PushBack((*it).c_str(), jDoc.GetAllocator());
    }

    jDoc.SetObject();
    jDoc.AddMember("meetingMemberList", mems, jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void MEETINGMEMBERLIST::GetMsg(const std::string &str, std::string &err)
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
    if(!(jsonReqDoc.HasMember("meetingMemberList") && jsonReqDoc["meetingMemberList"].IsArray()))
    {
        err = "parse meetingMemberList error";
        return;
    }
    rapidjson::Value& mems = jsonReqDoc["meetingMemberList"];
    for (int i=0; i<mems.Capacity(); i++) {
        rapidjson::Value& m = mems[i]["userid"];
        _uslist.push_front(m.GetString());
    }
}
