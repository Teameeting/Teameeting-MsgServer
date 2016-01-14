//
//  RTMeetingRoom.h
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMeeting__RTMeetingRoom__
#define __dyncRTMeeting__RTMeetingRoom__

#include <stdio.h>
#include <iostream>
#ifdef WEBRTC_MAC
#include <ext/hash_map>
#else
#include <map>
#endif
#include <string>
#include <list>
#include "LinkedList.h"
#include "refcount.h"
#include "scoped_ptr.h"
#include "scoped_ref_ptr.h"
#include "OSMutex.h"
#include "RTMeetMsg.h"

class RTMeetingRoom : public rtc::RefCountedObject< rtc::scoped_ptr<RTMeetingRoom> >{
public:
    RTMeetingRoom(const std::string mid, const std::string ownerid);
    ~RTMeetingRoom();
    
    typedef struct _notify_msgs{
        int seq;
        long long pubTime;
        std::string notifyMsg;
        std::string publisher;
        _notify_msgs() {
            seq = 0;
            pubTime = 0;
            notifyMsg = "";
            publisher = "";
        }
    }NotifyMsg;
    
    typedef enum _member_status{
        MS_NIL=0,
        MS_INMEETING,
        MS_OUTMEETING,
    }MemberStatus;
    
    typedef struct _room_member{
        std::string _uid;
        MemberStatus  _memStatus;
        _room_member(const std::string& uid, MemberStatus memStatus) {
            _uid = uid;
            _memStatus = memStatus;
        }
    }RoomMember;
#ifdef WEBRTC_MAC
    typedef __gnu__cxx::hashmap<const std::string, RoomMember*> RoomMembers;
#else
    typedef std::map<const std::string, RoomMember*> RoomMembers;
#endif
    
    typedef enum _get_members_status {
        GMS_NIL = 0,
        GMS_WAITTING,
        GMS_DONE
    }GetMembersStatus;
    
#ifdef WEBRTC_MAC
    typedef __gnu__cxx::hashmap<std::string, NotifyMsg*>  RoomNotifyMsgs;
#else
    typedef std::map<std::string, NotifyMsg*>  RoomNotifyMsgs;
#endif
    
    
    void AddMemberToRoom(const std::string& uid, MemberStatus status);
    void SyncRoomMember(const std::string& uid, MemberStatus status);
    void UpdateMemberStatus(const std::string& uid, MemberStatus status);
    bool IsMemberInRoom(const std::string& uid);
    void DelMemberFmRoom(const std::string& uid);
    int  GetRoomMemberNumber() { return (int)m_roomMembers.size(); }
    int  GetRoomMemberOnline();
    
    void SetGetMembersStatus(GetMembersStatus status){ m_eGetMembersStatus = status; }
    GetMembersStatus GetGetMembersStatus() { return m_eGetMembersStatus; }
    
    void AddUserToList(const std::string& userid);
    void UpdateMemberList(std::list<const std::string>& ulist);
    
    int GetRoomMemberJson(const std::string from, std::string& users);
    int GetRoomMemberMeetingJson(const std::string from, std::string& users);
    
    int AddNotifyMsg(const std::string pubsher, const std::string pubid);
    int DelNotifyMsg(const std::string pubsher, std::string& pubid);
    RoomNotifyMsgs& GetRoomNotifyMsgsMap() { return m_roomNotifyMsgs; }
    void AddMsgToWaiting(MEETMSG msg);
    void SendWaitingMsgs();
    
    const std::string& GetRoomId() { return m_roomId; }
    const std::string& GetOwnerId() { return m_ownerId; }
    const std::string& GetSessionId() { return m_sessionId; }
    
public:
    void CheckMembers();
    
private:
    int GenericNotifySeq();
    void GenericMeetingSessionId(std::string& strId);
    
    OSMutex                         m_mutex;
    OSMutex                         m_notifyMutex;
    OSMutex                         m_waitingMutex;
    const std::string               m_roomId;
    const std::string               m_ownerId;
    std::string                     m_sessionId;
    GetMembersStatus                m_eGetMembersStatus;
    RoomMembers                     m_roomMembers;
    int                             m_maxRoomMem;
    RoomNotifyMsgs                  m_roomNotifyMsgs;
    std::list<MEETMSG>              m_waitingMsgs;
    
};
#endif /* defined(__dyncRTMeeting__RTMeetingRoom__) */
