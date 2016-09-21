#ifndef _TEST_CLIENT_PUBLISH_
#define _TEST_CLIENT_PUBLISH_

#include "xRedisClient.h"
#include <string>
#include <stdio.h>

class TestClientPublish{
    public:
        TestClientPublish();
        ~TestClientPublish();

        void AddUserAndPub(const std::string& channel, const std::string& groupid, const std::string& userid);

        void publish(const char* channel, const char* value);
        void sadd(const char* groupid, const char* userid);
        void smembers(const char* groupid);
        void encode_add_group(const std::string& groupId, const std::string& userId, std::string& outJson);
    private:
        xRedisClient        xClient;
};

#endif // _TEST_CLIENT_PUBLISH_
