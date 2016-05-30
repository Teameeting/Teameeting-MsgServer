//
//  RTHiredis.cpp
//  dyncRTConnector
//
//  Created by hp on 11/25/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "RTHiredis.h"

void RTHiredis::Connect(int timeout)
{
    if (timeout > 0) {
        int t = timeout / 1000;
        if (t == 0) {
            m_sTimeout = 0;
            m_msTimeout = timeout % 1000;
        } else {
            m_sTimeout = t;
            m_msTimeout = timeout % 1000;

        }
    }
    struct timeval st_timeout = { m_sTimeout, m_msTimeout * 1000 }; // 1.5 seconds
    m_redisContext = redisConnectWithTimeout(m_host.c_str(), m_port, st_timeout);
    if (m_redisContext == NULL || m_redisContext->err) {
        if (m_redisContext) {
            printf("Connection error: %s\n", m_redisContext->errstr);
            redisFree(m_redisContext);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }
}

void RTHiredis::DisConn()
{
    if (m_redisContext) {
        redisFree(m_redisContext);
        m_redisContext = NULL;
    }
}

bool RTHiredis::CmdPing()
{
    if (!m_redisContext) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "PING");
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    bool ping = false;
    if (m_redisReply->str) {
        printf("ping response:%s\n", m_redisReply->str);
        ping = true;
    }
    freeReplyObject((void*)m_redisReply);
    return ping;
}

