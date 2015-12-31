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

int MsgClient::MCInit(XMsgCallback& cb, const std::string& server, int port)
{
    return Init(cb, server, port);
}

int MsgClient::MCUnin()
{
    return Unin();
}

int MsgClient::MCLogin(const std::string& userid, const std::string& pass)
{
    return Login(userid, pass);
}

int MsgClient::MCSndMsg(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg)
{
    return SndMsg(userid, pass, roomid, msg);
}

int MsgClient::MCGetMsg(const std::string& userid, const std::string& pass)
{
    return GetMsg(userid, pass);
}

int MsgClient::MCLogout(const std::string& userid, const std::string& pass)
{
    return Logout(userid, pass);
}

int MsgClient::MCCreateRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    return CreateRoom(userid, pass, roomid);
}

int MsgClient::MCEnterRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    return EnterRoom(userid, pass, roomid);
}

int MsgClient::MCLeaveRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    return LeaveRoom(userid, pass, roomid);
}

int MsgClient::MCDestroyRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    return DestroyRoom(userid, pass, roomid);
}

int MsgClient::MCSndMsgTo(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg, const std::list<std::string>& ulist)
{
    return SndMsgTo(userid, pass, roomid, msg, ulist);
}

/////////////////////////////////////////////////

void MsgClient::OnLogin(int code)
{
    std::cout << "MsgClient::OnLogin code:" << code << std::endl;
    [m_protocol OnLoginCode:code];
}

void MsgClient::OnSndMsg(const std::string msg)
{
    std::cout << "MsgClient::OnSndMsg msg:" << msg << std::endl;
    NSString* ns_msg = [[NSString alloc] initWithUTF8String:msg.c_str()];
    [m_protocol OnSndMsgMsg:ns_msg];
}

void MsgClient::OnGetMsg(const std::string from, const std::string msg)
{
    std::cout << "MsgClient::OnGetMsg from:" << from << ",msg:" << msg << std::endl;
    NSString* ns_from = [[NSString alloc] initWithUTF8String:from.c_str()];
    NSString* ns_msg = [[NSString alloc] initWithUTF8String:msg.c_str()];
    [m_protocol OnGetMsgFrom:ns_from msg:ns_msg];
}

void MsgClient::OnLogout(int code)
{
    std::cout << "MsgClient::OnLogout code:" << code << std::endl;
    [m_protocol OnLogoutCode:code];
}