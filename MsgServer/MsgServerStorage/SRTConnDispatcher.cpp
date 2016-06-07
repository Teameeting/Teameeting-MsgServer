//
//  SRTConnDispatcher.cpp
//  dyncRTDispatcher
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTConnDispatcher.h"
#include "SRTConnManager.h"


void SRTConnDispatcher::OnRecvEvent(const char*pData, int nLen)
{
    SRTConnManager::Instance().ProcessRecvEvent(pData, nLen);
}

void SRTConnDispatcher::OnTickEvent(const char*pData, int nLen)
{
    SRTConnManager::Instance().ProcessTickEvent(pData, nLen);
}

