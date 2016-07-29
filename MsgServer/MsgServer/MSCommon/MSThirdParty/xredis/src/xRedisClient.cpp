/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2013-2014, xSky <guozhw at gmail dot com>
 * All rights reserved.
 * Distributed under GPL license.
 * ----------------------------------------------------------------------------
 */

#include "xRedisClient.h"
#include "xRedisPool.h"
#include <sstream>
#include <iostream>

RedisDBIdx::RedisDBIdx() {
    mType = 0;
    mIndex = 0;
    mStrerr = NULL;
    mClient = NULL;
    mIOtype = MASTER;
    mIOFlag = false;
}

RedisDBIdx::RedisDBIdx(xRedisClient *xredisclient) {
    mType = 0;
    mIndex = 0;
    mStrerr = NULL;
    mClient = xredisclient;
    mIOtype = MASTER;
    mIOFlag = false;
}
RedisDBIdx::~RedisDBIdx() {
    if (NULL != mStrerr){
        delete[] mStrerr;
        mStrerr = NULL;
    }
}

bool RedisDBIdx::CreateDBIndex(const char *key,  HASHFUN fun, const unsigned int type) {
    unsigned int hashbase = mClient->GetRedisPool()->getHashBase(type);
    if ((NULL!=fun) && (hashbase>0)) {
        mIndex = fun(key)%hashbase;
        mType  = type;
        return true;
    }
    return false;
}

bool RedisDBIdx::CreateDBIndex(const int64_t id, const unsigned int type) {
    unsigned int hashbase = mClient->GetRedisPool()->getHashBase(type);
    if (hashbase>0) {
        mType  = type;
        mIndex = id%hashbase;
        return true;
    }
    return false;
}

void RedisDBIdx::IOtype(unsigned int type) {
    mIOtype = type;
}

void RedisDBIdx::SetIOMaster()
{
    mIOtype = MASTER;
    mIOFlag = true;
}

bool RedisDBIdx::SetErrInfo(const char *info, int len) {
    if (NULL == info) {
        return false;
    }
    if (NULL == mStrerr){
        mStrerr = new char[len + 1];
    }
    if (NULL != mStrerr) {
        strncpy(mStrerr, info, len);
        mStrerr[len] = '\0';
        return true;
    }
    return false;
}

xRedisClient::xRedisClient()
{
    mRedisPool = NULL;
}


xRedisClient::~xRedisClient()
{
    Release();
}

bool xRedisClient::Init(unsigned int maxtype) {
    if(NULL==mRedisPool) {
        mRedisPool = new RedisPool;
        if (NULL==mRedisPool) {
            return false;
        }

        return mRedisPool->Init(maxtype);
    }
    return false;
}

void xRedisClient::Release() {
    if (NULL!=mRedisPool) {
        mRedisPool->Release();
        delete mRedisPool;
        mRedisPool = NULL;
    }
}

void xRedisClient::Keepalive() {
    if (NULL!=mRedisPool) {
        mRedisPool->Keepalive();
    }
}

inline RedisPool *xRedisClient::GetRedisPool() {
    return mRedisPool;
}

void xRedisClient::FreeReply(const rReply* reply)
{
    RedisPool::FreeReply((redisReply*)reply);
}

bool xRedisClient::ConnectRedisCache( const RedisNode *redisnodelist, unsigned int hashbase, unsigned int cachetype) {
    if (NULL==mRedisPool) {
        return false;
    }

    if (!mRedisPool->setHashBase(cachetype, hashbase)) {
        return false;
    }

    for (unsigned int n = 0; n<hashbase; n++) {
        const RedisNode *pNode = &redisnodelist[n];
        if (NULL==pNode) {
            return false;
        }

        bool bRet = mRedisPool->ConnectRedisDB(cachetype, pNode->dbindex, pNode->host, pNode->port,
            pNode->passwd, pNode->poolsize, pNode->timeout, pNode->role);
        if (!bRet) {
            return false;
        }
    }

    return true;
}

int xRedisClient::GetReply(xRedisContext* ctx, redisReply** reply)
{
    return redisGetReply(ctx, (void**)reply);
}

