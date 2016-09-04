//
//  PRTApnsPusher.h
//  MsgServerPusher
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerPusher__PRTApnsPusher__
#define __MsgServerPusher__PRTApnsPusher__

#include <stdio.h>
#include <iostream>
#include <map>

#include "PRTXRedis.h"
#include "RCPthread.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class PRTApnsPusher
    : public RCPthread {
public:
    PRTApnsPusher(const std::string& ip, int port);
    virtual ~PRTApnsPusher();

    bool Init();
    bool Unit();

    void Stop();

// from RCPthread
public:
    virtual void Run(void* data);

private:

private:
    std::string                 m_addr;
    int                         m_port;
    bool                        m_isRun;

    PRTXRedis                   m_xRedis;
};

#endif /* defined(__MsgServerPusher__PRTApnsPusher__) */
