//
//  XMsgProcesser.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XMsgProcesser.h"
#include "webrtc/base/logging.h"
#include "XMsgClient.h"

#ifdef WEBRTC_ANDROID
#include <android/log.h>
#define  LOG_TAG    "XMsgProcesser"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#include <iostream>
#include <string>
#endif

int XMsgProcesser::EncodeLogin(std::string& outstr, const std::string& userid, const std::string& token, const std::string& nname)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Login login;
    login.set_usr_from(userid);
    login.set_usr_token(token);
    login.set_usr_nname(nname);

    req.set_svr_cmds(pms::EServerCmd::CLOGIN);
    req.set_mod_type(pms::EModuleType::TMEETING);
    req.set_content(login.SerializeAsString());
    outstr = req.SerializeAsString();
#else
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::login;
    m_msg._cmd = MEETCMD::meetcmd_invalid;
    m_msg._action = DCOMMACTION::dcommaction_invalid;
    m_msg._tags = SENDTAGS::sendtags_invalid;
    m_msg._type = SENDTYPE::sendtype_invalid;
    m_msg._mseq = 0;
    m_msg._from = userid;
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = pass;
    m_msg._code = 0;
    m_msg._nname = nname;
    m_msg._rname = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;

    s_msg._stype = SIGNALTYPE::login;
    s_msg._scont = m_msg.ToJson();

    outstr = s_msg.ToJson();
#endif
    return 0;
}

int XMsgProcesser::EncodeSndMsg(std::string& outstr, const std::string& userid, const std::string& token, const std::string& nname, const std::string& roomid, const std::string& rname, const std::string& to, const std::string& msg, int tag, int type)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::MeetMsg meet;
    meet.set_msg_head(pms::EMsgHead::HSND);
    meet.set_msg_tag((pms::EMsgTag)tag);
    meet.set_msg_type((pms::EMsgType)type);
    meet.set_usr_from(userid);
    meet.set_msg_cont(msg);
    meet.set_rom_id(roomid);
    meet.set_rom_name(rname);
    meet.set_nck_name(nname);
    meet.set_usr_token(token);
    meet.set_usr_toto(to);

    req.set_svr_cmds(pms::EServerCmd::CSNDMSG);
    req.set_mod_type(pms::EModuleType::TMEETING);
    req.set_content(meet.SerializeAsString());
    outstr = req.SerializeAsString();
#else
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::sndmsg;
    m_msg._cmd = cmd;
    m_msg._action = action;
    m_msg._tags = tags;
    m_msg._type = type;
    m_msg._mseq = GenericTransSeq();
    m_msg._from = userid;
    m_msg._room = roomid;
    m_msg._to = to;
    m_msg._cont = msg;
    m_msg._pass = pass;
    m_msg._code = 0;
    m_msg._nname = nname;
    m_msg._rname = rname;
    m_msg._nmem = 0;
    m_msg._ntime = 0;

    s_msg._stype = SIGNALTYPE::sndmsg;
    s_msg._scont = m_msg.ToJson();

    outstr = s_msg.ToJson();
#endif
    return 0;
}

int XMsgProcesser::EncodeGetMsg(std::string& outstr, const std::string& userid, const std::string& token, int tag)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::MeetMsg meet;
    meet.set_msg_tag((pms::EMsgTag)tag);
    meet.set_usr_from(userid);
    meet.set_usr_token(token);

    req.set_svr_cmds(pms::EServerCmd::CGETMSG);
    req.set_mod_type(pms::EModuleType::TMEETING);
    req.set_content(meet.SerializeAsString());
    outstr = req.SerializeAsString();
#else
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::getmsg;
    m_msg._cmd = MEETCMD::meetcmd_invalid;
    m_msg._action = DCOMMACTION::dcommaction_invalid;
    m_msg._tags = SENDTAGS::sendtags_invalid;
    m_msg._type = SENDTYPE::sendtype_invalid;
    m_msg._mseq = GenericTransSeq();
    m_msg._from = userid;
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = pass;
    m_msg._code = 0;
    m_msg._nname = "";
    m_msg._rname = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;

    s_msg._stype = SIGNALTYPE::getmsg;
    s_msg._scont = m_msg.ToJson();

    outstr = s_msg.ToJson();
#endif
    return 0;
}

int XMsgProcesser::EncodeLogout(std::string& outstr, const std::string& userid, const std::string& token)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Logout logout;
    logout.set_usr_from(userid);
    logout.set_usr_token(token);

    req.set_svr_cmds(pms::EServerCmd::CLOGOUT);
    req.set_mod_type(pms::EModuleType::TMEETING);
    req.set_content(logout.SerializeAsString());
    outstr = req.SerializeAsString();
