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

#include "ProtoCommon.h"

#define PACKED_MSG_NUM_ONCE (10)

class XGrpMsgClientHelper {
public:
    XGrpMsgClientHelper() {}
    ~XGrpMsgClientHelper() {}

    virtual void OnGroupNotify(int code, const std::string& cont) = 0;
    virtual void OnCreateGroupSeqn(int code, const std::string& cont) = 0;
    virtual void OnDeleteGroupSeqn(int code, const std::string& cont) = 0;
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

    int EncodeCreateGroupSeqn(std::string& outstr, const std::string& userid, const std::string& cltUserid, const std::string& groupid, int module);
    int EncodeDeleteGroupSeqn(std::string& outstr, const std::string& userid, const std::string& cltUserid, const std::string& groupid, int module);
    int EncodeGrpSyncSeqnNotify(std::string& outstr, const std::string& userid, const std::string& groupid, const std::string& mtype, const std::string& ispush, const std::string& version, int64 seqn, int module);
    int EncodeGrpSyncSeqnNotifys(std::string& outstr, const std::vector<std::string>& userids, const std::string& groupid, const std::string& mtype, const std::string& ispush, const std::string& version, int64 seqn, int module);

    int EncodeGrpSyncDataNotify(std::string& outstr, const std::string& userid, const std::string& groupid, int64 curseqn, int module);
    int EncodeGrpSyncDataNotifys(std::string& outstr, const std::vector<std::string>& userids, const std::string& groupid, int64 curseqn,  int module);

    int DecodeRecvData(const char* pData, int nLen);

protected:
    int DecodeGroupNotify(int code, const std::string& cont);
    int DecodeCreateGroupSeqn(int code, const std::string& cont);
    int DecodeDeleteGroupSeqn(int code, const std::string& cont);
private:
    XGrpMsgClientHelper    &m_helper;
    pms::PackedStoreMsg  m_packed;

};

#endif /* defined(__dyncRTCMsgClient__XGrpMsgProcesser__) */
