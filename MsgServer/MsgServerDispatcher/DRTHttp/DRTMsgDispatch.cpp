//
//  DRTMsgDispatch.cpp
//  MsgServer
//
//  Created by hp on 2/18/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "DRTMsgDispatch.hpp"
#include "RTMeetMsg.h"
#include "DRTConnManager.h"
#include "RTUtils.hpp"
#include "DRTTransferSession.h"

DRTMsgDispatch::DRTMsgDispatch()
: RTDispatch()
{

}

DRTMsgDispatch::~DRTMsgDispatch()
{

}

void DRTMsgDispatch::OnPushEvent(const char* pData, int nLen)
{
#if DEF_PROTO
    if (!pData || nLen<= 0) {
        LE("DRTMsgDispatch::OnPushEvent params error\n");
        return;
    }
    std::string msg(pData, nLen);
    pms::RelayMsg pmsg;
    pms::MsgRep respmsg;
    pms::MeetMsg mmsg;
    if (!pmsg.ParseFromString(msg)) {
        LE("pmsg.ParseFromString error\n");
        return;
    }
    if (!respmsg.ParseFromString(pmsg.content())) {
        LE("respmsg.ParseFromString error\n");
        return;
    }
    if (!mmsg.ParseFromString(respmsg.rsp_cont())) {
        LE("mmsg.ParseFromString error\n");
        return;
    }
    pmsg.PrintDebugString();
    respmsg.PrintDebugString();
    mmsg.PrintDebugString();

    TOPUSHUSER pushUser;
    pms::ToUser toUser = pmsg.touser();
    for(int i=0;i<toUser.users_size();++i) {
        pushUser._us.push_back((toUser.users(i)));
    }

    TOPUSHMSG pushMsg;
    pushMsg._tags = mmsg.msg_tag();
    pushMsg._roomid = mmsg.rom_id();
    std::string strPushMsg = pushMsg.ToJson();
    printf("mmsg.romd_id:%s\n", mmsg.rom_id().c_str());
    printf("mmsg.usr_from:%s\n", mmsg.usr_from().c_str());
    printf("pushUser.ToJson:%s\n", pushUser.ToJson().c_str());
    printf("mmsg.msg_cont:%s\n", mmsg.msg_cont().c_str());
    printf("strPushMsg:%s\n", strPushMsg.c_str());

    if (mmsg.msg_tag() == pms::EMsgTag::TCHAT) {
        std::string no = mmsg.rom_name() + " - " + mmsg.nck_name() + ": " + mmsg.msg_cont();
        DRTConnManager::Instance().PushMeetingMsg(mmsg.rom_id(), mmsg.usr_from(), pushUser.ToJson(), mmsg.msg_cont(), no, strPushMsg);
    } else if (mmsg.msg_tag() == pms::EMsgTag::TENTER) {
        std::string no = mmsg.nck_name() + "进入\"" + mmsg.rom_name() + "\"房间正在等你哦~";
        DRTConnManager::Instance().PushMeetingMsg(mmsg.rom_id(), mmsg.usr_from(), pushUser.ToJson(), mmsg.msg_cont(), no, strPushMsg);
    } else if (mmsg.msg_tag() == pms::EMsgTag::TLEAVE) {
        //std::string no = mmsg.nck_name() + "离开房间\"" + mmsg.rom_name() + "\"";
        //DRTConnManager::Instance().PushMeetingMsg(mmsg.rom_id(), mmsg.usr_from(), pushUser.ToJson(), mmsg.msg_cont(), no, strPushMsg);
    } else if (mmsg.msg_tag() == pms::EMsgTag::TNOTIFY) {
        std::string no = mmsg.nck_name() + "喊你进房间\"" + mmsg.rom_name() + "\"啦~";
        DRTConnManager::Instance().PushMeetingMsg(mmsg.rom_id(), mmsg.usr_from(), pushUser.ToJson(), mmsg.msg_cont(), no, strPushMsg);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void DRTMsgDispatch::OnSendEvent(const char*pData, int nLen)
{
#if DEF_PROTO
    if (!pData || nLen<=0) {
        return;
    }
    std::string msg(pData, nLen);
    pms::RelayMsg dmsg;
    if (!dmsg.ParseFromString(msg)) {
        LE("dmsg.ParseFromString error\n");
        return;
    }
    pms::TransferMsg trmsg;
    trmsg.set_type(pms::ETransferType::TDISPATCH);
    trmsg.set_content(msg);

    std::string st = trmsg.SerializeAsString();

    DRTConnManager::ModuleInfo* pmi = DRTConnManager::Instance().findConnectorInfoById("notnull", dmsg.connector());
    if (pmi && pmi->pModule) {
        pmi->pModule->SendTransferData(st.c_str(), (int)st.length());
    } else {
        LE("mi.pModule is NULL\n");
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}
