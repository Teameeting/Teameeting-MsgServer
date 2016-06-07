//
//  RTHttpClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTHttpClient.hpp"
#include "RTJson.hpp"

RTHttpClient::RTHttpClient()
: mCurlClient("http://192.168.7.218:8055")
, mUserid("")
, mAuth("")
, mUname("")
, mPushToken(""){

}

RTHttpClient::~RTHttpClient()
{

}

int RTHttpClient::UserInit(const std::string& uid)
{
    std::string resp;
    mCurlClient.RTUserInit(uid, "0", "0", "0", "pushtoken", resp);
    HttpRespUserInit respUserInit;
    std::string err("");
    respUserInit.GetMsg(resp, err);
    if (err.length()>0) {
        printf("get response body userinit error:%s\n", err.c_str());
        return -1;
    }
    printf("bodyInfo userinit userid:%s, auth:%s, uname:%s\n"
           , respUserInit._information._userid.c_str()
           , respUserInit._authorization.c_str()
           , respUserInit._information._uname.c_str());

    mUserid = respUserInit._information._userid;
    mAuth = respUserInit._authorization;
    mUname = respUserInit._information._uname;
    mPushToken = respUserInit._information._upushtoken;
    return 0;
}

int RTHttpClient::ApplyRoom()
{
    std::string resp;
    mCurlClient.RTApplyRoom(mAuth, "msgtest", "0", "0", "1", "test msg", resp);
    printf("RTHttpClient::ApplyRoom auth:%s, resp:%s\n", mAuth.c_str(), resp.c_str());
    HttpRespApplyRoom respApplyRoom;
    std::string err("");
    respApplyRoom.GetMsg(resp, err);
    if (err.length()>0) {
        printf("get response body applyroom err:%s\n", err.c_str());
        return -1;
    }
    printf("respbody applyroom meetingid:%ld, aynrtcid:%ld, meetname:%s\n"
           , respApplyRoom._meetingInfo._meetingid
           , respApplyRoom._meetingInfo._anyrtcid
           , respApplyRoom._meetingInfo._meetname.c_str());
    char mid[16] = {0};
    sprintf(mid, "%ld", respApplyRoom._meetingInfo._meetingid);
    mRoomSet.insert(mid);
    printf("RTHttpClient::RoomSet mid:%s\n", (*(mRoomSet.begin())).c_str());
    FILE* fRoomIds = fopen("test_roomids", "a+");
    if (fRoomIds==NULL) {
        return -1;
    }
    if (fRoomIds) {
        fputs(mid, fRoomIds);
        fputs("\n", fRoomIds);
    }
    if (fRoomIds) {
        fclose(fRoomIds);
        fRoomIds = NULL;
    }
    return 0;
}

void RTHttpClient::SyncRoomSet(std::unordered_set<std::string>& roomSet)
{
    printf("RTHttpClient::SyncRoomSet size:%ld\n", mRoomSet.size());
    for (auto lit=mRoomSet.begin() ; lit!=mRoomSet.end(); ++lit) {
        //printf("RTHttpClient::SyncRoomSet mRoomSet roomid:%s\n", (*lit).c_str());
        roomSet.insert(*lit);
    }
}
