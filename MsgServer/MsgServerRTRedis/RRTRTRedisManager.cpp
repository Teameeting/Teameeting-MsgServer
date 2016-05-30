//
//  RRTRTRedisManager.cpp
//  dyncRTRTRedis
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RRTRTRedisManager.h"
#include <assert.h>
#include "OSMutex.h"
#include <algorithm>
#include "RTHiredis.h"

static OSMutex          s_mutexList;

static RRTRTRedisManager::TransferSessionList   s_TransferSessions(0);

void RRTRTRedisManager::PushBack(RRTTransferSession* session)
{
    {
        OSMutexLocker locker(&s_mutexList);
        s_TransferSessions.push_back(session);
    }
}

void RRTRTRedisManager::GenerateRTRedis()
{
    {
        OSMutexLocker locker(&s_mutexList);
        //TransferSessionListIt it = s_TransferSessions.front();
        //if (it != s_TransferSessions.end())
        {
            //std::string msg = it->GetProcessingMsg();
            //parse msg
            //get userid, msgid
            //RTHiredisLocal::Instance().Incr();
        }
    }
}

bool RRTRTRedisManager::SignalKill()
{
    return true;
}

bool RRTRTRedisManager::ClearAll()
{
    {
        OSMutexLocker mlocker(&s_mutexList);
        for (auto & x : s_TransferSessions) {
            usleep(100*1000);
        }
        s_TransferSessions.clear();
    }
    return true;
}
