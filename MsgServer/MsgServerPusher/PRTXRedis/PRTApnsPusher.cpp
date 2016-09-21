//
//  PRTApnsPusher.cpp
//  dyncRTPusher
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "OSThread.h"
#include "PRTApnsPusher.h"
#include "RTUtils.hpp"
#include "IosPusher.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "MsgServer/proto/entity_msg.pb.h"
#include "MsgServer/proto/entity_msg_type.pb.h"

#if 1
#include <sys/syscall.h>
#define gettidv1() syscall(__NR_gettid)
#endif

#define TIMEOUT_TS (60*1000)

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
        // if still has need push msg in redis, signal
        int64_t count = 0;
        if (m_xRedis.LenListMsg("ios", count))
        {
            LI("PRTApnsPusher::Run LenListMsg count:%lld\n", count);
            if (count == 0)
            {
                LI("PRTApnsPusher::Run LenListMsg no msg now, sleep 1s waiting...\n");
                OSThread::Sleep(1000); // sleep 1s
                continue;
            }
        }

        // get 10 needed push msg in redis each time
        int64_t start=0, stop=9;
        ArrayReply reply;
        if (!m_xRedis.GetNeedPushMsg("ios", reply, start, stop))
        {
            LE("PRTApnsPusher::Run GetNeedPushMsg start:%lld stop:%lld failed\n", start, stop);
            continue;
        }
        LI("PRTApnsPusher::Run GetNeedPushMsg reply.size:%d\n", reply.size());

        // parse each msg one by one
        ReplyData::iterator iter = reply.begin();
        for (; iter != reply.end(); iter++) {
            if (((*iter).str.empty()) || ((*iter).str.length()==0)) continue;

            pms::Pushing pushMsg;
            if (!pushMsg.ParseFromString((*iter).str))
            {
                LE("PRTApnsPusher::Run pushMsg.ParseFromString failed, continue\n");
                continue;
            }
            std::string strJson = pushMsg.content();
            rapidjson::Document jReqDoc;
            if (jReqDoc.ParseInsitu<0>((char*)strJson.c_str()).HasParseError())
            {
                LE("PRTApnsPusher::Run ParseInsitu failed\n");
                continue;
            }

            if (!(jReqDoc.HasMember("token")&&jReqDoc["token"].IsString()))
            {
                LE("PRTApnsPusher::Run has no member token\n");
                continue;
            }
            std::string token = jReqDoc["token"].GetString();

            if (!(jReqDoc.HasMember("cont")&&jReqDoc["cont"].IsString()))
            {
                LE("PRTApnsPusher::Run has no member cont\n");
                continue;
            }
            std::string cont = jReqDoc["cont"].GetString();

            if (!(jReqDoc.HasMember("groupid")&&jReqDoc["groupid"].IsString()))
            {
                LE("PRTApnsPusher::Run has no member groupid\n");
                continue;
            }
            std::string groupid = jReqDoc["groupid"].GetString();

            if (!(jReqDoc.HasMember("frommsgid")&&jReqDoc["frommsgid"].IsString()))
            {
                LE("PRTApnsPusher::Run has no member frommsgid\n");
                continue;
            }
            std::string frommsgid = jReqDoc["frommsgid"].GetString();

            if (token.length()==0 || cont.length()==0 || groupid.length()==0 || frommsgid.length()==0)
            {
                LE("PRTApnsPusher::Run member length is 0\n");
                continue;
            }

            std::vector<const char*> extra;
            extra.push_back("groupid");
            extra.push_back(groupid.c_str());
            extra.push_back("frommsgid");
            extra.push_back(frommsgid.c_str());
            LI("PRTApnsPusher::Run GetNeedPushMsg call push msg\n");
            if (!IosPusher::Instance().PushMsg(token.c_str(), cont.c_str(), extra.data(), extra.size()))
            {
                // push failed, so store msg to redis
                int pushtimes = pushMsg.ptimes();
                LI("PRTApnsPusher::Run SetNeedPushMsg ====>>>>>pushtimes is:%d\n", pushtimes);
                if (pushtimes <5) {
                    pushMsg.set_ptimes(++pushtimes);
                    LI("PRTApnsPusher::Run SetNeedPushMsg call pushmsg push times:%d\n", pushMsg.ptimes());
                    m_xRedis.SetNeedPushMsg("ios", pushMsg.SerializeAsString());
                }
            }
        }
    }
    LI("PRTApnsPusher::Run while over...\n");
    LI("PRTApnsPusher::Run finished over...\n");
}


////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
