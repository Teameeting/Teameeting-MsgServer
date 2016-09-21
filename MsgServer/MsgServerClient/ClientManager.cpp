//
//  ClientManager.cpp
//  dyncRTRTRedis
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "ClientManager.h"
#include <assert.h>
#include <algorithm>
#include <sys/time.h>
#include "OS.h"

#include "client_common/proto/storage_msg.pb.h"
#include "client_common/proto/storage_msg_type.pb.h"

static int s_msg_id = 0;
static uint32_t g_last_update_time = 0;

static uint32_t XGetTimestamp(void)
{
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000;
}

void ClientManager::InitClient(const char* pGroupid, const char* pUserid, const char* pIp, unsigned int port)
{
    std::string logfile(pUserid);
    logfile.append("-log");
    mIfs.open(logfile.c_str(), std::ifstream::in);
    mClientSession = new ClientSession();
    if (!mClientSession)
    {
         printf("ClientManager::InitClient new ClientSession failed\n");
         exit(0);
    }
    mClientSession->Init(pGroupid, pUserid, pIp, port);
}

void ClientManager::UninClient()
{
    if (!mClientSession) Assert(false);
    mClientSession->Unin();
    delete mClientSession;
    mClientSession = nullptr;
    mIfs.close();
}

void ClientManager::AddGroup(const std::string& groupid)
{
     if (mClientSession)
     {
         mClientSession->AddGroup(groupid);
     }
}

void ClientManager::SendGroupMsg()
{
    std::string groupid = "TestGroupId01";
    std::string times = std::to_string(++s_msg_id);
    std::string msg = "Hello Msg ";
    msg.append(times);
     if (mClientSession)
     {
         mClientSession->SendGroupMsg(groupid, msg);
     }
}
