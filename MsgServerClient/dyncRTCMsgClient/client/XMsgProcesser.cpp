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
#endif
    return 0;
}

int XMsgProcesser::EncodeSndMsg(std::string& outstr, const std::string& userid, const std::string& token, const std::string& nname, const std::string& roomid, const std::string& rname, const std::vector<std::string>& to, const std::string& msg, int tag, int type)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::MeetMsg meet;
    pms::ToUser *touser = meet.mutable_usr_toto();
    meet.set_msg_head(pms::EMsgHead::HSND);
    meet.set_msg_tag((pms::EMsgTag)tag);
    meet.set_msg_type((pms::EMsgType)type);
    meet.set_usr_from(userid);
    meet.set_msg_cont(msg);
    meet.set_rom_id(roomid);
    meet.set_rom_name(rname);
    meet.set_nck_name(nname);
    meet.set_usr_token(token);
    for(int i=0;i<(int)to.size();++i) {
         touser->add_users(to.at(i));
    }

    req.set_svr_cmds(pms::EServerCmd::CSNDMSG);
    req.set_mod_type(pms::EModuleType::TMEETING);
    req.set_content(meet.SerializeAsString());
    outstr = req.SerializeAsString();
#else
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
#endif
    return 0;
}

int XMsgProcesser::EncodeSyncSeqn(std::string& outstr, const std::string& userid, const std::string& token, long long seqn)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_mflag(pms::EStorageType::TREAD);
    store.set_userid(userid);
    store.set_sequence(seqn);

    printf("XMsgProcesser::EncodeSyncSeqn was called\n");
    req.set_svr_cmds(pms::EServerCmd::CSYNCSEQN);
    req.set_mod_type(pms::EModuleType::TLIVE);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeSyncData(std::string& outstr, const std::string& userid, const std::string& token, long long seqn)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_mflag(pms::EStorageType::TREAD);
    store.set_userid(userid);
    store.set_sequence(seqn);

    printf("XMsgProcesser::EncodeSyncSeqn was called\n");
    req.set_svr_cmds(pms::EServerCmd::CSYNCDATA);
    req.set_mod_type(pms::EModuleType::TLIVE);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
#else
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
    if (!resp.ParseFromString(strmsg)) {
        LOG(LS_ERROR) << "RecvData resp.ParseFromString error!";
    }
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
        case pms::EServerCmd::CSYNCSEQN:
            DecodeSyncSeqn(resp.rsp_code(), resp.rsp_cont());
            break;
        case pms::EServerCmd::CSYNCDATA:
            DecodeSyncData(resp.rsp_code(), resp.rsp_cont());
            break;

        default:
            LOG(LS_ERROR) << "invalid svr_cmds type:" << resp.svr_cmds();
            break;
    }
#else
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

int XMsgProcesser::DecodeSyncSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("DecodeSyncSeqn userid:%s, sequence:%lld\n", store.userid().c_str(), store.sequence());
    return 0;
}

int XMsgProcesser::DecodeSyncData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    store.ParseFromString(cont);
    printf("DecodeSyncData userid:%s, sequence:%lld, content:%s\n\n", store.userid().c_str(), store.sequence(), store.content().c_str());
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

