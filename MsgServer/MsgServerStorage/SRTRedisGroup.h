//
//  SRTRedisGroup.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTRedisGroup__
#define __MsgServerStorage__SRTRedisGroup__

#include <stdio.h>
#include <iostream>
#include <utility>
#include <string>
#include <queue>
#include <vector>
#include "OSMutex.h"
#include "RTEventLooper.h"
#include "SRTResponseSender.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class SRTStorageRedis;
class SRTTransferSession;

class SRTRedisGroup : public RTEventLooper, public RTObserverConnection {
    public:
        SRTRedisGroup(SRTTransferSession *sess, const std::string& ip, int port);
        virtual ~SRTRedisGroup();

        void DispatchPushData(const std::string& data);
        void DispatchPostData(const std::string& data);

        // from RTEventLooper
    public:
        virtual void OnPostEvent(const char*pData, int nSize);
        virtual void OnSendEvent(const void*pData, int nSize);
        virtual void OnWakeupEvent(const void*pData, int nSize);
        virtual void OnPushEvent(const char*pData, int nSize);
        virtual void OnTickEvent(const void*pData, int nSize);

// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();
    private:
        int                              m_Port;
        std::string                      m_Ip;

        std::vector<SRTStorageRedis*>    m_WriteRedises;
        std::vector<SRTStorageRedis*>    m_ReadRedises;

        pms::PackedStoreMsg              m_RecvPushMsg;
        pms::PackedStoreMsg              m_RecvPostMsg;
        SRTResponseSender                m_ResponseSender;
};

#endif /* defined(__MsgServerStorage__SRTRedisGroup__) */
