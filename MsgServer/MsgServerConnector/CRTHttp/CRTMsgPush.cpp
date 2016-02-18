//
//  CRTMsgPush.cpp
//  MsgServer
//
//  Created by hp on 2/18/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "CRTMsgPush.hpp"
#include "RTMeetMsg.h"
#include "CRTConnectionManager.h"

CRTMsgPush::CRTMsgPush()
: RTPush()
{

}

CRTMsgPush::~CRTMsgPush()
{

}

void CRTMsgPush::OnPushEvent(const char* pData, int nLen)
{
    if (!pData || nLen<= 0) {
        LE("CRTMsgPush::OnPushEvent params error\n");
        return;
    }
    LI("CRTMsgPush::OnPushEvent was called...\n");
    MEETMSG m_msg;
    std::string err("");
    m_msg.GetMsg(pData, err);
    if (err.length()>0) {
        LE("CRTMsgPush::OnPushEvent get MeetMsg err:%s\n", err.c_str());
        return;
    }
    printf("CRTMsgPush::OnPushEvent msg tags:%d\n", m_msg._tags);
    if (m_msg._tags == SENDTAGS::sendtags_talk) {
        std::string no = m_msg._rname + ": " + m_msg._nname + " send a msg!";
        LI("CRTMsgPush::OnPushEvent talk cont:%s\n", m_msg._cont.c_str());
        CRTConnectionManager::Instance()->PushMeetingMsg(m_msg._pass, m_msg._room, m_msg._cont, no);
    } else if (m_msg._tags == SENDTAGS::sendtags_enter) {
        std::string no = m_msg._nname + " enter room " + m_msg._rname + " and is waiting for you!";
        LI("CRTMsgPush::OnPushEvent enter cont:%s\n", m_msg._cont.c_str());
        CRTConnectionManager::Instance()->PushMeetingMsg(m_msg._pass, m_msg._room, m_msg._cont, no);
    }
}