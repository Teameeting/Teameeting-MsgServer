//
//  MRTRoomDispatcher.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "MRTRoomDispatcher.h"
#include "MRTRoomManager.h"


void MRTRoomDispatcher::OnTickEvent(const char*pData, int nLen)
{
    MRTRoomManager::Instance().ProcessTickEvent(pData, nLen);
}

void MRTRoomDispatcher::ConnectionDisconnected()
{
    LI("%s was called\n", __FUNCTION__);
}
