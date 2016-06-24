//
//  XGrpMsgProcesser.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef __dyncRTCMsgClient__XGrpMsgProcesser__
#define __dyncRTCMsgClient__XGrpMsgProcesser__

#include <stdio.h>
#include <iostream>
#include <vector>

#include "RTMsgCommon.h"

#define PACKED_MSG_NUM_ONCE (10)

class XGrpMsgClientHelper {
public:
    XGrpMsgClientHelper() {}
    ~XGrpMsgClientHelper() {}

    virtual void OnLogin(int code, const std::string& userid) = 0;
    virtual void OnLogout(int code, const std::string& userid) = 0;
    virtual void OnKeepLive(int code, const std::string& cont) = 0;
    virtual void OnGroupNotify(int code, const std::string& cont) = 0;
};

class XGrpMsgProcesser{
public:
    XGrpMsgProcesser(XGrpMsgClientHelper& helper):m_helper(helper){}
    ~XGrpMsgProcesser(){}

    void Init()
    {
        for (int i=0;i<PACKED_MSG_NUM_ONCE;++i)
            m_packed.add_msgs();
    }
public:

    int EncodeLogin(std::string& outstr, const std::string& userid, const std::string& token, int module);
    int EncodeLogout(std::string& outstr, const std::string& userid, const std::string& token, int module);
    int EncodeKeepAlive(std::string& outstr, const std::string& userid, int module);
    int EncodeGroupNotify(std::string& outstr, const std::string& userid, const std::string& groupid, int64 curseqn, int module);
    int EncodeGroupNotifys(std::string& outstr, const std::vector<std::string>& userids, const std::string& groupid, int64 curseqn,  int module);

    int EncodeSyncDataRequest(std::string& outstr, const std::string& userid, const std::string& groupid, int64 curseqn, int module);
    int EncodeSyncDataRequests(std::string& outstr, const std::vector<std::string>& userids, const std::string& groupid, int64 curseqn,  int module);

    int DecodeRecvData(const char* pData, int nLen);

protected:
    int DecodeLogin(int code, const std::string& cont);
    int DecodeLogout(int code, const std::string& cont);
    int DecodeKeepAlive(int code, const std::string& cont);
    int DecodeGroupNotify(int code, const std::string& cont);
private:
    XGrpMsgClientHelper    &m_helper;
    pms::PackedStoreMsg  m_packed;

};

#endif /* defined(__dyncRTCMsgClient__XGrpMsgProcesser__) */
