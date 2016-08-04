#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>

#include "xRedisClient.h"

#define TEST_SET_GET (1)

// AP Hash Function
unsigned int APHash(const char *str) {
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


RedisNode RedisList1[3]=
{
    {0,"127.0.0.1", 6379, "", 2, 5, 0},
    //{1,"127.0.0.1", 6379, "", 2, 5, 0},
    //{2,"127.0.0.1", 6379, "", 2, 5, 0}
};

RedisNode RedisList2[5]=
{
    {0,"127.0.0.1", 6379, "", 2, 5, 0},
    {1,"127.0.0.1", 6379, "", 2, 5, 0},
    {2,"127.0.0.1", 6379, "", 2, 5, 0},
    {3,"127.0.0.1", 6379, "", 2, 5, 0},
    {4,"127.0.0.1", 6379, "", 2, 5, 0},
};

int main(int argc, char **argv) {

    (void)argc;(void)argv;

    xRedisClient xRedis;
    xRedis.Init(CACHE_TYPE_MAX);
    xRedis.ConnectRedisCache(RedisList1, 1, CACHE_TYPE_1);
    //xRedis.ConnectRedisCache(RedisList2, 5, CACHE_TYPE_2);

#if TEST_SET_GET
    //for (int n = 0; ; n++) {
        char szKey[256] = {0};
        sprintf(szKey, "test_%d", 0);
        RedisDBIdx dbi(&xRedis);
        dbi.CreateDBIndex(szKey, APHash, CACHE_TYPE_1);
        bool bRet = xRedis.setex(dbi, szKey, 10, "hello redis!");
        if (!bRet){
            printf("error:key: %s, err: %s \n", szKey, dbi.GetErrInfo());
        }
    //}

    for (int n = 0; ; n++) {
        char szKey[256] = {0};
        sprintf(szKey, "test_%d", 0);
        RedisDBIdx dbi(&xRedis);
        dbi.CreateDBIndex(szKey, APHash, CACHE_TYPE_1);
        string strValue;
        xRedis.get(dbi, szKey, strValue);
        int64_t t = 0;
        bool ttl = xRedis.ttl(dbi, szKey, t);
        printf("%s , ttl:%d, left time:%ld\r\n", strValue.c_str(), ttl, t);
        sleep(1);
    }
#else
    const char* szKey = "test_subscribe";
    printf("xredis-example was called...\n");
    RedisDBIdx dbi(&xRedis);
    dbi.CreateDBIndex(szKey, APHash, CACHE_TYPE_1);
    RedisConn* conn = xRedis.GetRedisConn(dbi);
    if (!conn)
    {
        xRedis.Release();
        return 0;
    }
    xRedisContext* ctx = xRedis.GetxRedisContext(conn);
    int m = 3;
    if (xRedis.subscribe(dbi, conn, szKey)) {
        printf("xRedis.subscribe success\r\n");
        redisReply* reply = NULL;
        while(0==xRedis.GetReply(ctx, &reply)) {
            printf("xRedisClient::GetReply was called ok, reply->type:%d\n", reply->type);
            if (reply->type==REDIS_REPLY_ARRAY) {
                for (size_t i = 0; i<reply->elements; i++) {
                    std::string str(reply->element[i]->str, reply->element[i]->len);
                    printf("element i:%lu str:%s, len:%lu\n", i, str.c_str(), str.length());
                }
            }
            xRedis.FreeReply(reply);
            reply = NULL;
            if (m-- == 0) break;
        }
        xRedis.FreeReply((redisReply*)ctx->reader->reply);
        ctx->reader->reply = NULL;
    } else {
         printf("xRedis.subscribe has err:%s\r\n", dbi.GetErrInfo());
    }

    RedisConn* conn1 = xRedis.GetRedisConn(dbi);
    if (!conn1)
    {
        xRedis.Release();
        return 0;
    }
    xRedisContext* ctx1 = xRedis.GetxRedisContext(conn1);
    if (xRedis.unsubscribe(dbi, conn1, szKey)) {
        printf("xRedis.unsubscribe success\r\n");
        xRedis.FreeReply((redisReply*)ctx1->reader->reply);
        ctx1->reader->reply = NULL;
    } else {
         printf("xRedis.unsubscribe has err:%s\r\n", dbi.GetErrInfo());
    }
    printf("xRedis FreexRedisContext call...\n");
    xRedis.FreexRedisConn(conn);
    xRedis.FreexRedisConn(conn1);
#endif

    xRedis.Keepalive();
    usleep(1000*1000*2);

    xRedis.Release();
    return 0;
}


