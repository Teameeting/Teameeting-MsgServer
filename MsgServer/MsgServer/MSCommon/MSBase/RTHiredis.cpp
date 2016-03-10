//
//  RTHiredis.cpp
//  dyncRTConnector
//
//  Created by hp on 11/25/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

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

void RTHiredis::CmdPing()
{
    if (!m_redisContext) {
        return;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "PING");
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            LE("error::%s\n", m_redisReply->str);
        }
        return;
    }
    freeReplyObject((void*)m_redisReply);
}

bool RTHiredis::CmdSet(const std::string key, const std::string value)
{
    if (!m_redisContext || key.empty() || value.empty()) {
        return false;
    }
    redisReply* m_redisReply = (redisReply*)redisCommand(m_redisContext, "SET %s %s", key.c_str(), value.c_str());
    if (!m_redisReply || m_redisReply->type == REDIS_REPLY_ERROR) {
        if (m_redisReply) {
            LE("error::%s\n", m_redisReply->str);
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
            LE("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    if (m_redisReply->str) {
        value = m_redisReply->str;
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
            LE("error::%s\n", m_redisReply->str);
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
            LE("error::%s\n", m_redisReply->str);
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
            LE("error::%s\n", m_redisReply->str);
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
            LE("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, NULL);
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
            LE("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, NULL);
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
            LE("error::%s\n", m_redisReply->str);
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
            LE("error::%s\n", m_redisReply->str);
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
            LE("error::%s\n", m_redisReply->str);
        }
        return false;
    }
    HandleHReply(m_redisReply, ress);
    freeReplyObject((void*)m_redisReply);
    return true;
}


bool RTHiredis::HandleReply(redisReply* reply, std::string* res)
{
    switch (reply->type) {
        case REDIS_REPLY_STRING:
        {
            if (res) {
                res->assign(reply->str);
            } else {
                //LI("reply str:%s\n", reply->str);
            }
        }
            break;
        case REDIS_REPLY_ARRAY:
        {
            //LE("Is this Possible REPLY_ARRAY?\n");
        }
            break;
        case REDIS_REPLY_INTEGER:
        {
            if (res) {
                char c[12] = {0};
                sprintf(c, "%lld", reply->integer);
                res->assign(c);
            } else {
                //LI("reply int:%lld\n", reply->integer);
            }
        }
            break;
        case REDIS_REPLY_NIL:
        {
            LI("reply nil\n");
        }
            break;
        case REDIS_REPLY_STATUS:
        {
            LI("reply status\n");
        }
            break;
        case REDIS_REPLY_ERROR:
        {
            if (res) {
                res->assign(reply->str);
            } else {
                LI("reply err:%s\n", reply->str);
            }
        }
            break;
        default:
        {
            LE("reply default handle\n");
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
                LI("hreply str:%s\n", reply->str);
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
                    //LI("RTHiredis::HandleReply ARRAY was called\n");
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
                LI("hreply int:%lld\n", reply->integer);
            }
        }
            break;
        case REDIS_REPLY_NIL:
        {
            LI("hreply nil\n");
        }
            break;
        case REDIS_REPLY_STATUS:
        {
            LI("hreply status\n");
        }
            break;
        case REDIS_REPLY_ERROR:
        {
            if (ress) {
                ress->push_front(reply->str);
            } else {
                LI("hreply err:%s\n", reply->str);
            }
        }
            break;
        default:
        {
            LE("hreply default handle\n");
        }
            break;
    }
    return true;
}
