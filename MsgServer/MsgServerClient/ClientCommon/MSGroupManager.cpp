//
//  MSGroupManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "MSGroupManager.h"
#include "MsgClient.h"

void MSGroupManager::AddDelegate(const MSGroupDelegate* grpDelegate)
{
    MsgClient::Instance().MCSetGroupDelegate(grpDelegate);
}

void MSGroupManager::DelDelegate(const MSGroupDelegate* grpDelegate)
{
    MsgClient::Instance().MCSetGroupDelegate(nullptr);
}

void MSGroupManager::AddGroup(const std::string& grpId)
{
    if (grpId.length()==0) return;
    MsgClient::Instance().MCAddGroup(grpId);
}

void MSGroupManager::RmvGroup(const std::string& grpId)
{
    if (grpId.length()==0) return;
    MsgClient::Instance().MCRmvGroup(grpId);
}
