//
//  MsgClient.m
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MsgClient.h"

MsgClient::MsgClient(id<MsgClientProtocol> protocol)
{
    m_protocol = protocol;
}

MsgClient::~MsgClient()
{

}

int MsgClient::MCInit(XMsgCallback* cb, const std::string& uid, const std::string& token, const std::string& nname, const std::string& server, int port)
{
    return Init(cb, uid, token, nname, server, port);
}

int MsgClient::MCUnin()
{
    return Unin();
}

int MsgClient::MCSndMsg(const std::string& roomid, const std::string& rname, const std::string& msg)
{
    return SndMsg(roomid, rname, msg);
}

int MsgClient::MCGetMsg(GETCMD cmd)
{
    return GetMsg(cmd);
}

int MsgClient::MCOptRoom(MEETCMD cmd, const std::string& roomid, const std::string& rname, const std::string& remain)
{
    return OptRoom(cmd, roomid, rname, remain);
}

int MsgClient::MCSndMsgTo(const std::string& roomid, const std::string& rname, const std::string& msg, const std::list<std::string>& ulist)
{
    return SndMsgTo(roomid, rname, msg, ulist);
}

int MsgClient::MCNotifyMsg(const std::string& roomid, const std::string& rname, SENDTAGS tags, const std::string& msg)
{
    return NotifyMsg(roomid, rname, tags, msg);
}

/////////////////////////////////////////////////

void MsgClient::OnSndMsg(const std::string& msg)
{
    std::cout << "MsgClient::OnSndMsg msg:" << msg << std::endl;
    NSString* ns_msg = [[NSString alloc] initWithUTF8String:msg.c_str()];
    [m_protocol OnSndMsgMsg:ns_msg];
}

void MsgClient::OnGetMsg(const std::string& msg)
{
    std::cout << "MsgClient::OnGetMsg msg:" << msg << std::endl;
    NSString* ns_msg = [[NSString alloc] initWithUTF8String:msg.c_str()];
    [m_protocol OnGetMsgMsg:ns_msg];
}

void MsgClient::OnMsgServerConnected()
{
    //std::cout << "MsgClient::OnMsgServerConnected was called" << std::endl;
    [m_protocol OnMsgServerConnected];
}

void MsgClient::OnMsgServerDisconnect()
{
    //std::cout << "MsgClient::OnMsgServerDisconnect was called" << std::endl;
    [m_protocol OnMsgServerDisconnect];
}

void MsgClient::OnMsgServerConnectionFailure()
{
    //std::cout << "MsgClient::OnMsgServerConnectionFailer was called" << std::endl;
    [m_protocol OnMsgServerConnectionFailure];
}

void MsgClient::OnMsgServerState(MSState state)
{
    std::cout << "MsgClient::OnMsgServerState was called state:" << state << std::endl;
    [m_protocol OnMsgServerStateConnState:(MCConnState)state];
}