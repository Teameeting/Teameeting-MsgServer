//
//  RTHiredis.h
//  dyncRTConnector
//
//  Created by hp on 11/25/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMeeting__RTHiredis__
#define __dyncRTMeeting__RTHiredis__

#include <stdio.h>
#include <iostream>
#include <list>
#include "hiredis.h"
#include "rtklog.h"

#define HI_USER_CONNECTOR_ID "hi_user_connector_id"
#define HI_ROOM_OWNER_ID "hi_room_owner_id"

class RTHiredis{
public:
    // 1s = 1000ms = 1000 000us = 1000 000 000ns;
    void Connect(int mstimeout = -1);
    void DisConn();
    
    void SetHostAddr(const char* host, int port) {
        if(!host || port <= 1024) {
            LE("RTHiredis SetHostAddr failed\n");
         return;
        }
        m_host.assign(host);
        m_port = port;
    }
    
    void CmdPing();
    bool CmdSet(const std::string key, const std::string value);
    bool CmdGet(const std::string key, std::string& value);
    bool CmdDel(const std::string key);
    bool CmdHSet(const std::string hid, const std::string key, const std::string value);
    bool CmdHGet(const std::string hid, const std::string key, std::string& value);
    bool CmdHDel(const std::string hid, const std::string key, std::string& res);
    bool CmdHExists(const std::string hid, const std::string key, std::string& res);
    bool CmdHLen(const std::string hid, int* len);
    bool CmdHKeys(const std::string hid, std::list<const std::string>* ress);
    bool CmdHVals(const std::string hid, std::list<const std::string>* ress);
    virtual void MakeAbstract() = 0;
protected:
    RTHiredis():
        m_redisContext(NULL)
        ,m_host("127.0.0.1")
        ,m_port(6379)
        ,m_sTimeout(1)
        ,m_msTimeout(500){}
    virtual ~RTHiredis(){ DisConn(); }
private:
    bool HandleReply(redisReply* reply, std::string* res);
    bool HandleHReply(redisReply* reply, std::list<const std::string>* res);
    
    redisContext* m_redisContext;
    std::string m_host;
    int m_port;
    int m_sTimeout;
    int m_msTimeout;
};

class RTHiredisRemote : public RTHiredis{
public:
    static RTHiredisRemote* Instance() {
        static RTHiredisRemote s_remoteHiredis;
        return &s_remoteHiredis;
    }
    virtual void MakeAbstract() {}
private:
    RTHiredisRemote(){}
    virtual ~RTHiredisRemote(){}
};

class RTHiredisLocal : public RTHiredis {
public:
    static RTHiredisLocal* Instance() {
        static RTHiredisLocal s_localHiredis;
        return &s_localHiredis;
    }
    virtual void MakeAbstract() {}
private:
    RTHiredisLocal(){}
    virtual ~RTHiredisLocal(){}
};
#endif /* defined(__dyncRTMeeting__RTHiredis__) */
