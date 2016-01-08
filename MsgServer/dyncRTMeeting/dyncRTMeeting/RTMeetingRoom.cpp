//
//  RTMeetingRoom.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <algorithm>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "RTMeetingRoom.h"
#include "rtklog.h"
#include "atomic.h"
#include "RTRoomManager.h"

static unsigned int	 s_notify_msg_id = 0;

int RTMeetingRoom::GenericNotifySeq()
{
    return atomic_add(&s_notify_msg_id, 1);
}

RTMeetingRoom::RTMeetingRoom(const std::string mid, const std::string ownerid)
: m_roomId(mid)
, m_sessionId("")
, m_ownerId(ownerid)
, m_pRoomSession(NULL)
{
    ListZero(&m_roomMemList);
    //ListAppend(&m_roomMemList, (void*)m_ownerId.c_str(), (int)m_ownerId.length());
}

RTMeetingRoom::~RTMeetingRoom()
{
    if (m_pRoomSession) {
        delete m_pRoomSession;
        m_pRoomSession = NULL;
    }
    ListEmpty(&m_roomMemList);
}

bool RTMeetingRoom::AddMemberToRoom(const std::string uid)
{
    OSMutexLocker locker(&m_mutex);
    if (IsMemberInRoom(uid)) {
        LI("user: %s ALREADY in room: %s\n", uid.c_str(), m_roomId.c_str());
        return false;
    } else {
        const char* str = (const char*)strdup(uid.c_str());
        ListAppend(&m_roomMemList, (void*)str, (int)strlen(str));
        LI("user: %s ADDOK room: %s\n", uid.c_str(), m_roomId.c_str());
        return true;
    }
}

bool RTMeetingRoom::IsMemberInRoom(const std::string uid)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMemList.count==0) {
        return false;
    }
    return (!!ListFindItem(&m_roomMemList, (void*)uid.c_str(), stringcompare));
}


bool RTMeetingRoom::DelMemberFmRoom(const std::string uid)
{
    OSMutexLocker locker(&m_mutex);
    if (IsMemberInRoom(uid)) {
        if (ListRemoveItem(&m_roomMemList, (void*)uid.c_str(), stringcompare)) {
            LI("user: %s DELOK room: %s\n", uid.c_str(), m_roomId.c_str());
            return true;
        } else {
            LI("user: %s DEL FAILED room: %s\n", uid.c_str(), m_roomId.c_str());
            return false;
        }
    } else {
        LI("user: %s NOT in room: %s\n", uid.c_str(), m_roomId.c_str());
        return false;
    }
}

bool RTMeetingRoom::AddMemberToSession(const std::string sid, const std::string uid)
{
    return m_pRoomSession->AddMemberToSession(uid);
}

bool RTMeetingRoom::DelMemberFmSession(const std::string sid, const std::string uid)
{
    return m_pRoomSession->DelMemberFmSession(uid);
}

int RTMeetingRoom::GetRoomMemberInJson(const std::string from, std::string& users)
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMemList.count==0) {
        return -1;
    }
    ListElement* elem = NULL;
    TOJSONUSER touser;
    LI("room members:%d\n", m_roomMemList.count);
    while (ListNextElement(&m_roomMemList, &elem)!=NULL) {
        LI("\t-->MeetingRoom members :%s\n", (char*)elem->content);
        if (strcmp(from.c_str(), (const char*)elem->content)!=0) {
            touser._us.push_front((const char*)elem->content);
        }
    }
    users = touser.ToJson();
    return 0;
}

void RTMeetingRoom::CreateSession()
{
    OSMutexLocker  locker(&m_mutex);
    if (m_pRoomSession) {
        return;
    }
    if (m_pRoomSession == NULL) {
        RTRoomManager::Instance()->GenericMeetingSessionId(m_sessionId);
        m_pRoomSession = new RTRoomSession(m_sessionId);
    }
}

void RTMeetingRoom::DestroySession()
{
    OSMutexLocker  locker(&m_mutex);
    if (!m_pRoomSession) {
        return;
    }
    if (m_pRoomSession->GetSessionMemberNumber() == 0) {
        delete m_pRoomSession;
        m_pRoomSession = NULL;
    }
}

