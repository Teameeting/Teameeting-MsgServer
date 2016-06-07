//
//  MRTConnDispatcher.cpp
//  dyncRTDispatcher
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "MRTConnDispatcher.h"
#include "MRTConnManager.h"


void MRTConnDispatcher::OnRecvEvent(const char*pData, int nLen)
{
    MRTConnManager::Instance().ProcessRecvEvent(pData, nLen);
}

void MRTConnDispatcher::OnTickEvent(const char*pData, int nLen)
{
    MRTConnManager::Instance().ProcessTickEvent(pData, nLen);
}

