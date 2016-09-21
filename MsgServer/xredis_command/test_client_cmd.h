#ifndef _TEST_CLIENT_CMD_
#define _TEST_CLIENT_CMD_

#include "xRedisClient.h"

class TestClientCmd{
    public:
        TestClientCmd();
        ~TestClientCmd();
        void test_zadd();
        void test_set(const char* strkey, const char* strValue);
        void test_append();
        void test_decr();
        void test_decrby();
        void test_incr();
        void test_incrby();
        void test_get();
        void test_type();
        void test_getrange();
        void test_exists();
        void test_del();
        void test_mset();
        void test_mget();
        void test_hset();
        void test_lpush();
        void test_llen();
        void test_lrange();
        void test_lpop();
        void test_rpop();
        void test_publish();
        void test_subscribe();
        void test_sadd();
        void test_smembers();
    private:
        xRedisClient        xClient;
};

#endif // _TEST_CLIENT_CMD_