void RTMeetingRoom::UpdateUserList(std::list<const std::string>& ulist)
{
    OSMutexLocker locker(&m_mutex);
    LI("before UpdateUserList count:%d\n", m_roomMemList.count);
    if (ulist.size() == m_roomMemList.count) {
        LI("UpdateUserList equal mem count:%d\n", m_roomMemList.count);
        return;
    } else if (ulist.size()>m_roomMemList.count) {
        std::list<const std::string>::iterator ait = ulist.begin();
        for (; ait!=ulist.end(); ait++) {
            AddMemberToRoom((*ait));
        }
        LI("UpdateUserList add mem count:%d\n", m_roomMemList.count);
    } else if (ulist.size()<m_roomMemList.count) {
        ListElement* elem = NULL;
        std::list<const std::string> tmpList;
        while (ListNextElement(&m_roomMemList, &elem)!=NULL) {
            LI("\t-->MeetingRoom members :%s\n", (char*)elem->content);
            if ((char*)elem->content) {
                std::list<const std::string>::iterator t = std::find(ulist.begin(), ulist.end(), (char*)elem->content);
                if (t==ulist.end()) {
                    tmpList.push_back((char*)elem->content);
                }
            }
        }
        std::list<const std::string>::iterator dit = tmpList.begin();
        for (; dit!=tmpList.end(); dit++) {
            DelMemberFmRoom((*dit));
        }
        LI("UpdateUserList del mem count:%d\n", m_roomMemList.count);
    }
}

void RTMeetingRoom::CheckMembers()
{
    OSMutexLocker locker(&m_mutex);
    if (m_roomMemList.count==0) {
        return;
    }
    ListElement* elem = NULL;
    LI("MeetingRoom members:%d\n", m_roomMemList.count);
    while (ListNextElement(&m_roomMemList, &elem)!=NULL) {
        LI("\t-->MeetingRoom members :%s\n", (char*)elem->content);
    }
}

int RTMeetingRoom::AddNotifyMsg(const std::string pubsher, const std::string msg)
{
    RoomNotifyMsgs::iterator it = m_roomNotifyMsgs.find(pubsher);
    if (it == m_roomNotifyMsgs.end()) {
        std::list<NotifyMsg*>* msgList = new std::list<NotifyMsg*>();
        NotifyMsg* noMsg = new NotifyMsg();
        noMsg->seq = GenericNotifySeq();
        noMsg->pubTime = OS::Milliseconds();
        noMsg->notifyMsg = msg;
        noMsg->notified.push_back(pubsher);
        msgList->push_back(noMsg);
        m_roomNotifyMsgs.insert(make_pair(pubsher, msgList));
        LI("RTMeetingRoom::AddNotifyMsg pubsher:%s, msg:%s\n", pubsher.c_str(), msg.c_str());
    } else {
        
    }
    
    return 0;
}

int RTMeetingRoom::DelNotifyMsg(int msgid)
{
    return 0;
}

int RTMeetingRoom::ShouldNotify(const std::string userid, std::string& msgNo, std::string& msgPub)
{
    RoomNotifyMsgs::iterator rit = m_roomNotifyMsgs.begin();
    for (; rit!=m_roomNotifyMsgs.end(); rit++) {
        std::string publisher = rit->first;
        std::list<NotifyMsg*>* l = rit->second;
        LI("RTMeetingRoom::ShouldNotify 1 find notify msg:%s\n", msgNo.c_str());
        if (l) {
            std::list<NotifyMsg*>::iterator lit = l->begin();
            LI("RTMeetingRoom::ShouldNotify 2 find notify msg:%s\n", msgNo.c_str());
            for (; lit!=l->end(); lit++) {
                NotifyMsg* pMsg = *lit;
                LI("RTMeetingRoom::ShouldNotify 3 find notify msg:%s\n", msgNo.c_str());
                if (pMsg) {
                    LI("RTMeetingRoom::ShouldNotify 4 find notify msg:%s\n", msgNo.c_str());
                    std::list<std::string>::iterator mit = std::find(pMsg->notified.begin(), pMsg->notified.end(), userid);
                    if (mit==pMsg->notified.end()) {
                        msgNo = pMsg->notifyMsg;
                        msgPub = publisher;
                        LI("RTMeetingRoom::ShouldNotify find notify msg:%s\n", msgNo.c_str());
                        return pMsg->seq;
                    }
                }
            }
        }
    }
    LI("RTMeetingRoom::ShouldNotify did not find notify msg\n");
    return 0;
}

int RTMeetingRoom::Notify(const std::string userid, std::string& msg)
{
    return 0;
}

void RTMeetingRoom::NotifyDone(const std::string userid, std::string& msgPub, int msgid)
{
    RoomNotifyMsgs::iterator rit = m_roomNotifyMsgs.find(msgPub);
    if (rit!=m_roomNotifyMsgs.end()) {
        std::list<NotifyMsg*>* l = rit->second;
        if (l) {
            std::list<NotifyMsg*>::iterator lit = l->begin();
            for (; lit!=l->end(); lit++) {
                NotifyMsg* pMsg = *lit;
                if (pMsg && (pMsg->seq == msgid)) {
                    pMsg->notified.push_back(userid);
                    LE("RTMeetingRoom::NotifyDone [success]:userid:%s, msgPub:%s, msgid:%d", userid.c_str(), msgPub.c_str(), msgid);
                    return;
                }
            }
        }
    }
    LE("RTMeetingRoom::NotifyDone [failed]:userid:%s, msgPub:%s, msgid:%d", userid.c_str(), msgPub.c_str(), msgid);
    return;
}
