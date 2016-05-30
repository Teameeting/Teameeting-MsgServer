//
//  ClientManager.cpp
//  dyncRTRTRedis
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "ClientManager.h"
#include <assert.h>
#include "OSMutex.h"
#include <algorithm>
#include <sys/time.h>
#include "OS.h"

#define KEEP_ALIVE_TIME_MSEC (45*1000)
#define _MAX_SECOND_NOTIFY 5
#define _MAX_MSG_SEND_NUM  10

static int s_msg_id = 29;
static uint32_t g_last_update_time = 0;

static uint32_t XGetTimestamp(void)
{
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000;
}

void ClientManager::InitClient(const char* pUserid, const char* pIp, unsigned int port)
{
    mUserId = pUserid;
    mRecvCounter = 0;
    mSendCounter = 0;
    std::string recvfile(mUserId);
    recvfile.append("-recv");
    {
        pRecvFile = fopen(recvfile.c_str(), "w");
        if (!pRecvFile)
        {
            std::cout << "fopen file for user " << recvfile << " failed" << std::endl;
            Assert(false);
        }
    }
    std::string sendfile(mUserId);
    sendfile.append("-send");
    {
        pSendFile = fopen(sendfile.c_str(), "w");
        if (!pSendFile)
        {
            std::cout << "fopen file for user " << sendfile << " failed" << std::endl;
            Assert(false);
        }
    }
    mClientSession = new ClientSession();
    if (!mClientSession) Assert(false);
    mClientSession->Init();
    mClientSession->Connect(pIp, port);
}

void ClientManager::UninClient()
{
    if (!mClientSession) Assert(false);
    delete mClientSession;
    mClientSession = nullptr;
    if (pSendFile)
    {
         fclose(pSendFile);
         pSendFile = nullptr;
    }
    if (pRecvFile)
    {
         fclose(pRecvFile);
         pRecvFile = nullptr;
    }
}

void ClientManager::RequestLoop()
{
    mIsRun = true;
    int test_times = 0;
    long long last_seq = 0;
    //long long count = 1000*10;
    //long long count = 1000;
    //long long expire_count = count*80;
    while(mIsRun)
    {
        usleep(1500);
        //sleep(2);
        //if (test_times++ < count)
        {
            GenericMsg(mUserId);
        }
	//if (test_times > expire_count)
	{
       	//	break;
	}
        uint32_t now = XGetTimestamp();
        if (g_last_update_time + KEEP_ALIVE_TIME_MSEC < now)
        {
            g_last_update_time = now;
            Keepalive();
        }
        DoPackage();
    }
}


void ClientManager::GenericMsg(const std::string& userid)
{
	for(int i=0;i<10;++i) {
		pms::SequenceMsg* p_sequence = new pms::SequenceMsg;
		std::string msgId;
		GenericMsgId(msgId);
		p_sequence->set_msgid(msgId);
		p_sequence->set_userid(userid);

		//std::cout << "GenericMsg each size:" << p_sequence->ByteSize() << \
		//                ", msgid:" << p_sequence->msgid()  << \
		//                ", userid:" << p_sequence->userid() << std::endl;
		m_wait2SndList.push_back(p_sequence);
	}
	SInt64 curTime = OS::Milliseconds();
	char buf[128] = {0};
	sprintf(buf, "send_time:%lld:mSendCounter:%lld\n", curTime, ++mSendCounter);
	fwrite(buf, 1, 128, pSendFile);
	LI("%s\n", buf);
}

int ClientManager::GenericMsgId(std::string& strMsgId)
{
    char msgid[32] = {0};
    sprintf(msgid, "%s_%d", mUserId.c_str(), ++s_msg_id);
    strMsgId = msgid;
    return 0;
}


void ClientManager::DoPackage()
{
    if (m_wait2SndList.size()==0) return;
    pms::PackedSeqnMsg * pit = new pms::PackedSeqnMsg;
    for (int i=0;i<_MAX_MSG_SEND_NUM;++i) {
        if (m_wait2SndList.size()>0) {
            pms::SequenceMsg* tit = pit->add_msgs();
	    pms::SequenceMsg* ttit = m_wait2SndList.front();
            tit->CopyFrom(*(ttit));
	    delete ttit;
	    ttit = nullptr;
            m_wait2SndList.pop_front();
        }
    }
    mClientSession->SendTransferData(pit->SerializeAsString());
	delete pit;
	pit = nullptr;
}

void ClientManager::ProcessRecvMessage(const char*data, int nLen)
{
    std::string msg(data, nLen);
    pms::PackedSeqnMsg packMsg;
    packMsg.ParseFromString(msg);
    for (int i=0;i<packMsg.msgs_size();++i)
    {
        if (packMsg.msgs(i).userid().compare("keepalive")==0)continue;
    }
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:mRecvCounter:%lld\n", curTime, ++mRecvCounter);
    fwrite(buf, 1, 128, pRecvFile);
    LI("%s\n",buf);
}

void ClientManager::Keepalive()
{
	LI("ClientManager::Keepalive...\n");
    mClientSession->UpdateTime();
    pms::SequenceMsg* p_sequence = new pms::SequenceMsg;
    p_sequence->set_userid("keepalive");
    m_wait2SndList.push_back(p_sequence);
}

bool ClientManager::SignalKill()
{
    return true;
}

bool ClientManager::ClearAll()
{
    return true;
}