void xRedisClient::FreeReply(redisReply* reply)
{
    RedisPool::FreeReply(reply);
}

void xRedisClient::FreexRedisConn(RedisConn* conn)
{
    mRedisPool->FreeConnection(conn);
}

RedisConn* xRedisClient::GetRedisConn(const RedisDBIdx& dbi)
{
    RedisConn *pRedisConn = NULL;
    pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
    }
    return pRedisConn;
}

xRedisContext* xRedisClient::GetxRedisContext(const RedisConn* conn)
{
     if (!conn) return NULL;
     return (xRedisContext*)conn->getCtx();
}



void xRedisClient::SetErrInfo(const RedisDBIdx& dbi, void *p) {
    if (NULL==p){
        SetErrString(dbi, CONNECT_CLOSED_ERROR, ::strlen(CONNECT_CLOSED_ERROR));
    } else {
        redisReply *reply = (redisReply*)p;
        SetErrString(dbi, reply->str, reply->len);
    }
}

void xRedisClient::SetErrString(const RedisDBIdx& dbi, const char *str, int len) {
    RedisDBIdx &dbindex = const_cast<RedisDBIdx&>(dbi);
    dbindex.SetErrInfo(str, len);
}

void xRedisClient::SetIOtype(const RedisDBIdx& dbi, unsigned int iotype, bool ioflag) {
    RedisDBIdx &dbindex = const_cast<RedisDBIdx&>(dbi);
    dbindex.IOtype(iotype);
    dbindex.mIOFlag = ioflag;
}

void xRedisClient::SetErrMessage(const RedisDBIdx& dbi, const char* fmt, ...)
{
    char szBuf[128] = { 0 };
    va_list va;
    va_start(va, fmt);
    vsnprintf(szBuf, sizeof(szBuf), fmt, va);
    va_end(va);
    SetErrString(dbi, szBuf, ::strlen(szBuf));
}

rReply *xRedisClient::command(const RedisDBIdx& dbi, const char* cmd)
{
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return NULL;
    }
    rReply *reply = static_cast<rReply *>(redisCommand(pRedisConn->getCtx(), cmd));

    mRedisPool->FreeConnection(pRedisConn);
    return reply;
}

bool xRedisClient::command_bool(const RedisDBIdx& dbi, const char *cmd, ...) {
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    va_list args;
    va_start(args, cmd);
    redisReply *reply = static_cast<redisReply *>(redisvCommand(pRedisConn->getCtx(), cmd, args));
    va_end(args);

    if (RedisPool::CheckReply(reply)) {
        if (REDIS_REPLY_STATUS==reply->type) {
            bRet = true;
        } else {
            bRet = (reply->integer == 1) ? true : false;
        }
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);

    return bRet;
}

bool xRedisClient::command_status(const RedisDBIdx& dbi, const char* cmd, ...) {
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    va_list args;
    va_start(args, cmd);
    redisReply *reply = static_cast<redisReply *>(redisvCommand(pRedisConn->getCtx(), cmd, args));
    va_end(args);

    if (RedisPool::CheckReply(reply)) {
        bRet = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);

    return bRet;
}

bool xRedisClient::command_integer(const RedisDBIdx& dbi, int64_t &retval, const char* cmd, ...) {
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    va_list args;
    va_start(args, cmd);
    redisReply *reply = static_cast<redisReply *>(redisvCommand(pRedisConn->getCtx(), cmd, args));
    va_end(args);
    if (RedisPool::CheckReply(reply)) {
        retval = reply->integer;
        bRet = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);

    return bRet;
}

bool xRedisClient::command_string(const RedisDBIdx& dbi, string &data, const char* cmd, ...) {
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    va_list args;
    va_start(args, cmd);
    redisReply *reply = static_cast<redisReply *>(redisvCommand(pRedisConn->getCtx(), cmd, args));
    va_end(args);
    if (RedisPool::CheckReply(reply)) {
        data.assign(reply->str, reply->len);
        bRet = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);

    return bRet;
}

