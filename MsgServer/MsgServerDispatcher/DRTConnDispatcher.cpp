//
//  DRTConnDispatcher.cpp
//  dyncRTDispatcher
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "DRTConnDispatcher.h"
#include "DRTConnManager.h"


void DRTConnDispatcher::OnRecvEvent(const char*pData, int nLen)
{
    DRTConnManager::Instance().ProcessRecvEvent(pData, nLen);
}

void DRTConnDispatcher::OnTickEvent(const char*pData, int nLen)
{
    DRTConnManager::Instance().ProcessTickEvent(pData, nLen);
}

