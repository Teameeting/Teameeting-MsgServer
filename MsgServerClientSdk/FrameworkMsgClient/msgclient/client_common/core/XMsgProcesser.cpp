//
//  XMsgProcesser.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "core/XMsgProcesser.h"
#include "core/XMsgClient.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/timeutils.h"
#include <sys/time.h>
#include <time.h>

#ifdef WEBRTC_ANDROID
#include <android/log.h>
#define  LOG_TAG    "XMsgProcesser"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#include <iostream>
#include <string>
#endif

static long long GetSecond()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec;
}

static std::string GetStrMicroSecond()
{
    // memory problem???
    char ct[32] = {0};
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long micro = (long long)tv.tv_sec + (long long)tv.tv_usec/1000;
    sprintf(ct, "%lld", micro);
    printf("gettime ct:%s\n", ct);
    return std::string(ct);
}

int XMsgProcesser::EncodeLogin(std::string& outstr, const std::string& userid, const std::string& token, const std::string& nname, int module)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Login login;
    login.set_usr_from(userid);
    login.set_usr_token(token);
    login.set_usr_nname(nname);
    login.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CLOGIN);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(login.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeSndMsg(std::string& outstr, std::string& outmsgid, const std::string& userid, const std::string& token, const std::string& nname, const std::string& roomid, const std::string& rname, const std::vector<std::string>& to, const std::string& msg, int tag, int type, int module, int flag, int push)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Entity entity;
    entity.set_msg_head(pms::EMsgHead::HSND);
    entity.set_msg_tag((pms::EMsgTag)tag);
    entity.set_msg_type((pms::EMsgType)type);
    entity.set_msg_flag((pms::EMsgFlag)flag);
    entity.set_usr_from(userid);
    entity.set_msg_cont(msg);
    entity.set_rom_id(roomid);
    entity.set_rom_name(rname);
    entity.set_nck_name(nname);
    entity.set_usr_token(token);
    entity.set_msg_time(GetSecond());
    entity.set_version(MSG_VERSION);
    entity.set_ispush(push);

    entity.set_cmsg_id(GetStrMicroSecond());
    outmsgid = entity.cmsg_id();

    printf("XMsgProcesser::EncodeSndMsg to.size:%lu\n", to.size());
    if (to.size()>0) {
        pms::ToUser *touser = entity.mutable_usr_toto();
        for(int i=0;i<(int)to.size();++i) {
            printf("XMsgProcesser::EncodeSndMsg to.name:%s\n", to.at(i).c_str());
            touser->add_users(to.at(i));
        }
    }
    req.set_svr_cmds(pms::EServerCmd::CSNDMSG);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(entity.SerializeAsString());
    outstr = req.SerializeAsString();
    //printf("XMsgProcesser::EncodeSndMsg usr_toto.size:%d\n", entity.usr_toto().users_size());
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeGetMsg(std::string& outstr, const std::string& userid, const std::string& token, int tag, int module)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Entity entity;
    entity.set_msg_tag((pms::EMsgTag)tag);
    entity.set_usr_from(userid);
    entity.set_usr_token(token);
    entity.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CGETMSG);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(entity.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeLogout(std::string& outstr, const std::string& userid, const std::string& token, int module)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Logout logout;
    logout.set_usr_from(userid);
    logout.set_usr_token(token);
    logout.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CLOGOUT);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(logout.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeKeepAlive(std::string& outstr, const std::string& userid, int module)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::Keep keep;
    keep.set_usr_from(userid);
    keep.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CKEEPALIVE);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(keep.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeSyncSeqn(std::string& outstr, const std::string& userid, const std::string& token, int64 seqn, int module, int tag, int flag, int role)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_rsvrcmd(pms::EServerCmd::CSYNCSEQN);
    store.set_tsvrcmd(pms::EServerCmd::CSYNCSEQN);
    store.set_mtag((pms::EStorageTag)(tag));
    store.set_mflag((pms::EMsgFlag)flag);
    store.set_mrole((pms::EMsgRole)role);
    store.set_storeid(userid);
    store.set_ruserid(userid);
    store.set_sequence(seqn);
    store.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CSYNCSEQN);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeSyncData(std::string& outstr, const std::string& userid, const std::string& token, int64 seqn, int module, int tag, int flag)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_rsvrcmd(pms::EServerCmd::CSYNCDATA);
    store.set_tsvrcmd(pms::EServerCmd::CSYNCDATA);
    store.set_mtag((pms::EStorageTag)tag);
    store.set_mflag((pms::EMsgFlag)flag);
    store.set_storeid(userid);
    store.set_ruserid(userid);
    store.set_sequence(seqn);
    store.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CSYNCDATA);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeSyncGroupSeqn(std::string& outstr, const std::string& userid, const std::string& groupid, const std::string& token, int64 seqn, int module, int tag, int flag, int role)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_rsvrcmd(pms::EServerCmd::CSYNCSEQN);
    store.set_tsvrcmd(pms::EServerCmd::CSYNCSEQN);
    store.set_mtag((pms::EStorageTag)tag);
    store.set_mflag((pms::EMsgFlag)flag);
    store.set_mrole((pms::EMsgRole)role);
    store.set_storeid(groupid);
    store.set_ruserid(userid);
    store.set_groupid(groupid);
    store.set_sequence(seqn);
    store.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CSYNCSEQN);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}

