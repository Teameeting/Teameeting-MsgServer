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
#include "RTMeetMsg.h"
#include "RTMessage.h"

class MRTMeetingRoom : public rtc::RefCountedObject< rtc::scoped_ptr<MRTMeetingRoom> >{
public:
    MRTMeetingRoom(const std::string mid, const std::string ownerid);
    ~MRTMeetingRoom();
    typedef struct _notify_msgs{
        SENDTAGS    tags;
        std::string notifyMsg;
        std::string publisher;
        _notify_msgs()
            :tags(sendtags_invalid)
            ,notifyMsg("")
            ,publisher(""){}
    }NotifyMsg;

    typedef enum _member_status{
        MS_NIL=0,
        MS_INMEETING,
        MS_OUTMEETING,
    }MemberStatus;

    typedef std::set<std::string>    RoomMembers;
    typedef RoomMembers::iterator       RoomMembersIt;
    typedef std::set<std::string>    MeetingMembers;
    typedef MeetingMembers::iterator    MeetingMembersIt;

    typedef enum _get_members_status {
        GMS_NIL = 0,
        GMS_WAITTING,
        GMS_DONE
    }GetMembersStatus;

    typedef std::unordered_map<std::string, NotifyMsg*>  PublishIdMsgs;
    typedef PublishIdMsgs::iterator PublishIdMsgsIt;
    typedef std::unordered_map<std::string, NotifyMsg*>  AudioSetMsgs;
    typedef AudioSetMsgs::iterator AudioSetMsgsIt;
    typedef std::unordered_map<std::string, NotifyMsg*>  VideoSetMsgs;
    typedef VideoSetMsgs::iterator VideoSetMsgsIt;

    // not sending msgs
    typedef struct _waiting_msg{
        int             _wtype;
        int             _wtags;
        TRANSMSG        _wtmsg;
        MEETMSG         _wmsg;
        _waiting_msg(int wtype, int wtags, TRANSMSG& wtmsg, MEETMSG& wmsg):
        _wtype(wtype),_wtags(wtags),_wtmsg(wtmsg),_wmsg(wmsg){}
    }WaitingMsg;
    typedef std::list<WaitingMsg> WaitingMsgsList;


    void AddMemberToRoom(const std::string& uid, MemberStatus status);
    bool IsMemberInRoom(const std::string& uid);
    void DelMemberFmRoom(const std::string& uid);
    int  GetRoomMemberNumber() { return (int)m_roomMembers.size(); }
    int  GetRoomMemberOnline();
    void AddMemberToMeeting(const std::string& uid);
    void DelMemberFmMeeting(const std::string& uid);

    void SetGetMembersStatus(GetMembersStatus status){ m_eGetMembersStatus = status; }
    GetMembersStatus GetGetMembersStatus() { return m_eGetMembersStatus; }

    void AddUserToList(const std::string& userid);
    void UpdateMemberList(std::list<std::string>& ulist);

    int GetAllRoomMemberJson(std::string& users);
    int GetRoomMemberJson(const std::string from, std::string& users);
    int GetAllMeetingMemberJson(std::string& users);
    int GetMeetingMemberJson(const std::string from, std::string& users);
    void ResetSessionId() {
        OSMutexLocker locker(&m_memberMutex);
        if (m_meetingMembers.empty()) {
            m_sessionId = "";
        }
    }

    bool IsMemberInMeeting(const std::string& uid);
    MemberStatus GetRoomMemberStatus(const std::string& uid);
    
    int AddPublishIdMsg(const std::string pubsher, SENDTAGS tags, const std::string content);
    int DelPublishIdMsg(const std::string pubsher, std::string& pubid);
    PublishIdMsgs& GetPublishIdMsgsMap() { return m_publishIdMsgs; }
    
    int AddAudioSetMsg(const std::string pubsher, SENDTAGS tags, const std::string content);
    int DelAudioSetMsg(const std::string pubsher);
    AudioSetMsgs& GetAudioSetMsgsMap() { return m_audioSetMsgs; }
    
    int AddVideoSetMsg(const std::string pubsher, SENDTAGS tags, const std::string content);
    int DelVideoSetMsg(const std::string pubsher);
    VideoSetMsgs& GetVideoSetMsgsMap() { return m_videoSetMsgs; }

    void AddWaitingMsgToList(int type, int tag, TRANSMSG& tmsg, MEETMSG& mmsg);
    WaitingMsgsList& GetWaitingMsgs() { return m_waitingMsgsList; }

    const std::string& GetRoomId() { return m_roomId; }
    const std::string& GetOwnerId() { return m_ownerId; }
    const std::string& GetSessionId() { return m_sessionId; }

public:
    void CheckMembers();

private:
    void GenericMeetingSessionId(std::string& strId);

    OSMutex                         m_memberMutex;
    OSMutex                         m_notifyMutex;
    OSMutex                         m_wmsgMutex;
    const std::string               m_roomId;
    const std::string               m_ownerId;
    std::string                     m_sessionId;
    GetMembersStatus                m_eGetMembersStatus;
    RoomMembers                     m_roomMembers;
    MeetingMembers                  m_meetingMembers;
    PublishIdMsgs                   m_publishIdMsgs;
    AudioSetMsgs                    m_audioSetMsgs;
    VideoSetMsgs                    m_videoSetMsgs;
    WaitingMsgsList                 m_waitingMsgsList;

};
#endif /* defined(__MsgServerMeeting__MRTMeetingRoom__) */
