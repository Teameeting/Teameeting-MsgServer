#include "test_client_publish.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <json/json.h>

#include "test_client_common.h"


TestClientPublish::TestClientPublish()
{
    xClient.Init(3);
    RedisNode RedisList1[3] = {
        {0, "192.168.7.218", 6379, "", 2, 5, 0},
        {1, "192.168.7.218", 6379, "", 2, 5, 0},
        {2, "192.168.7.218", 6379, "", 2, 5, 0}
    };

    xClient.ConnectRedisCache(RedisList1, 3, CACHE_TYPE_1);
}

TestClientPublish::~TestClientPublish()
{
    xClient.Release();
}

void TestClientPublish::AddUserAndPub(const std::string& channel, const std::string& groupid, const std::string& userid)
{
    // channel "follow_group" "unfollow_group"
    // redis key anyrtc_live_%s groupid
    std::string out;
    std::string set_group_key = "anyrtc_live_";
    set_group_key.append(groupid);
    encode_add_group(groupid, userid, out);
    sadd(set_group_key.c_str(), userid.c_str());
    publish(channel.c_str(), out.c_str());
    std::cout << "publish channel:" << channel << ", msg:" << out << std::endl;
}

void TestClientPublish::publish(const char* channel, const char* value)
{
    char szHKey[256] = { 0 };
    strcpy(szHKey, channel);
    RedisDBIdx dbi(&xClient);

    bool bRet = dbi.CreateDBIndex(szHKey, APHash, CACHE_TYPE_1);
    if (bRet) {
        int64_t count;
        if (xClient.publish(dbi, szHKey, value, count)) {
            printf("%s channel:%s success \r\n", __PRETTY_FUNCTION__, channel);
        } else {
            printf("%s channel:%s error [%s] \r\n", __PRETTY_FUNCTION__, channel, dbi.GetErrInfo());
        }
    }
}

void TestClientPublish::sadd(const char* groupid, const char* userid)
{
    char szHKey[256] = {0};
    strcpy(szHKey, groupid);
    VALUES vValue;
    vValue.push_back(userid);
    RedisDBIdx dbi(&xClient);
    bool bRet = dbi.CreateDBIndex(szHKey, APHash, CACHE_TYPE_1);
    if (bRet) {
        int64_t count = 0;
        if (xClient.sadd(dbi, szHKey, vValue, count)) {
            printf("%s success \r\n", __PRETTY_FUNCTION__);
        } else {
            printf("%s error [%s] \r\n", __PRETTY_FUNCTION__, dbi.GetErrInfo());
        }
    }
}

void TestClientPublish::smembers(const char* groupid)
{
    char szHKey[256] = {0};
    strcpy(szHKey, groupid);
    VALUES vValue;
    RedisDBIdx dbi(&xClient);
    bool bRet = dbi.CreateDBIndex(szHKey, APHash, CACHE_TYPE_1);
    if (bRet) {
        if (xClient.smembers(dbi, szHKey, vValue)) {
            printf("%s success, vValue.size:%lu \r\n", __PRETTY_FUNCTION__, vValue.size());
            for (int i=0;i<(int)vValue.size();i++)
            {
                 printf("smembers value i: %d value is:%s\n", i, vValue[i].c_str());
            }
        } else {
            printf("%s error [%s] \r\n", __PRETTY_FUNCTION__, dbi.GetErrInfo());
        }
    }
}


void TestClientPublish::encode_add_group(const std::string& groupId, const std::string& userId, std::string& outJson)
{
    Json::Value value;
    value["groupid"] = groupId;
    value["userid"]  = userId;
    std::string s = Json::FastWriter().write(value);
    outJson = s;
    printf("encode_add_group s is:%s\n", s.c_str());
}
