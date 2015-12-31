//
//  RTHiredis.h
//  dyncRTMsgQueue
//
//  Created by hp on 11/25/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMsgQueue__RTHiredis__
#define __dyncRTMsgQueue__RTHiredis__

#include <stdio.h>
#include <iostream>
#include <list>
#include "hiredis.h"

#define HI_USER_CONNECTOR_ID "hi_user_connector_id"

class RTHiredis{
public:
    static RTHiredis* Instance() {
        static RTHiredis s_rtHiredis;
        return &s_rtHiredis;
    }
    // 1s = 1000ms = 1000 000us = 1000 000 000ns;
    void Connect(int mstimeout = -1);
    void DisConn();
    
    void SetHost(const char* host) { if(!host) return; s_host.assign(host); }
    void SetPort(int port) { if(port<=1024) return; s_port = port; }
    
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
private:
    bool HandleReply(redisReply* reply, std::string* res);
    bool HandleHReply(redisReply* reply, std::list<const std::string>* res);
    RTHiredis(){}
    ~RTHiredis(){ DisConn(); }
    
    static redisContext* s_redisContext;
    static std::string s_host;
    static int s_port;
    static int s_sTimeout;
    static int s_msTimeout;
};
#endif /* defined(__dyncRTMsgQueue__RTHiredis__) */
