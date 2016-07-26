//
//  GRTConnDispatcher.cpp
//  dyncRTGrouper
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "GRTConnDispatcher.h"
#include "GRTConnManager.h"


void GRTConnDispatcher::OnRecvEvent(const char*pData, int nLen)
{
    GRTConnManager::Instance().ProcessRecvEvent(pData, nLen);
}

void GRTConnDispatcher::OnTickEvent(const char*pData, int nLen)
{
    GRTConnManager::Instance().ProcessTickEvent(pData, nLen);
}

