//
//  XPushMsgProcesser.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef __dyncRTCMsgClient__XPushMsgProcesser__
#define __dyncRTCMsgClient__XPushMsgProcesser__

#include <stdio.h>
#include <iostream>
#include <vector>

#include "ProtoCommon.h"

#define PACKED_MSG_NUM_ONCE (10)

class XPushMsgClientHelper {
public:
    XPushMsgClientHelper() {}
    ~XPushMsgClientHelper() {}

    virtual void OnPGetData(int code, const std::string& cont) = 0;
};

class XPushMsgProcesser{
public:
    XPushMsgProcesser(XPushMsgClientHelper& helper):m_helper(helper){}
    ~XPushMsgProcesser(){}

    void Init()
    {
        for (int i=0;i<PACKED_MSG_NUM_ONCE;++i)
            m_packed.add_msgs();
    }

    void Unin()
    {
        for (int i=0;i<PACKED_MSG_NUM_ONCE;++i)
            m_packed.mutable_msgs(i)->Clear();
    }

public:

    int EncodePushGetDataNotify(std::string& outstr, pms::StorageMsg& msg, int module);

    int DecodeRecvData(const char* pData, int nLen);

protected:
    int DecodePGetData(int code, const std::string& cont);
private:
    XPushMsgClientHelper    &m_helper;
    pms::PackedStoreMsg     m_packed;

};

#endif /* defined(__dyncRTCMsgClient__XPushMsgProcesser__) */