bool xRedisClient::command_list(const RedisDBIdx& dbi, VALUES &vValue, const char* cmd, ...) {
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    va_list args;
    va_start(args, cmd);
    redisReply *reply = static_cast<redisReply *>(redisvCommand(pRedisConn->getCtx(), cmd, args));
    va_end(args);
    if (RedisPool::CheckReply(reply)) {
        for (size_t i = 0; i<reply->elements; i++) {
            vValue.push_back(string(reply->element[i]->str, reply->element[i]->len));
        }
        bRet  = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);

    return bRet;
}

bool xRedisClient::command_array(const RedisDBIdx& dbi,  ArrayReply& array,  const char* cmd, ...){
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    va_list args;
    va_start(args, cmd);
    redisReply *reply = static_cast<redisReply *>(redisvCommand(pRedisConn->getCtx(), cmd, args));
    va_end(args);
    if (RedisPool::CheckReply(reply)) {
        for (size_t i = 0; i<reply->elements; i++) {
            DataItem item;
            item.type = reply->element[i]->type;
            item.str.assign(reply->element[i]->str, reply->element[i]->len);
            array.push_back(item);
        }
        bRet  = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);
    return bRet;
}

bool xRedisClient::command_withconn(const RedisConn* conn, const char* cmd, ...) {
     bool bRet = false;
     if (!conn) return bRet;

    va_list args;
    va_start(args, cmd);
    redisReply *reply = static_cast<redisReply *>(redisvCommand(conn->getCtx(), cmd, args));
    va_end(args);
    if (RedisPool::CheckReply(reply)) {
        std::cout << "xRedisClient::command_withconn CheckReply ok" << std::endl;
        if (reply->type == REDIS_REPLY_ARRAY) {
            string cmd(reply->element[0]->str, reply->element[0]->len);
            string chl(reply->element[1]->str, reply->element[1]->len);
            string msg(reply->element[2]->str, reply->element[2]->len);
            std::cout << "command_withconn cmd:" << cmd << std::endl;
            std::cout << "command_withconn chl:" << chl << std::endl;
            std::cout << "command_withconn  numsub is:" << reply->element[2]->integer << std::endl;
        }
        bRet  = true;
    } else {
        std::cerr << "xRedisClient::command_withconn CheckReply err" << std::endl;
    }
    RedisPool::FreeReply(reply);
    return bRet;
}


bool xRedisClient::commandargv_bool(const RedisDBIdx& dbi, const VDATA& vData) {
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return bRet;
    }

    vector<const char *> argv( vData.size() );
    vector<size_t> argvlen( vData.size() );
    unsigned int j = 0;
    for ( VDATA::const_iterator i = vData.begin(); i != vData.end(); ++i, ++j ) {
        argv[j] = i->c_str(), argvlen[j] = i->size();
    }

    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(pRedisConn->getCtx(), argv.size(), &(argv[0]), &(argvlen[0])));
    if (RedisPool::CheckReply(reply)) {
        bRet = (reply->integer==1)?true:false;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);

    return bRet;
}

bool xRedisClient::commandargv_status(const RedisDBIdx& dbi, const VDATA& vData) {
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return bRet;
    }

    vector<const char *> argv( vData.size() );
    vector<size_t> argvlen( vData.size() );
    unsigned int j = 0;
    for ( VDATA::const_iterator i = vData.begin(); i != vData.end(); ++i, ++j ) {
        argv[j] = i->c_str(), argvlen[j] = i->size();
    }

    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(pRedisConn->getCtx(), argv.size(), &(argv[0]), &(argvlen[0])));
    if (RedisPool::CheckReply(reply)) {
        bRet = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);

    return bRet;
}

