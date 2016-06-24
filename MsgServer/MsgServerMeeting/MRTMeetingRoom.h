//
//  MRTMeetingRoom.h
//  MsgServerMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerMeeting__MRTMeetingRoom__
#define __MsgServerMeeting__MRTMeetingRoom__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <string>
#include <set>
#include <list>
#include "LinkedList.h"
#include "refcount.h"
#include "scoped_ptr.h"
#include "scoped_ref_ptr.h"
#include "OSMutex.h"

#define DEF_PROTO
#include "ProtoCommon.h"

class MRTMeetingRoom : public rtc::RefCountedObject< rtc::scoped_ptr<MRTMeetingRoom> >{
public:
    MRTMeetingRoom(const std::string mid, const std::string ownerid);
    ~MRTMeetingRoom();
    typedef struct _notify_msgs{
        pms::EMsgTag    tags;
        std::string notifyMsg;
        std::string publisher;
        _notify_msgs()
            :tags(pms::EMsgTag::TCHAT)
            ,notifyMsg("")
            ,publisher(""){}
    }NotifyMsg;

    typedef enum _member_status{
        MS_NIL=0,
        MS_INMEETING,
        MS_OUTMEETING,
    }MemberStatus;

    typedef std::set<std::string>    RoomMembers;//all the members own this room
    typedef RoomMembers::iterator       RoomMembersIt;
    typedef std::set<std::string>    MeetingMembers;//the members in meeting
    typedef MeetingMembers::iterator    MeetingMembersIt;

    typedef enum _get_members_status {
        GMS_NIL = 0,
        GMS_WAITTING,
        GMS_DONE
    }GetMembersStatus;

    // not sending msgs
    typedef struct _waiting_msg{
        int             _wtype;
        int             _wtags;
        pms::RelayMsg        _wtmsg;
        pms::Entity          _wmsg;
        _waiting_msg(int wtype, int wtags, pms::RelayMsg& wtmsg, pms::Entity& wmsg):
        _wtype(wtype),_wtags(wtags),_wtmsg(wtmsg),_wmsg(wmsg){}
    }WaitingMsg;
    typedef std::list<WaitingMsg> WaitingMsgsList;


    void AddMemberToRoom(const std::string& uid, MemberStatus status);
    bool IsMemberInRoom(const std::string& uid);
    void DelMemberFmRoom(const std::string& uid);
    int  GetRoomMemberNumber() { return (int)m_roomMembers.size(); }

    void AddMemberToMeeting(const std::string& uid);
    bool IsMemberInMeeting(const std::string& uid);
    void DelMemberFmMeeting(const std::string& uid);
    int  GetMeetingMemberNumber() { return (int)m_meetingMembers.size(); }


    void SetGetMembersStatus(GetMembersStatus status){ m_eGetMembersStatus = status; }
    GetMembersStatus GetGetMembersStatus() { return m_eGetMembersStatus; }

    void AddUserToList(const std::string& userid);
    void UpdateMemberList(std::list<std::string>& ulist);

    int GetAllRoomMemberJson(pms::ToUser* users);
    int GetRoomMemberJson(const std::string from, pms::ToUser* users);
    int GetAllMeetingMemberJson(pms::ToUser* users);
    int GetMeetingMemberJson(const std::string from, pms::ToUser* users);
    void ResetSessionId() {
        OSMutexLocker locker(&m_memberMutex);
        if (m_meetingMembers.empty()) {
            m_sessionId = "";
        }
    }

    MemberStatus GetRoomMemberStatus(const std::string& uid);

    void AddWaitingMsgToList(int type, int tag, pms::RelayMsg& tmsg, pms::Entity& emsg);
    WaitingMsgsList& GetWaitingMsgs() { return m_waitingMsgsList; }

    const std::string& GetRoomId() { return m_roomId; }
    const std::string& GetOwnerId() { return m_ownerId; }
    const std::string& GetSessionId() { return m_sessionId; }

public:
    void CheckMembers();

private:

    OSMutex                         m_memberMutex;
    OSMutex                         m_notifyMutex;
    OSMutex                         m_wmsgMutex;
    const std::string               m_roomId;
    const std::string               m_ownerId;
    std::string                     m_sessionId;
    GetMembersStatus                m_eGetMembersStatus;
    RoomMembers                     m_roomMembers;
    MeetingMembers                  m_meetingMembers;
    WaitingMsgsList                 m_waitingMsgsList;

};
#endif /* defined(__MsgServerMeeting__MRTMeetingRoom__) */
