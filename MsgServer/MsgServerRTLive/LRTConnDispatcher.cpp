//
//  LRTConnDispatcher.cpp
//  dyncRTDispatcher
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "LRTConnDispatcher.h"
#include "LRTConnManager.h"


void LRTConnDispatcher::OnRecvEvent(const char*pData, int nLen)
{
    LRTConnManager::Instance().ProcessRecvEvent(pData, nLen);
}

void LRTConnDispatcher::OnTickEvent(const char*pData, int nLen)
{
    LRTConnManager::Instance().ProcessTickEvent(pData, nLen);
}

