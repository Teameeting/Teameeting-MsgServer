//
//  PRTConnDispatcher.cpp
//  dyncRTDispatcher
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "PRTConnDispatcher.h"
#include "PRTConnManager.h"


void PRTConnDispatcher::OnRecvEvent(const char*pData, int nLen)
{
    PRTConnManager::Instance().ProcessRecvEvent(pData, nLen);
}

void PRTConnDispatcher::OnTickEvent(const char*pData, int nLen)
{
    PRTConnManager::Instance().ProcessTickEvent(pData, nLen);
}

void PRTConnDispatcher::ConnectionDisconnected()
{
    LI("%s was called\n", __FUNCTION__);
}