bool xRedisClient::commandargv_array(const RedisDBIdx& dbi, const VDATA& vDataIn, ArrayReply& array){
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    vector<const char*> argv( vDataIn.size() );
    vector<size_t> argvlen( vDataIn.size() );
    unsigned int j = 0;
    for ( VDATA::const_iterator i = vDataIn.begin(); i != vDataIn.end(); ++i, ++j ) {
        argv[j] = i->c_str(), argvlen[j] = i->size();
    }

    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(pRedisConn->getCtx(), argv.size(), &(argv[0]), &(argvlen[0])));
    if (RedisPool::CheckReply(reply)) {
        for (size_t i = 0; i<reply->elements; i++) {
            DataItem item;
            item.type = reply->element[i]->type;
            item.str.assign(reply->element[i]->str, reply->element[i]->len);
            array.push_back(item);
        }
        bRet  = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);
    return bRet;
}

bool xRedisClient::commandargv_array(const RedisDBIdx& dbi, const VDATA& vDataIn, VALUES& array){
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    vector<const char*> argv( vDataIn.size() );
    vector<size_t> argvlen( vDataIn.size() );
    unsigned int j = 0;
    for ( VDATA::const_iterator i = vDataIn.begin(); i != vDataIn.end(); ++i, ++j ) {
        argv[j] = i->c_str(), argvlen[j] = i->size();
    }

    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(pRedisConn->getCtx(), argv.size(), &(argv[0]), &(argvlen[0])));
    if (RedisPool::CheckReply(reply)) {
        for (size_t i = 0; i<reply->elements; i++) {
            string str(reply->element[i]->str, reply->element[i]->len);
            array.push_back(str);
        }
        bRet  = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);
    return bRet;
}

bool xRedisClient::commandargv_integer(const RedisDBIdx& dbi, const VDATA& vDataIn, int64_t& retval){
    bool bRet = false;
    RedisConn *pRedisConn = mRedisPool->GetConnection(dbi.mType, dbi.mIndex, dbi.mIOtype);
    if (NULL == pRedisConn) {
        SetErrString(dbi, GET_CONNECT_ERROR, ::strlen(GET_CONNECT_ERROR));
        return false;
    }

    vector<const char*> argv( vDataIn.size() );
    vector<size_t> argvlen( vDataIn.size() );
    unsigned int j = 0;
    for ( VDATA::const_iterator iter = vDataIn.begin(); iter != vDataIn.end(); ++iter, ++j ) {
        argv[j] = iter->c_str(), argvlen[j] = iter->size();
    }

    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(pRedisConn->getCtx(), argv.size(), &(argv[0]), &(argvlen[0])));
    if (RedisPool::CheckReply(reply)) {
        retval = reply->integer;
        bRet  = true;
    } else {
        SetErrInfo(dbi, reply);
    }

    RedisPool::FreeReply(reply);
    mRedisPool->FreeConnection(pRedisConn);
    return bRet;
}


bool xRedisClient::commandargv_withconn(const RedisConn* conn, const VDATA& vDataIn) {
    bool bRet = false;
    if (!conn) return bRet;

    vector<const char*> argv( vDataIn.size() );
    vector<size_t> argvlen( vDataIn.size() );
    unsigned int j = 0;
    for ( VDATA::const_iterator iter = vDataIn.begin(); iter != vDataIn.end(); ++iter, ++j ) {
        argv[j] = iter->c_str(), argvlen[j] = iter->size();
    }

    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(conn->getCtx(), argv.size(), &(argv[0]), &(argvlen[0])));
    if (RedisPool::CheckReply(reply)) {
        std::cout << "xRedisClient::commandargv_withconn CheckReply VDATA ok" << std::endl;
        if (reply->type == REDIS_REPLY_ARRAY) {
            string cmd(reply->element[0]->str, reply->element[0]->len);
            string chl(reply->element[1]->str, reply->element[1]->len);
            string msg(reply->element[2]->str, reply->element[2]->len);
            std::cout << "commandargv_withconn cmd:" << cmd << std::endl;
            std::cout << "commandargv_withconn chl:" << chl << std::endl;
            std::cout << "commandargv_withconn  numsub is:" << reply->element[2]->integer << std::endl;
        }
        bRet  = true;
    } else {
        std::cerr << "xRedisClient::commandargv_withconn CheckReply err" << std::endl;
    }
    RedisPool::FreeReply(reply);
    return bRet;
}


