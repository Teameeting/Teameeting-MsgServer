//
//  RTMessage.cpp
//  dyncRTConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <stdio.h>
#include "RTMessage.h"
#include "RTMsgCommon.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"

_transfermsg::_transfermsg()
      : _action(transferaction_invalid)
      , _fmodule(transfermodule_invalid)
      , _type(transfertype_invalid)
      , _trans_seq(0)
      , _trans_seq_ack(0)
      , _valid(0)
      , _content(""){}

std::string TRANSFERMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("action", TRANSFERMSG::_action, jDoc.GetAllocator());
    jDoc.AddMember("fmodule", TRANSFERMSG::_fmodule, jDoc.GetAllocator());
    jDoc.AddMember("type", TRANSFERMSG::_type, jDoc.GetAllocator());
    jDoc.AddMember("trans_seq", TRANSFERMSG::_trans_seq, jDoc.GetAllocator());
    jDoc.AddMember("trans_seq_ack", TRANSFERMSG::_trans_seq_ack, jDoc.GetAllocator());
    jDoc.AddMember("valid", TRANSFERMSG::_valid, jDoc.GetAllocator());
    jDoc.AddMember("content", TRANSFERMSG::_content.c_str(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void TRANSFERMSG::GetMsg(const std::string& str, std::string& err)
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
    if(!(jsonReqDoc.HasMember("action") && jsonReqDoc["action"].IsInt()))
    {
        err = "parse action error";
        return;
    }
    _action = (TRANSFERACTION)jsonReqDoc["action"].GetInt();
    if(!(jsonReqDoc.HasMember("fmodule") && jsonReqDoc["fmodule"].IsInt()))
    {
        err = "parse fmodule error";
        return;
    }
    _fmodule = (TRANSFERMODULE)jsonReqDoc["fmodule"].GetInt();
    if(!(jsonReqDoc.HasMember("type") && jsonReqDoc["type"].IsInt()))
    {
        err = "parse type error";
        return;
    }
    _type = (TRANSFERTYPE)jsonReqDoc["type"].GetInt();
    if(!(jsonReqDoc.HasMember("trans_seq") && jsonReqDoc["trans_seq"].IsUint64()))
    {
        err = "parse trans_seq error";
        return;
    }
    _trans_seq = jsonReqDoc["trans_seq"].GetUint64();
    if(!(jsonReqDoc.HasMember("trans_seq_ack") && jsonReqDoc["trans_seq_ack"].IsUint64()))
    {
        err = "parse trans_seq_ack error";
        return;
    }
    _trans_seq_ack = jsonReqDoc["trans_seq_ack"].GetUint64();
    if(!(jsonReqDoc.HasMember("valid") && jsonReqDoc["valid"].IsInt()))
    {
        err = "parse valid error";
        return;
    }
    _valid = jsonReqDoc["valid"].GetInt();
    if(!(jsonReqDoc.HasMember("content") && jsonReqDoc["content"].IsString()))
    {
        err = "parse content error";
        return;
    }
    _content = jsonReqDoc["content"].GetString();
}

_connmsg::_connmsg()
    : _tag(conntag_invalid)
    , _msg("")
    , _id("")
    , _msgid("")
    , _moduleid(""){}

std::string CONNMSG::ToJson()
{

    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("tag", CONNMSG::_tag, jDoc.GetAllocator());
    jDoc.AddMember("msg", CONNMSG::_msg.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("id", CONNMSG::_id.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("msgid", CONNMSG::_msgid.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("moduleid", CONNMSG::_moduleid.c_str(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void CONNMSG::GetMsg(const std::string& str, std::string& err)
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
    if(!(jsonReqDoc.HasMember("tag") && jsonReqDoc["tag"].IsInt()))
    {
        err = "parse tag error";
        return;
    }
    _tag = (CONNTAG)jsonReqDoc["tag"].GetInt();
    if(!(jsonReqDoc.HasMember("msg") && jsonReqDoc["msg"].IsString()))
    {
        err = "parse msg error";
        return;
    }
    _msg = jsonReqDoc["msg"].GetString();
    if(!(jsonReqDoc.HasMember("id") && jsonReqDoc["id"].IsString()))
    {
        err = "parse id error";
        return;
    }
    _id = jsonReqDoc["id"].GetString();
    if(!(jsonReqDoc.HasMember("msgid") && jsonReqDoc["msgid"].IsString()))
    {
        err = "parse msgid error";
        return;
    }
    _msgid = jsonReqDoc["msgid"].GetString();
    if(!(jsonReqDoc.HasMember("moduleid") && jsonReqDoc["moduleid"].IsString()))
    {
        err = "parse moduleid error";
        return;
    }
    _moduleid = jsonReqDoc["moduleid"].GetString();
}

_transmsg::_transmsg()
    : _flag(0)
    , _touser("")
    , _connector("")
    , _content(""){}

std::string TRANSMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("flag", TRANSMSG::_flag, jDoc.GetAllocator());
    jDoc.AddMember("touser", TRANSMSG::_touser.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("connector", TRANSMSG::_connector.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("content", TRANSMSG::_content.c_str(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void TRANSMSG::GetMsg(const std::string& str, std::string& err)
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
    if(!(jsonReqDoc.HasMember("flag") && jsonReqDoc["flag"].IsInt()))
    {
        err = "parse flag error";
        return;
    }
    _flag = jsonReqDoc["flag"].GetInt();
    if(!(jsonReqDoc.HasMember("touser") && jsonReqDoc["touser"].IsString()))
    {
        err = "parse touser error";
        return;
    }
    _touser = jsonReqDoc["touser"].GetString();
    if(!(jsonReqDoc.HasMember("connector") && jsonReqDoc["connector"].IsString()))
    {
        err = "parse connector error";
        return;
    }
    _connector = jsonReqDoc["connector"].GetString();
    if(!(jsonReqDoc.HasMember("content") && jsonReqDoc["content"].IsString()))
    {
        err = "parse content error";
        return;
    }
    _content = jsonReqDoc["content"].GetString();
}

_queuemsg::_queuemsg()
    : _flag(0)
    , _touser("")
    , _connector("")
    , _content(""){}

std::string QUEUEMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("flag", QUEUEMSG::_flag, jDoc.GetAllocator());
    jDoc.AddMember("touser", QUEUEMSG::_touser.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("connector", QUEUEMSG::_connector.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("content", QUEUEMSG::_content.c_str(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void QUEUEMSG::GetMsg(const std::string& str, std::string& err)
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
    if(!(jsonReqDoc.HasMember("flag") && jsonReqDoc["flag"].IsInt()))
    {
        err = "parse flag error";
        return;
    }
    _flag = jsonReqDoc["flag"].GetInt();
    if(!(jsonReqDoc.HasMember("touser") && jsonReqDoc["touser"].IsString()))
    {
        err = "parse touser error";
        return;
    }
    _touser = jsonReqDoc["touser"].GetString();
    if(!(jsonReqDoc.HasMember("connector") && jsonReqDoc["connector"].IsString()))
    {
        err = "parse connector error";
        return;
    }
    _connector = jsonReqDoc["connector"].GetString();
    if(!(jsonReqDoc.HasMember("content") && jsonReqDoc["content"].IsString()))
    {
        err = "parse content error";
        return;
    }
    _content = jsonReqDoc["content"].GetString();
}

_dispatchmsg::_dispatchmsg()
    : _flag(0)
    , _touser("")
    , _connector("")
    , _content(""){}

std::string DISPATCHMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("flag", DISPATCHMSG::_flag, jDoc.GetAllocator());
    jDoc.AddMember("touser", DISPATCHMSG::_touser.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("connector", DISPATCHMSG::_connector.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("content", DISPATCHMSG::_content.c_str(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void DISPATCHMSG::GetMsg(const std::string& str, std::string& err)
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
    if(!(jsonReqDoc.HasMember("flag") && jsonReqDoc["flag"].IsInt()))
    {
        err = "parse flag error";
        return;
    }
    _flag = jsonReqDoc["flag"].GetInt();
    if(!(jsonReqDoc.HasMember("touser") && jsonReqDoc["touser"].IsString()))
    {
        err = "parse touser error";
        return;
    }
    _touser = jsonReqDoc["touser"].GetString();
    if(!(jsonReqDoc.HasMember("connector") && jsonReqDoc["connector"].IsString()))
    {
        err = "parse connector error";
        return;
    }
    _connector = jsonReqDoc["connector"].GetString();
    if(!(jsonReqDoc.HasMember("content") && jsonReqDoc["content"].IsString()))
    {
        err = "parse content error";
        return;
    }
    _content = jsonReqDoc["content"].GetString();
}

_pushmsg::_pushmsg()
    : _flag(0)
    , _touser("")
    , _connector("")
    , _content(""){}

std::string PUSHMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("flag", PUSHMSG::_flag, jDoc.GetAllocator());
    jDoc.AddMember("touser", PUSHMSG::_touser.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("connector", PUSHMSG::_connector.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("content", PUSHMSG::_content.c_str(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void PUSHMSG::GetMsg(const std::string& str, std::string& err)
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
    if(!(jsonReqDoc.HasMember("flag") && jsonReqDoc["flag"].IsInt()))
    {
        err = "parse flag error";
        return;
    }
    _flag = jsonReqDoc["flag"].GetInt();
    if(!(jsonReqDoc.HasMember("touser") && jsonReqDoc["touser"].IsString()))
    {
        err = "parse touser error";
        return;
    }
    _touser = jsonReqDoc["touser"].GetString();
    if(!(jsonReqDoc.HasMember("connector") && jsonReqDoc["connector"].IsString()))
    {
        err = "parse connector error";
        return;
    }
    _connector = jsonReqDoc["connector"].GetString();
    if(!(jsonReqDoc.HasMember("content") && jsonReqDoc["content"].IsString()))
    {
        err = "parse content error";
        return;
    }
    _content = jsonReqDoc["content"].GetString();
}

_topushmsg::_topushmsg()
: _tags(sendtags_invalid)
, _roomid(""){}

std::string TOPUSHMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    jDoc.SetObject();
    jDoc.AddMember("tags", TOPUSHMSG::_tags, jDoc.GetAllocator());
    jDoc.AddMember("roomid", TOPUSHMSG::_roomid.c_str(), jDoc.GetAllocator());
    
    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void TOPUSHMSG::GetMsg(const std::string& str, std::string& err)
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
    if(!(jsonReqDoc.HasMember("tags") && jsonReqDoc["tags"].IsInt()))
    {
        err = "parse tags error";
        return;
    }
    _tags = jsonReqDoc["tags"].GetInt();
    if(!(jsonReqDoc.HasMember("roomid") && jsonReqDoc["roomid"].IsString()))
    {
        err = "parse roomid error";
        return;
    }
    _roomid = jsonReqDoc["roomid"].GetString();
}

_tojsonuser::_tojsonuser()
    : _us(){}

std::string TOJSONUSER::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    rapidjson::Value mems(rapidjson::kArrayType);
    std::list<std::string>::iterator it = TOJSONUSER::_us.begin();
    for (; it!=TOJSONUSER::_us.end(); it++) {
        mems.PushBack((*it).c_str(), jDoc.GetAllocator());
    }

    jDoc.SetObject();
    jDoc.AddMember("u", mems, jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void TOJSONUSER::GetMsg(const std::string &str, std::string &err)
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
    if(!(jsonReqDoc.HasMember("u") && jsonReqDoc["u"].IsArray()))
    {
        err = "parse u error";
        return;
    }
    rapidjson::Value& mems = jsonReqDoc["u"];
    for (int i=0; i<(int)mems.Capacity(); i++) {
        rapidjson::Value& m = mems[i];
        _us.push_front(m.GetString());
    }
}

_topushuser::_topushuser()
: _us(){}

std::string TOPUSHUSER::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    rapidjson::Value mems(rapidjson::kArrayType);
    std::list<std::string>::iterator it = TOPUSHUSER::_us.begin();
    for (; it!=TOPUSHUSER::_us.end(); it++) {
        mems.PushBack((*it).c_str(), jDoc.GetAllocator());
    }
    
    mems.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void TOPUSHUSER::GetMsg(const std::string &str, std::string &err)
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
    if(!(jsonReqDoc.IsArray()))
    {
        err = "parse ReqDoc error";
        return;
    }
    rapidjson::Value& mems = jsonReqDoc;
    for (int i=0; i<(int)mems.Capacity(); i++) {
        rapidjson::Value& m = mems[i];
        _us.push_front(m.GetString());
    }
}