#else
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::logout;
    m_msg._cmd = MEETCMD::meetcmd_invalid;
    m_msg._action = DCOMMACTION::dcommaction_invalid;
    m_msg._tags = SENDTAGS::sendtags_invalid;
    m_msg._type = SENDTYPE::sendtype_invalid;
    m_msg._mseq = 1;
    m_msg._from = userid;
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = pass;
    m_msg._code = 0;
    m_msg._nname = "";
    m_msg._rname = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;

    s_msg._stype = SIGNALTYPE::logout;
    s_msg._scont = m_msg.ToJson();

    outstr = s_msg.ToJson();
#endif
    return 0;
}

int XMsgProcesser::EncodeKeepAlive(std::string& outstr, const std::string& userid)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Keep keep;
    keep.set_usr_from(userid);

    req.set_svr_cmds(pms::EServerCmd::CKEEPALIVE);
    req.set_mod_type(pms::EModuleType::TMEETING);
    req.set_content(keep.SerializeAsString());
    outstr = req.SerializeAsString();
#else
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::keepalive;
    m_msg._cmd = MEETCMD::meetcmd_invalid;
    m_msg._action = DCOMMACTION::dcommaction_invalid;
    m_msg._tags = SENDTAGS::sendtags_invalid;
    m_msg._type = SENDTYPE::sendtype_invalid;
    m_msg._mseq = 2;
    m_msg._from = userid;
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = "";
    m_msg._code = 0;
    m_msg._nname = "";
    m_msg._rname = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;

    s_msg._stype = SIGNALTYPE::keepalive;
    s_msg._scont = m_msg.ToJson();

    outstr = s_msg.ToJson();
#endif
    return 0;
}

/////////////////////////////////////////////////////
///////////////////DECODE MEETMSG////////////////////
/////////////////////////////////////////////////////

int XMsgProcesser::DecodeRecvData(const char* pData, int nLen)
{
#if DEF_PROTO
    const std::string strmsg(pData, nLen);
    pms::MsgRep resp;
    resp.ParseFromString(strmsg);
    std::cout << "XMsgProcess::DecodeRecvData--->" << std::endl;
    resp.PrintDebugString();
    switch (resp.svr_cmds()) {
        case pms::EServerCmd::CLOGIN:
            DecodeLogin(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CSNDMSG:
            DecodeSndMsg(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CGETMSG:
            DecodeGetMsg(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CLOGOUT:
            DecodeLogout(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CKEEPALIVE:
            DecodeKeepAlive(resp.rsp_code(), resp.rsp_cont());
            break;

        default:
        LOG(LS_ERROR) << "invalid svr_cmds type:" << resp.svr_cmds();
            break;
    }
#else
    MEETMSG mmsg;
    std::string err;
    std::string strmsg(pData, nLen);
    mmsg.GetMsg(strmsg, err);
    if (err.length()>0) {
        LOG(LS_ERROR) << "parse MEETMSG err:" << err;
        return -1;
    }
    switch (mmsg._signaltype) {
        case SIGNALTYPE::login:
            DecodeLogin(mmsg);
            break;

        case SIGNALTYPE::sndmsg:
            DecodeSndMsg(mmsg);
            break;

        case SIGNALTYPE::getmsg:
            DecodeGetMsg(mmsg);
            break;

        case SIGNALTYPE::logout:
            DecodeLogout(mmsg);
            break;

        case SIGNALTYPE::keepalive:
            DecodeKeepAlive(mmsg);
            break;

        default:
        LOG(LS_ERROR) << "invalid signaltype type:" << mmsg._signaltype;
            break;
    }
#endif
    return nLen;
}

int XMsgProcesser::DecodeLogin(int code, const std::string& cont)
{
    m_helper.OnLogin(code, cont);
    return 0;
}

int XMsgProcesser::DecodeSndMsg(int code, const std::string& cont)
{
    m_callback.OnSndMsg(cont);
    return 0;
}

int XMsgProcesser::DecodeGetMsg(int code, const std::string& cont)
{
    m_callback.OnGetMsg(cont);
    return 0;
}

int XMsgProcesser::DecodeLogout(int code, const std::string& cont)
{
    m_helper.OnLogout(code, cont);
    return 0;
}

int XMsgProcesser::DecodeKeepAlive(int code, const std::string& cont)
{
    return 0;
}

int XMsgProcesser::GetMemberToJson(const std::list<std::string>& ulist, std::string& tousers)
{
    if (ulist.size()==0) {
        return -1;
    }
    pms::ToUser users;
    std::list<std::string>::const_iterator it = ulist.begin();
    for (int i = 0; it!=ulist.end(); i++,it++) {
        users.set_users(i, (*it));
    }
    tousers = users.SerializeAsString();
    return 0;
}

void XMsgProcesser::ServerConnected()
{
    m_callback.OnMsgServerConnected();
}

void XMsgProcesser::ServerDisconnect()
{
    m_callback.OnMsgServerDisconnect();
}

void XMsgProcesser::ServerConnectionFailure()
{
    m_callback.OnMsgServerConnectionFailure();
}

void XMsgProcesser::ServerState(MSState state)
{
    m_callback.OnMsgServerState(state);
}

