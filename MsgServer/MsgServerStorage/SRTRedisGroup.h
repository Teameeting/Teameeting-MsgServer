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
#include "SRTStorageRedis.h"

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

class SRTTransferSession;

class SRTRedisGroup : public RTEventLooper {
    public:
        SRTRedisGroup(SRTTransferSession *sess, const std::string& ip, int port);
        virtual ~SRTRedisGroup();

        void DispatchPushData(const char*pData, int nSize);

        // from RTEventLooper
    public:
        virtual void OnPostEvent(const char*pData, int nSize);
        virtual void OnSendEvent(const void*pData, int nSize);
        virtual void OnWakeupEvent(const void*pData, int nSize);
        virtual void OnPushEvent(const char*pData, int nSize);
        virtual void OnTickEvent(const void*pData, int nSize);

    private:
        int                              m_Port;
        std::string                      m_Ip;

        std::vector<SRTStorageRedis*>    m_Redises;
        pms::PackedStoreMsg              m_RecvPackedMsg;
        SRTResponseSender                m_ResponseSender;
};

#endif /* defined(__MsgServerStorage__SRTRedisGroup__) */
