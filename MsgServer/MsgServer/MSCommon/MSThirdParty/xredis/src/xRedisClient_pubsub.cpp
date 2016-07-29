/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2013-2014, xSky <guozhw at gmail dot com>
 * All rights reserved.
 * Distributed under GPL license.
 * ----------------------------------------------------------------------------
 */

#include "xRedisClient.h"
#include <sstream>
#include <iostream>


bool xRedisClient::psubscribe(const RedisDBIdx& dbi, const KEYS& vChannels) {
    VDATA vCmdData;
    vCmdData.push_back("PSUBSCRIBE");
    addparam(vCmdData, vChannels);
    SETDEFAULTIOTYPE(MASTER);
    return commandargv_status(dbi, vCmdData);
}

bool xRedisClient::publish(const RedisDBIdx& dbi, const std::string& channel, const std::string& value) {
    if (0==channel.length()) {
        return false;
    }
    int64_t count;
    SETDEFAULTIOTYPE(MASTER);
    return command_integer(dbi, count, "PUBLISH %s %s", channel.c_str(), value.c_str());
}

bool xRedisClient::pubsub(const RedisDBIdx& dbi, const std::string& cmd, const VALUES& vValues, std::string& value) {
    if (0==cmd.length()) {
        return false;
    }
    VDATA vCmdData;
    ArrayReply array;
    vCmdData.push_back("PUBSUB");
    vCmdData.push_back(cmd);
    addparam(vCmdData, vValues);
    SETDEFAULTIOTYPE(MASTER);
    return commandargv_array(dbi, vCmdData, array);
}

bool xRedisClient::punsubscribe(const RedisDBIdx& dbi, const std::string& channel) {
    VDATA vCmdData;
    vCmdData.push_back("PUNSUBSCRIBE");
    vCmdData.push_back(channel);
    SETDEFAULTIOTYPE(MASTER);
    return commandargv_status(dbi, vCmdData);
}

bool xRedisClient::subscribe(const RedisDBIdx& dbi, const RedisConn* conn, const KEY& channel) {
    VDATA vCmdData;
    vCmdData.push_back("SUBSCRIBE");
    vCmdData.push_back(channel);
    SETDEFAULTIOTYPE(MASTER);
    std::cout << "xRedisClient::subscribe was called" << std::endl;
    return commandargv_withconn(conn, vCmdData);
}

bool xRedisClient::unsubscribe(const RedisDBIdx& dbi, const RedisConn* conn, const KEY& channel) {
    VDATA vCmdData;
    vCmdData.push_back("UNSUBSCRIBE");
    vCmdData.push_back(channel);
    SETDEFAULTIOTYPE(MASTER);
    std::cout << "xRedisClient::unsubscribe was called" << std::endl;
    return commandargv_withconn(conn, vCmdData);
}
