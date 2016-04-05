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
    if (!pData || nLen<= 0) {
        LE("DRTMsgDispatch::OnPushEvent params error\n");
        return;
    }
    PUSHMSG pmsg;
    MEETMSG mmsg;
    std::string err("");
    pmsg.GetMsg(pData, err);
    if (err.length()>0) {
        LE("DRTMsgDispatch::OnPushEvent get PushMsg err:%s\n", err.c_str());
        return;
    }
    mmsg.GetMsg(pmsg._content, err);
    if (err.length()>0) {
        LE("DRTMsgDispatch::OnPushEvent get MeetMsg err:%s\n", err.c_str());
        return;
    }

#if 1
    TOPUSHUSER pushUser;
    TOJSONUSER jsonUser;
    jsonUser.GetMsg(pmsg._touser, err);
    if (err.length()>0) {
        LE("DRTMsgDispatch::OnPushEvent get JsonUser err:%s\n", err.c_str());
        return;
    }
    std::list<std::string>::iterator it = jsonUser._us.begin();
    for (; it!=jsonUser._us.end(); it++) {
        pushUser._us.push_back((*it));
    }
#endif

    TOPUSHMSG pushMsg;
    pushMsg._tags = mmsg._tags;
    pushMsg._roomid = mmsg._room;
    std::string strPushMsg = pushMsg.ToJson();
    printf("pushUser.ToJson:%s\n", pushUser.ToJson().c_str());

    if (mmsg._tags == SENDTAGS::sendtags_talk) {
        std::string no = mmsg._rname + " - " + mmsg._nname + ": " + mmsg._cont;
        //DRTConnManager::Instance().PushCommonMsg(mmsg._pass, pushUser.ToJson(), mmsg._cont, no, strPushMsg);
        DRTConnManager::Instance().PushMeetingMsg(mmsg._room, mmsg._from, pushUser.ToJson(), mmsg._cont, no, strPushMsg);
    } else if (mmsg._tags == SENDTAGS::sendtags_enter) {
        std::string no = mmsg._nname + "进入\"" + mmsg._rname + "\"房间正在等你哦~";
        //DRTConnManager::Instance().PushCommonMsg(mmsg._pass, pushUser.ToJson(), mmsg._cont, no, strPushMsg);
        DRTConnManager::Instance().PushMeetingMsg(mmsg._room, mmsg._from, pushUser.ToJson(), mmsg._cont, no, strPushMsg);
    } else if (mmsg._tags == SENDTAGS::sendtags_leave) {
        //std::string no = mmsg._nname + "离开房间\"" + mmsg._rname + "\"";
        //DRTConnManager::Instance().PushCommonMsg(mmsg._pass, pushUser.ToJson(), mmsg._cont, no, strPushMsg);
        //DRTConnManager::Instance().PushCommonMsg(mmsg._pass, mmsg._room, mmsg._cont, no, strPushMsg);
    }
}

void DRTMsgDispatch::OnSendEvent(const char*pData, int nLen)
{
    if (!pData || nLen<=0) {
        return;
    }
    DISPATCHMSG dmsg;
    std::string err("");
    dmsg.GetMsg(pData, err);
    if (err.length()>0) {
        LE("DRTMsgDispatch::OnSendEvent get DispatchMsg err:%s\n", err.c_str());
        return;
    }
    TRANSFERMSG trmsg;
    trmsg._action = TRANSFERACTION::req;
    trmsg._fmodule = TRANSFERMODULE::mmsgqueue;
    trmsg._type = TRANSFERTYPE::dispatch;
    trmsg._trans_seq = GenericTransSeq();
    trmsg._trans_seq_ack = 0;
    trmsg._valid = 1;
    std::string sd = pData;
    trmsg._content = sd;
    std::string st = trmsg.ToJson();

    DRTConnManager::ModuleInfo* pmi = DRTConnManager::Instance().findConnectorInfoById("notnull", dmsg._connector);
    if (pmi && pmi->pModule) {
        pmi->pModule->SendTransferData(st.c_str(), (int)st.length());
    } else {
        LE("mi.pModule is NULL\n");
    }
}
