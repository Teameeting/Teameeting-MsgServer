//
//  XGrpMsgProcesser.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XGrpMsgProcesser.h"

#include <iostream>
#include <string>

int XGrpMsgProcesser::EncodeLogin(std::string& outstr, const std::string& userid, const std::string& token, int module)
{

    pms::MsgReq req;
    pms::Login login;
    login.set_usr_from(userid);
    login.set_usr_token(token);

    req.set_svr_cmds(pms::EServerCmd::CLOGIN);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(login.SerializeAsString());
    outstr = req.SerializeAsString();
    return 0;
}

int XGrpMsgProcesser::EncodeLogout(std::string& outstr, const std::string& userid, const std::string& token, int module)
{

    pms::MsgReq req;
    pms::Logout logout;
    logout.set_usr_from(userid);
    logout.set_usr_token(token);

    req.set_svr_cmds(pms::EServerCmd::CLOGOUT);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(logout.SerializeAsString());
    outstr = req.SerializeAsString();
    return 0;
}

int XGrpMsgProcesser::EncodeKeepAlive(std::string& outstr, const std::string& userid, int module)
{
    pms::MsgReq req;
    pms::Keep keep;
    keep.set_usr_from(userid);

    req.set_svr_cmds(pms::EServerCmd::CKEEPALIVE);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(keep.SerializeAsString());
    outstr = req.SerializeAsString();
    return 0;
}

int XGrpMsgProcesser::EncodeGroupNotify(std::string& outstr, const std::string& userid, const std::string& groupid, long long curseqn, int module)
{
    std::vector<std::string> v;
    v.push_back(userid);
    printf("EncodeGroupNotify userid:%s, groupid:%s, curseqn:%lld, module:%d\n", userid.c_str(), groupid.c_str(), curseqn, module);
    EncodeGroupNotifys(outstr, v, groupid, curseqn, module);
    return 0;
}

int XGrpMsgProcesser::EncodeGroupNotifys(std::string& outstr, const std::vector<std::string>& userids, const std::string& groupid, long long curseqn, int module)
{
    pms::MsgReq req;
    printf("EncodeGroupNotifys userids.size:%d\n", userids.size());
    for(int i=0,j=(userids.size()<=PACKED_MSG_NUM_ONCE)?userids.size():PACKED_MSG_NUM_ONCE; i<j;++i)
    {
        if (userids.at(i).length()==0)break;
        m_packed.mutable_msgs(i)->set_svrcmd(pms::EServerCmd::CGROUPNOTIFY);
        m_packed.mutable_msgs(i)->set_mtag(pms::EStorageTag::TDATA);
        m_packed.mutable_msgs(i)->set_userid(userids.at(i));
        m_packed.mutable_msgs(i)->set_groupid(groupid);
        m_packed.mutable_msgs(i)->set_sequence(curseqn);
    }

    req.set_svr_cmds(pms::EServerCmd::CGROUPNOTIFY);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(m_packed.SerializeAsString());
    outstr = req.SerializeAsString();
    for(int i=0;i<PACKED_MSG_NUM_ONCE;++i)
    {
        m_packed.mutable_msgs(i)->Clear();
    }
    return 0;
}

int XGrpMsgProcesser::EncodeSyncDataRequest(std::string& outstr, const std::string& userid, const std::string& groupid, long long curseqn, int module)
{
    std::vector<std::string> v;
    v.push_back(userid);
    printf("EncodeGroupNotify userid:%s, groupid:%s, curseqn:%lld, module:%d\n", userid.c_str(), groupid.c_str(), curseqn, module);
    EncodeSyncDataRequests(outstr, v, groupid, curseqn, module);
    return 0;
}

int XGrpMsgProcesser::EncodeSyncDataRequests(std::string& outstr, const std::vector<std::string>& userids, const std::string& groupid, long long curseqn, int module)
{
    pms::MsgReq req;
    printf("EncodeGroupNotifys userids.size:%d\n", userids.size());
    for(int i=0,j=(userids.size()<=PACKED_MSG_NUM_ONCE)?userids.size():PACKED_MSG_NUM_ONCE; i<j;++i)
    {
        if (userids.at(i).length()==0)break;
        m_packed.mutable_msgs(i)->set_svrcmd(pms::EServerCmd::CGROUPNOTIFY);
        m_packed.mutable_msgs(i)->set_mtag(pms::EStorageTag::TDATA);
        m_packed.mutable_msgs(i)->set_userid(userids.at(i));
        m_packed.mutable_msgs(i)->set_groupid(groupid);
        m_packed.mutable_msgs(i)->set_sequence(curseqn);
    }

    req.set_svr_cmds(pms::EServerCmd::CGROUPNOTIFY);
    req.set_mod_type((pms::EModuleType)module);
    req.set_content(m_packed.SerializeAsString());
    outstr = req.SerializeAsString();
    for(int i=0;i<PACKED_MSG_NUM_ONCE;++i)
    {
        m_packed.mutable_msgs(i)->Clear();
    }
    return 0;
}


/////////////////////////////////////////////////////
///////////////////DECODE MESSAGE////////////////////
/////////////////////////////////////////////////////

int XGrpMsgProcesser::DecodeRecvData(const char* pData, int nLen)
{
    const std::string strmsg(pData, nLen);
    pms::MsgRep resp;
    if (!resp.ParseFromString(strmsg)) {
        std::cout << "RecvData resp.ParseFromString error!" << std::endl;
    }
    switch (resp.svr_cmds()) {
        case pms::EServerCmd::CLOGIN:
            DecodeLogin(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CLOGOUT:
            DecodeLogout(resp.rsp_code(), resp.rsp_cont());
            break;

        case pms::EServerCmd::CKEEPALIVE:
            DecodeKeepAlive(resp.rsp_code(), resp.rsp_cont());
            break;
        case pms::EServerCmd::CGROUPNOTIFY:
            DecodeGroupNotify(resp.rsp_code(), resp.rsp_cont());
            break;

        default:
            std::cout << "invalid svr_cmds type:" << resp.svr_cmds() << std::endl;
            break;
    }
    return nLen;
}

int XGrpMsgProcesser::DecodeLogin(int code, const std::string& cont)
{
    m_helper.OnLogin(code, cont);
    return 0;
}

int XGrpMsgProcesser::DecodeLogout(int code, const std::string& cont)
{
    m_helper.OnLogout(code, cont);
    return 0;
}

int XGrpMsgProcesser::DecodeKeepAlive(int code, const std::string& cont)
{
    m_helper.OnKeepLive(code, cont);
    return 0;
}

int XGrpMsgProcesser::DecodeGroupNotify(int code, const std::string& cont)
{
    m_helper.OnGroupNotify(code, cont);
    return 0;
}

