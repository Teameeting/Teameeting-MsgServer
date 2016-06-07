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

#include "MsgServer/proto/storage_msg.pb.h"

#define KEEP_ALIVE_TIME_MSEC (45*1000)
#define _MAX_MSG_SEND_NUM  10
#define PACKED_MSG_NUM  10

#define MSG_TYPE_SQUNE (1)
#define MSG_TYPE_STORE (2)

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

    std::string logfile(mUserId);
    logfile.append("-log");
    mIfs.open(logfile.c_str(), std::ifstream::in);
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
    mIfs.close(); 
}

void ClientManager::RequestLoop()
{
    mIsRun = true;
    int test_times = 0;
    long long last_seq = 0;
#if 0 
    //long long count = 500;
    //long long expire_count = count*4;
    long long count = 5000;
    long long expire_count = count*8;
#else

#endif
	int type = MSG_TYPE_STORE;
	while(mIsRun)
	{
#if 0 
		usleep(100);
		if (test_times++ < count)
		{
			GenericMsg(type, mUserId);
		}
		if (test_times > expire_count)
		{
				break;
		}
#else
		usleep(100);
		GenericMsg(type, mUserId);
#endif
		uint32_t now = XGetTimestamp();
		if (g_last_update_time + KEEP_ALIVE_TIME_MSEC < now)
		{
			g_last_update_time = now;
			Keepalive(type);
		}
		DoPackage(type);
	}
}


void ClientManager::GenericMsg(int type, const std::string& userid)
{
	if (type == MSG_TYPE_SQUNE)
	{
		for(int i=0;i<PACKED_MSG_NUM;++i) {
			pms::SequenceMsg* p_sequence = new pms::SequenceMsg;
			std::string msgId;
			GenericMsgId(msgId);
			p_sequence->set_msgid(msgId);
			p_sequence->set_userid(userid);

			//std::cout << "GenericMsg each size:" << p_sequence->ByteSize() << \
			//                ", msgid:" << p_sequence->msgid()  << \
			//                ", userid:" << p_sequence->userid() << std::endl;
			m_sequeWait2SndList.push_back(p_sequence);
		}	
	} else if (type == MSG_TYPE_STORE) {
		for(int i=0;i<PACKED_MSG_NUM;++i) {
			pms::StorageMsg* p_storage = new pms::StorageMsg;
			std::string msgId;
			GenericMsgId(msgId);
			p_storage->set_mflag(1);
			p_storage->set_msgid(msgId);
			p_storage->set_userid(userid);
			p_storage->set_sequence(100);
			p_storage->set_content("hello redis");

			//std::cout << "GenericMsg each size:" << p_sequence->ByteSize() << \
			//                ", msgid:" << p_sequence->msgid()  << \
			//                ", userid:" << p_sequence->userid() << std::endl;
			m_storeWait2SndList.push_back(p_storage);
		}
	}
	
	SInt64 curTime = OS::Milliseconds();
	char buf[128] = {0};
	sprintf(buf, "send_time:%lld:mSendCounter:%lld\n", curTime, ++mSendCounter);
	LI("%s", buf);
}

int ClientManager::GenericMsgId(std::string& strMsgId)
{
    char msgid[32] = {0};
    sprintf(msgid, "%s_%d", mUserId.c_str(), ++s_msg_id);
    strMsgId = msgid;
    return 0;
}


void ClientManager::DoPackage(int type)
{
	if (type == MSG_TYPE_SQUNE)
	{
		if (m_sequeWait2SndList.size()==0) return;
		pms::PackedSeqnMsg * pit = new pms::PackedSeqnMsg;
		for (int i=0;i<_MAX_MSG_SEND_NUM;++i) {
			if (m_sequeWait2SndList.size()>0) {
				pms::SequenceMsg* tit = pit->add_msgs();
				pms::SequenceMsg* ttit = m_sequeWait2SndList.front();
				tit->CopyFrom(*(ttit));
				delete ttit;
				ttit = nullptr;
				m_sequeWait2SndList.pop_front();
			}
		}
		mClientSession->SendTransferData(pit->SerializeAsString());
		delete pit;
		pit = nullptr;
	} else if (type == MSG_TYPE_STORE)
	{
		if (m_storeWait2SndList.size()==0) return;
		pms::PackedStoreMsg * pit = new pms::PackedStoreMsg;
		for (int i=0;i<_MAX_MSG_SEND_NUM;++i) {
			if (m_storeWait2SndList.size()>0) {
				pms::StorageMsg* tit = pit->add_msgs();
				pms::StorageMsg* ttit = m_storeWait2SndList.front();
				tit->CopyFrom(*(ttit));
				delete ttit;
				ttit = nullptr;
				m_storeWait2SndList.pop_front();
			}
		}
		mClientSession->SendTransferData(pit->SerializeAsString());
		delete pit;
		pit = nullptr;
	}
}

void ClientManager::ProcessRecvMessage(const char*data, int nLen)
{
#if 0
    std::string msg(data, nLen);
    pms::PackedSeqnMsg packMsg;
    packMsg.ParseFromString(msg);
    for (int i=0;i<packMsg.msgs_size();++i)
    {
        if (packMsg.msgs(i).userid().compare("keepalive")==0)continue;
    }
#endif
	std::string msg(data, nLen);
    pms::PackedStoreMsg packMsg;
    packMsg.ParseFromString(msg);
    for (int i=0;i<packMsg.msgs_size();++i)
    {
		if (packMsg.msgs(i).content().length()>0)
		printf("recv storeMsg content:%s\n", packMsg.msgs(i).content().c_str());
    }
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:mRecvCounter:%lld\n", curTime, ++mRecvCounter);
    LI("%s",buf);
}

void ClientManager::Keepalive(int type)
{
	if (type == MSG_TYPE_SQUNE)
	{
		LI("ClientManager::Keepalive...\n");
		mClientSession->UpdateTime();
		pms::SequenceMsg* p_sequence = new pms::SequenceMsg;
		p_sequence->set_userid("keepalive");
		m_sequeWait2SndList.push_back(p_sequence);
	} else if (type == MSG_TYPE_STORE)
	{
		LI("ClientManager::Keepalive self...not sending\n");
		mClientSession->UpdateTime();
		//pms::StorageMsg* p_storage = new pms::StorageMsg;
		//p_storage->set_userid("keepalive");
		//m_storeWait2SndList.push_back(p_storage);
	}
}

bool ClientManager::SignalKill()
{
    return true;
}

bool ClientManager::ClearAll()
{
    return true;
}
