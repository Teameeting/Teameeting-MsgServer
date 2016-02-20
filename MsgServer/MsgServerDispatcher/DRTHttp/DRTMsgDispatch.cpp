//
//  DRTMsgDispatch.cpp
//  MsgServer
//
//  Created by hp on 2/18/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "DRTMsgDispatch.hpp"
#include "RTMeetMsg.h"
#include "DRTConnectionManager.h"
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
    LI("DRTMsgDispatch::OnPushEvent was called...\n");
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
    
    TOPUSHMSG pushMsg;
    pushMsg._roomid = mmsg._room;
    std::string strPushMsg = pushMsg.ToJson();
    
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
    
    printf("DRTMsgDispatch::OnPushEvent msg tags:%d\n", mmsg._tags);
    if (mmsg._tags == SENDTAGS::sendtags_talk) {
        std::string no = mmsg._rname + " - " + mmsg._nname + ": " + mmsg._cont;
        LI("DRTMsgDispatch::OnPushEvent talk cont:%s\n", mmsg._cont.c_str());
        DRTConnectionManager::Instance()->PushCommonMsg(mmsg._pass, pushUser.ToJson(), mmsg._cont, no, strPushMsg);
    } else if (mmsg._tags == SENDTAGS::sendtags_enter) {
        std::string no = mmsg._nname + " enter room " + mmsg._rname + " and is waiting for you!";
        LI("DRTMsgDispatch::OnPushEvent enter cont:%s\n", mmsg._cont.c_str());
        DRTConnectionManager::Instance()->PushCommonMsg(mmsg._pass, pushUser.ToJson(), mmsg._cont, no, strPushMsg);
    } else if (mmsg._tags == SENDTAGS::sendtags_leave) {
        
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
    
    DRTConnectionManager::ModuleInfo* pmi = DRTConnectionManager::Instance()->findConnectorInfoById("notnull", dmsg._connector);
    if (pmi && pmi->pModule) {
        pmi->pModule->SendTransferData(st.c_str(), (int)st.length());
    } else {
        LE("mi.pModule is NULL\n");
    }
}