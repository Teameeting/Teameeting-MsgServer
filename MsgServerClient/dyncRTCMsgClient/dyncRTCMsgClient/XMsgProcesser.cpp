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

static long long  g_msgs_id = 3;

int XMsgProcesser::EncodeLogin(std::string& outstr, const std::string& userid, const std::string& pass)
{
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::login;
    m_msg._cmd = 0;
    m_msg._action = 0;
    m_msg._tags = 0;
    m_msg._type = 0;
    m_msg._mseq = 0;
    m_msg._from = userid;
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = pass;
    m_msg._code = 0;
    m_msg._status = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;
    
    s_msg._stype = SIGNALTYPE::login;
    s_msg._scont = m_msg.ToJson();
    
    outstr = s_msg.ToJson();
    return 0;
}

int XMsgProcesser::EncodeSndMsg(std::string& outstr, const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& to, const std::string& msg, int cmd, int action, int tags, int type)
{
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
    m_msg._status = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;
    
    s_msg._stype = SIGNALTYPE::sndmsg;
    s_msg._scont = m_msg.ToJson();
    
    outstr = s_msg.ToJson();
    return 0;
}

int XMsgProcesser::EncodeGetMsg(std::string& outstr, const std::string& userid, const std::string& pass, int cmd)
{
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::getmsg;
    m_msg._cmd = 0;
    m_msg._action = 0;
    m_msg._tags = 0;
    m_msg._type = 0;
    m_msg._mseq = GenericTransSeq();
    m_msg._from = userid;
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = pass;
    m_msg._code = 0;
    m_msg._status = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;
    
    s_msg._stype = SIGNALTYPE::getmsg;
    s_msg._scont = m_msg.ToJson();
    
    outstr = s_msg.ToJson();
    return 0;
}

int XMsgProcesser::EncodeLogout(std::string& outstr, const std::string& userid, const std::string& pass)
{
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::logout;
    m_msg._cmd = 0;
    m_msg._action = 0;
    m_msg._tags = 0;
    m_msg._type = 0;
    m_msg._mseq = 1;
    m_msg._from = userid;
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = pass;
    m_msg._code = 0;
    m_msg._status = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;
    
    s_msg._stype = SIGNALTYPE::logout;
    s_msg._scont = m_msg.ToJson();
    
    outstr = s_msg.ToJson();
    return 0;
}

int XMsgProcesser::EncodeKeepAlive(std::string& outstr)
{
    SIGNALMSG s_msg;
    MEETMSG m_msg;
    m_msg._mtype = MSGTYPE::meeting;
    m_msg._messagetype = MESSAGETYPE::request;
    m_msg._signaltype = SIGNALTYPE::keepalive;
    m_msg._cmd = 0;
    m_msg._action = 0;
    m_msg._tags = 0;
    m_msg._type = 0;
    m_msg._mseq = 2;
    m_msg._from = "aliver";
    m_msg._room = "";
    m_msg._to = "";
    m_msg._cont = "";
    m_msg._pass = "";
    m_msg._code = 0;
    m_msg._status = "";
    m_msg._nmem = 0;
    m_msg._ntime = 0;
    
    s_msg._stype = SIGNALTYPE::keepalive;
    s_msg._scont = m_msg.ToJson();
    
    outstr = s_msg.ToJson();
    return 0;
}

/////////////////////////////////////////////////////
///////////////////DECODE MEETMSG////////////////////
/////////////////////////////////////////////////////

int XMsgProcesser::DecodeRecvData(const char* pData, int nLen)
{
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
    return nLen;
}

int XMsgProcesser::DecodeLogin(MEETMSG& msg)
{
    if (msg._code == 0) {
        ServerState(MSCONNECTED);
        ServerConnected();
    } else {
        m_helper.OnLogin(msg._code, msg._status, msg._from);
    }
    return 0;
}

int XMsgProcesser::DecodeSndMsg(MEETMSG& msg)
{
    const std::string tmsg(msg.ToJson());
    m_callback.OnSndMsg(tmsg);
    return 0;
}

int XMsgProcesser::DecodeGetMsg(MEETMSG& msg)
{
    const std::string tmsg(msg.ToJson());
    m_callback.OnGetMsg(tmsg);
    return 0;
}

int XMsgProcesser::DecodeLogout(MEETMSG& msg)
{
    
    return 0;
}

int XMsgProcesser::DecodeKeepAlive(MEETMSG& msg)
{
    return 0;
}

int XMsgProcesser::GetMemberToJson(const std::list<std::string> ulist, std::string& tousers)
{
    if (ulist.size()==0) {
        return -1;
    }
    TOJSONUSER touser;
    LOG(INFO) << "to user list size:" << ulist.size();
    std::list<std::string>::const_iterator it = ulist.begin();
    for (; it!=ulist.end(); it++) {
        touser._us.push_front((*it).c_str());
    }
    tousers = touser.ToJson();
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

void XMsgProcesser::ServerState(MSTcpState state)
{
    m_callback.OnMsgServerState(state);
}

long long XMsgProcesser::GenericTransSeq()
{
    return ++g_msgs_id;
}

