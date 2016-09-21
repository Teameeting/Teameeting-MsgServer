//
//  MSClientManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "MSClientManager.h"
#include "MsgClient.h"


int MSClientManager::InitMsgClient(const std::string& usrId, const std::string& token, const std::string& nName)
{
    LI("MSClientManager Linux Version:v1.0.0");
    if (usrId.length()==0||token.length()==0||(nName.length()==0)) return -101;
    MsgClient::Instance().MCInit(usrId, token, nName);
    MsgClient::Instance().MCRegisterMsgCb(&MsgClient::Instance());
    return 0;
}

int MSClientManager::UninMsgClient()
{
    MsgClient::Instance().MCUnRegisterMsgCb(&MsgClient::Instance());
    MsgClient::Instance().MCUnin();
    return 0;
}

void MSClientManager::AddDelegate(const MSClientDelegate* cltDelegate)
{
    MsgClient::Instance().MCSetClientDelegate(cltDelegate);
}

void MSClientManager::DelDelegate(const MSClientDelegate* cltDelegate)
{
    MsgClient::Instance().MCSetClientDelegate(nullptr);
}

int MSClientManager::ConnToServer(const std::string& server, int port)
{
    if (server.length()==0 || port <0) return -101;
    MsgClient::Instance().MCConnToServer(server, port);
    return 0;
}

MCConnState MSClientManager::ConnStatus()
{
    MsgClient::Instance().MCConnStatus();
    return MCConnStateNOT_CONNECTED;
}

void MSClientManager::SetUserId(const std::string& userId)
{
   //DO NOT IMPLEMENT
}

void MSClientManager::SetToken(const std::string& token)
{
    if (token.length()==0) return;
    MsgClient::Instance().MCSetToken(token);
}

void MSClientManager::SetNickName(const std::string& nName)
{
    if (nName.length()==0) return;
    MsgClient::Instance().MCSetNickName(nName);
}

void MSClientManager::SetUIconUrl(const std::string& url)
{
    if (url.length()==0) return;
    MsgClient::Instance().MCSetUIconUrl(url);
}

void MSClientManager::EnablePush(int push)
{
    MsgClient::Instance().MCEnablePush(push);
}

void MSClientManager::MuteNotify(int mute)
{
    MsgClient::Instance().MCMuteNotify(mute);
}

