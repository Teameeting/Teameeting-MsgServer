//
//  SRTResponseSender.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTResponseSender__
#define __MsgServerStorage__SRTResponseSender__

#include <stdio.h>
#include <iostream>
#include <utility>
#include <string>
#include <queue>
#include <deque>
#include <vector>
#include <list>
#include "OSMutex.h"
#include "RTEventLooper.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class SRTTransferSession;

class SRTResponseSender : public RTEventLooper {
    public:
        SRTResponseSender();
        virtual ~SRTResponseSender();

        void Init(SRTTransferSession *sess);
        void Unin();

        void PushResponseData(const char*pData, int nSize);
        void PostResponseData(const char*pData, int nSize);

        // from RTEventLooper
    public:
        virtual void OnPostEvent(const char*pData, int nSize){}
        virtual void OnSendEvent(const void*pData, int nSize) {}
        virtual void OnWakeupEvent(const void*pData, int nSize);
        virtual void OnPushEvent(const char*pData, int nSize){}
        virtual void OnTickEvent(const void*pData, int nSize);

    private:
        int                              m_IsRun;
        SRTTransferSession*              m_TransferSession;

        OSMutex                          m_MutexPush;
        pms::PackedStoreMsg              m_SendPushMsg;
        std::queue<std::string>          m_QPushMsg;

        OSMutex                          m_MutexPost;
        pms::PackedStoreMsg              m_SendPostMsg;
        std::queue<std::string>          m_QPostMsg;
};

#endif /* defined(__MsgServerStorage__SRTResponseSender__) */