int XMsgProcesser::EncodeSyncGroupData(std::string& outstr, const std::string& userid, const std::string& token, const std::string& groupid, int64 seqn, int module, int tag, int flag)
{
#if DEF_PROTO
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_rsvrcmd(pms::EServerCmd::CSYNCGROUPDATA);
    store.set_tsvrcmd(pms::EServerCmd::CSYNCGROUPDATA);
    store.set_mtag((pms::EStorageTag)tag);
    store.set_mflag((pms::EMsgFlag)flag);
    store.set_storeid(groupid);
    store.set_ruserid(userid);
    store.set_groupid(groupid);
    store.set_sequence(seqn);
    store.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CSYNCDATA);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
#else
#endif
    return 0;
}


int XMsgProcesser::EncodeCreateSeqn(std::string& outstr, const std::string& userid, const std::string& storeid, int64 seqn, int module, int tag, int flag)
{
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_rsvrcmd(pms::EServerCmd::CCREATESEQN);
    store.set_tsvrcmd(pms::EServerCmd::CCREATESEQN);
    store.set_mtag((pms::EStorageTag)tag);
    store.set_mflag((pms::EMsgFlag)flag);
    store.set_storeid(storeid);
    store.set_ruserid(userid);
    store.set_sequence(seqn);
    store.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CCREATESEQN);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
    return 0;
}

int XMsgProcesser::EncodeDeleteSeqn(std::string& outstr, const std::string& userid, const std::string& storeid, int64 seqn, int module, int tag, int flag)
{
    pms::MsgReq req;
    pms::StorageMsg store;
    store.set_rsvrcmd(pms::EServerCmd::CDELETESEQN);
    store.set_tsvrcmd(pms::EServerCmd::CDELETESEQN);
    store.set_mtag((pms::EStorageTag)tag);
    store.set_mflag((pms::EMsgFlag)flag);
    store.set_storeid(storeid);
    store.set_ruserid(userid);
    store.set_sequence(seqn);
    store.set_version(MSG_VERSION);

    req.set_svr_cmds(pms::EServerCmd::CDELETESEQN);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(store.SerializeAsString());
    outstr = req.SerializeAsString();
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
        return -1;
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

        case pms::EServerCmd::CGROUPNOTIFY:
            DecodeGroupNotify(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CSYNCGROUPDATA:
            DecodeSyncGroupData(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CSNTFSEQN:
            DecodeNotifySeqn(resp.rsp_code(), resp.rsp_cont());
            break;

        // this event case is no long used
        //case pms::EServerCmd::CSNTFDATA:
        //    DecodeNotifyData(resp.rsp_code(), resp.rsp_cont());
        //    break;

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
    m_helper.OnHelpLogin(code, cont);
    return 0;
}

int XMsgProcesser::DecodeSndMsg(int code, const std::string& cont)
{
    m_helper.OnHelpSndMsg(code, cont);
    return 0;
}

int XMsgProcesser::DecodeGetMsg(int code, const std::string& cont)
{
    return 0;
}

int XMsgProcesser::DecodeLogout(int code, const std::string& cont)
{
    m_helper.OnHelpLogout(code, cont);
    return 0;
}

int XMsgProcesser::DecodeKeepAlive(int code, const std::string& cont)
{
    m_helper.OnHelpKeepLive(code, cont);
    return 0;
}

int XMsgProcesser::DecodeSyncSeqn(int code, const std::string& cont)
{
    m_helper.OnHelpSyncSeqn(code, cont);
    return 0;
}

int XMsgProcesser::DecodeSyncData(int code, const std::string& cont)
{
    m_helper.OnHelpSyncData(code, cont);
    return 0;
}

int XMsgProcesser::DecodeGroupNotify(int code, const std::string& cont)
{
    m_helper.OnHelpGroupNotify(code, cont);
    return 0;
}

int XMsgProcesser::DecodeSyncGroupData(int code, const std::string& cont)
{
    m_helper.OnHelpSyncGroupData(code, cont);
    return 0;
}

int XMsgProcesser::DecodeNotifySeqn(int code, const std::string& cont)
{
    m_helper.OnHelpNotifySeqn(code, cont);
    return 0;
}

int XMsgProcesser::DecodeNotifyData(int code, const std::string& cont)
{
    m_helper.OnHelpNotifyData(code, cont);
    return 0;
}