bool RTHiredis::CmdSet(const std::string key, const std::string value)
{
    if (!m_redisContext || key.empty() || value.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "SET %s %s", key.c_str(), value.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdGet(const std::string key, std::string& value)
{
    if (!m_redisContext || key.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "GET %s", key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    if (m_redisReply->str) {
        value = m_redisReply->str;
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdSet(const std::string key, int value)
{
    if (!m_redisContext || key.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "SET %s %d", key.c_str(), value);
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdGet(const std::string key, int* value)
{
    if (!m_redisContext || key.empty() || value == nullptr) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "GET %s", key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    if (m_redisReply->str) {
        //printf("CmdGet str:%s\n", m_redisReply->str);
        char* endptr;
        *value = (int)strtol(m_redisReply->str, &endptr, 10);
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdDel(const std::string key)
{
    if (!m_redisContext || key.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "DEL %s", key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdIncr(const std::string key)
{
    if (!m_redisContext || key.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "INCR %s", key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}



bool RTHiredis::CmdHSet(const std::string hid, const std::string key, const std::string value)
{
    if (!m_redisContext || hid.empty() || key.empty() || value.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HSET %s %s %s", hid.c_str(), key.c_str(), value.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, NULL);
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHGet(const std::string hid, const std::string key, std::string& value)
{

    if (!m_redisContext || hid.empty() || key.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HGET %s %s", hid.c_str(), key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    std::list<std::string> ulist;
    HandleHReply(m_redisReply, &ulist);
    if (ulist.size()>0) {
        value = ulist.front();
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHDel(const std::string hid, const std::string key, std::string& res)
{

    if (!m_redisContext || hid.empty() || key.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HDEL %s %s", hid.c_str(), key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, NULL);
    res = "";
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHSet(const std::string hid, int key, const std::string value)
{
    if (!m_redisContext || hid.empty() || value.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HSET %s %d %s", hid.c_str(), key, value.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, NULL);
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHGet(const std::string hid, int key, std::string& value)
{

    if (!m_redisContext || hid.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HGET %d %s", hid.c_str(), key);
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    std::list<std::string> ulist;
    HandleHReply(m_redisReply, &ulist);
    if (ulist.size()>0) {
        value = ulist.front();
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHExists(const std::string hid, const std::string key, std::string& res)
{
    if (!m_redisContext || hid.empty() || key.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HEXISTS %s %s", hid.c_str(), key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, NULL);
    res = "";
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHLen(const std::string hid, int* len)
{
    if (!m_redisContext || hid.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HLEN %s", hid.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, NULL);
    if (len) {
        *len = (int)m_redisReply->integer;
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHKeys(const std::string hid, std::list<std::string>* ress)
{
    if (!m_redisContext || hid.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HKEYS %s", hid.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, ress);
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::CmdHVals(const std::string hid, std::list<std::string>* ress)
{
    if (!m_redisContext || hid.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "HVALS %s", hid.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, ress);
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::GenericIncrId(const std::string& key, long long* value)
{

    if (!m_redisContext || key.empty()) {
        if (!m_redisContext) printf("GenericIncrId m_redisContext is null, this:%p, getpid:%d\n", this, getpid());
        if (key.empty()) printf("GenericIncrId key is empty, this:%p, getpid:%d\n", this, getpid());
        printf("GenericIncrId params error\n");
        return false;
    }

    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "INCR %s", key.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            printf("INCR key:%s error::%s\n", key.c_str(), m_redisReply->str);
        }
        return false;
    }

    if (m_redisReply->type==REDIS_REPLY_INTEGER)
    {
        //printf("GenericIncrId integer int:%lld\n", m_redisReply->integer);
        *value = m_redisReply->integer;
    } else if (m_redisReply->type==REDIS_REPLY_STRING)
    {
        //printf("CmdGet key:%s, str:%s\n", key.c_str(), m_redisReply->str);
        char* endptr;
        *value = strtoll(m_redisReply->str, &endptr, 10);
        printf("GenericIncrId string str:%s\n", m_redisReply->str);
    } else
    {
        printf("GenericIncrId m_redisReply->str is null, key is:%s, reply->type:%d\n\n", key.c_str(), m_redisReply->type);
    }
    freeReplyObject((void*)m_redisReply);
    return true;
}

bool RTHiredis::HandleReply(redisReply* reply, std::string* res)
{
    switch (reply->type) {
        case REDIS_REPLY_STRING:
        {
            printf("reply string type:%d\n", reply->type);
            if (res) {
                res->assign(reply->str);
            } else {
                //printf("reply str:%s\n", reply->str);
            }
        }
            break;
        case REDIS_REPLY_ARRAY:
        {
            printf("reply array type:%d\n", reply->type);
            //printf("Is this Possible REPLY_ARRAY?\n");
        }
            break;
        case REDIS_REPLY_INTEGER:
        {
            printf("reply integer type:%d\n", reply->type);
            if (res) {
                char c[12] = {0};
                sprintf(c, "%lld", reply->integer);
                res->assign(c);
                printf("res is reply int:%lld\n", reply->integer);
            } else {
                printf("no res reply int:%lld\n", reply->integer);
            }
        }
            break;
        case REDIS_REPLY_NIL:
        {
            printf("reply nil\n");
        }
            break;
        case REDIS_REPLY_STATUS:
        {
            printf("reply status\n");
        }
            break;
        case REDIS_REPLY_ERROR:
        {
            printf("reply error type:%d\n", reply->type);
            if (res) {
                res->assign(reply->str);
            } else {
                printf("reply err:%s\n", reply->str);
            }
        }
            break;
        default:
        {
            printf("reply default handle\n");
        }
            break;
    }
    return true;
}

bool RTHiredis::HandleHReply(redisReply* reply, std::list<std::string>* ress)
{
    switch (reply->type) {
        case REDIS_REPLY_STRING:
        {
            if (ress) {
                ress->push_front(reply->str);
            } else {
                printf("hreply str:%s\n", reply->str);
            }
        }
            break;
        case REDIS_REPLY_ARRAY:
        {
            int n = (int)reply->elements;
            int i=0;
            for (;i<n;i++) {
                redisReply* rp = (redisReply*)*(reply->element);
                if (rp) {
                    //printf("RTHiredis::HandleReply ARRAY was called\n");
                    HandleHReply(rp, ress);
                }
            }
        }
            break;
        case REDIS_REPLY_INTEGER:
        {
            if (ress) {
                char c[12] = {0};
                sprintf(c, "%lld", reply->integer);
                ress->push_front(c);
            } else {
                printf("hreply int:%lld\n", reply->integer);
            }
        }
            break;
        case REDIS_REPLY_NIL:
        {
            printf("hreply nil\n");
        }
            break;
        case REDIS_REPLY_STATUS:
        {
            printf("hreply status\n");
        }
            break;
        case REDIS_REPLY_ERROR:
        {
            if (ress) {
                ress->push_front(reply->str);
            } else {
                printf("hreply err:%s\n", reply->str);
            }
        }
            break;
        default:
        {
            printf("hreply default handle\n");
        }
            break;
    }
    return true;
}
