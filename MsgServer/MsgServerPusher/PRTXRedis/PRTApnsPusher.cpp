//
//  PRTApnsPusher.cpp
//  dyncRTPusher
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "PRTApnsPusher.h"
#include "RTUtils.hpp"
#include "IosPusher.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#if 1
#include <sys/syscall.h>
#define gettidv1() syscall(__NR_gettid)
#endif

#define TIMEOUT_TS (60*1000)

static unsigned int APHash(const char *str) {
    unsigned int hash = 0;
    int i;
    for (i=0; *str; i++) {
        if ((i&  1) == 0) {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        } else {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash&  0x7FFFFFFF);
}

enum {
 CACHE_TYPE_1,
 CACHE_TYPE_2,
 CACHE_TYPE_MAX,
};


PRTApnsPusher::PRTApnsPusher(const std::string& ip, int port)
: RCPthread(this)
, m_addr(ip)
, m_port(port)
, m_isRun(false)
{

}

PRTApnsPusher::~PRTApnsPusher()
{

}

bool PRTApnsPusher::Init()
{

    m_xRedis.Init(m_addr, m_port);
    this->_Init();// this will lunch thread
    return true;
}

bool PRTApnsPusher::Unit()
{
    m_xRedis.Unin();
    this->_Unin();
    return true;
}

void PRTApnsPusher::Stop()
{
    m_isRun = false;
}

// from RCPthread

void PRTApnsPusher::Run(void* data)
{
    if (!data) return;
    PRTApnsPusher* pch = (PRTApnsPusher*)data;
    if (!pch) return;
    LI("PRTApnsPusher::Run was called...pid:%d, tid:%d, pthread_self:%d\n", getpid(), gettidv1(), pthread_self());
    m_isRun = true;
    while(m_isRun) {
        // get 10 needed push msg in redis each time
        int64_t start=0, stop=9;
        ArrayReply reply;
        if (!m_xRedis.GetNeedPushMsg("ios", reply, start, stop))
        {
            LE("PRTTransferSession::OnPushEvent GetNeedPushMsg start:%lld stop:%lld failed\n", start, stop);
            return;
        }
        LI("PRTTransferSession::OnPushEvent GetNeedPushMsg reply.size:%d\n", reply.size());

        // parse each msg one by one
        ReplyData::iterator iter = reply.begin();
        for (; iter != reply.end(); iter++) {
            if (((*iter).str.empty()) || ((*iter).str.length()==0)) continue;

            pms::Pushing pushMsg;
            if (!pushMsg.ParseFromString((*iter).str))
            {
                LE("PRTTransferSession::OnPushEvent pushMsg.ParseFromString failed, continue\n");
                continue;
            }
            std::string strJson = pushMsg.content();
            rapidjson::Document jReqDoc;
            if (jReqDoc.ParseInsitu<0>((char*)strJson.c_str()).HasParseError())
            {
                LE("PRTTransferSession::OnPushEvent ParseInsitu failed\n");
                continue;
            }

            if (!(jReqDoc.HasMember("token")&&jReqDoc["token"].IsString()))
            {
                LE("PRTTransferSession::OnPushEvent has no member token\n");
                continue;
            }
            std::string token = jReqDoc["token"].GetString();

            if (!(jReqDoc.HasMember("cont")&&jReqDoc["cont"].IsString()))
            {
                LE("PRTTransferSession::OnPushEvent has no member cont\n");
                continue;
            }
            std::string cont = jReqDoc["cont"].GetString();

            if (!(jReqDoc.HasMember("groupid")&&jReqDoc["groupid"].IsString()))
            {
                LE("PRTTransferSession::OnPushEvent has no member groupid\n");
                continue;
            }
            std::string groupid = jReqDoc["groupid"].GetString();

            if (!(jReqDoc.HasMember("frommsgid")&&jReqDoc["frommsgid"].IsString()))
            {
                LE("PRTTransferSession::OnPushEvent has no member frommsgid\n");
                continue;
            }
            std::string frommsgid = jReqDoc["frommsgid"].GetString();

            if (token.length()==0 || cont.length()==0 || groupid.length()==0 || frommsgid.length()==0)
            {
                LE("PRTTransferSession::OnPushEvent member length is 0\n");
                continue;
            }

            std::vector<const char*> extra;
            extra.push_back("groupid");
            extra.push_back(groupid.c_str());
            extra.push_back("frommsgid");
            extra.push_back(frommsgid.c_str());
            LI("PRTTransferSession::OnPushEvent GetNeedPushMsg call push msg\n");
            if (!IosPusher::Instance().PushMsg(token.c_str(), cont.c_str(), extra.data(), extra.size()))
            {
                // push failed, so store msg to redis
                LI("PRTTransferSession::OnPushEvent SetNeedPushMsg call push msg:%s\n", pushMsg.content().c_str());
                m_xRedis.SetNeedPushMsg("ios", pushMsg.SerializeAsString());
            }
        }

        // if still has need push msg in redis, signal
        int64_t count = 0;
        if (m_xRedis.LenListMsg("ios", count))
        {
            LI("PRTTransferSession::OnPushEvent LenListMsg count:%lld\n", count);
            //if (count > 0)
                // go on loop
        }
    }
    LI("PRTApnsPusher::Run while over...\n");
    LI("PRTApnsPusher::Run finished over...\n");
}


////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
