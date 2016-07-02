//
//  MsgClient.mm
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MsgClient.h"
#import "CommonMsg.pbobjc.h"
#import "EntityMsg.pbobjc.h"
#import "EntityMsgType.pbobjc.h"

#import "MSSharedSeqnConfig.h"

int MsgClient::MCInit(const std::string& uid, const std::string& token, const std::string& nname)
{
    std::cout << "MCInit add module here, please implement it!!!" << std::endl;
    m_userId = [[NSString alloc] initWithUTF8String:uid.c_str()];
    m_token = [[NSString alloc] initWithUTF8String:token.c_str()];
    m_nname = [[NSString alloc] initWithUTF8String:nname.c_str()];
    m_sqlite3Manager = [[MSSqlite3Manager alloc] init];
    [m_sqlite3Manager initManager];
    return Init(uid, token, nname, pms::EModuleType(EModuleType_Tlive));
}

int MsgClient::MCUnin()
{
    if (m_sqlite3Manager)
    {
        [m_sqlite3Manager uninManager];
    }
    return Unin();
}

int MsgClient::MCRegisterMsgCb(XMsgCallback* cb)
{
    return RegisterMsgCb(cb);
}

int MsgClient::MCUnRegisterMsgCb(XMsgCallback* cb)
{
    return UnRegisterMsgCb(cb);
}

int MsgClient::MCConnToServer(const std::string& server, int port)
{
    ConnToServer(server, port);
    return 0;
}


int MsgClient::MCAddGroup(const std::string& groupid)
{
    AddGroup(groupid);
    return 0;
}

int MsgClient::MCRmvGroup(const std::string& groupid)
{
    RmvGroup(groupid);
    return 0;
}


int MsgClient::MCSyncMsg()
{
    SyncMsg();
    return 0;
}

int MsgClient::MCSendTxtMsg(const std::string& groupid, const std::string& content)
{
    SndMsg(groupid, "roomname", content, (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle);
    return 0;
}

int MsgClient::MCSendTxtMsgToUsr(const std::string& userid, const std::string& content)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    SndMsgTo("groupid", "grpname", content, (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
    return 0;
}

int MsgClient::MCSendTxtMsgToUsrs(const std::vector<std::string>& vusrs, const std::string& content)
{
    SndMsgTo("groupid", "grpname", content, (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, vusrs);
    return 0;
}


int MsgClient::MCNotifyLive(const std::string& groupid, const std::string& hostid)
{
    std::vector<std::string> uvec;
    uvec.push_back(hostid);
    SndMsgTo(groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tliv, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
    return 0;
}

int MsgClient::MCNotifyRedEnvelope(const std::string& groupid, const std::string& hostid)
{
    std::vector<std::string> uvec;
    uvec.push_back(hostid);
    SndMsgTo(groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tren, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
    return 0;
}

int MsgClient::MCNotifyBlacklist(const std::string& groupid, const std::string& userid)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    SndMsgTo(groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tblk, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
    return 0;
}

int MsgClient::MCNotifyForbidden(const std::string& groupid, const std::string& userid)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    SndMsgTo(groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tfbd, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
    return 0;
}

int MsgClient::MCNotifySettedMgr(const std::string& groupid, const std::string& userid)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    SndMsgTo(groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tmgr, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
    return 0;
}

/////////////////////////////////////////////////
//////////Callback From XMsgCallback/////////////
/////////////////////////////////////////////////

void MsgClient::OnSndMsg(int code, const std::string& msgid)
{
    std::cout << "MsgClient::OnSndMsg msgid:" << msgid << std::endl;
    //[m_txtMsgDelegate OnSendMessageId:msgid code:code];
}

void MsgClient::OnCmdGroup(int code, int cmd, const std::string& groupid, const MSCbData& data)
{
    if (cmd == 0) // add
    {
        if (code == 0)
        {
            NSString *nsGrpId = [NSString stringWithCString:groupid.c_str() encoding:NSUTF8StringEncoding];
            NSLog(@"OnCmdGroup add group ok, insert groupid and seqn, toggle callback");
            [m_sqlite3Manager addGroupId:nsGrpId];
            [m_sqlite3Manager addGroupSeqnGrpId:nsGrpId seqn:[NSNumber numberWithLongLong:data.seqn]];
            [m_groupDelegate OnAddGroupSuccessGrpId:nsGrpId];
        } else if (code == -1)
        {
            [m_groupDelegate OnAddGroupFailedGrpId:[[NSString alloc] initWithUTF8String:groupid.c_str()] reason:[[NSString alloc] initWithUTF8String:data.data.c_str()] code:code];
        }
    } else if (cmd == 1) // del
    {
        if (code == 0)
        {
            NSString *nsGrpId = [NSString stringWithCString:groupid.c_str() encoding:NSUTF8StringEncoding];
            NSLog(@"OnCmdGroup del group ok, del groupid and seqn, toggle callback");
            [m_sqlite3Manager delGroupId:nsGrpId];
            [m_groupDelegate OnRmvGroupSuccessGrpId:nsGrpId];
        } else if (code == -1)
        {
            [m_groupDelegate OnRmvGroupFailedGrpId:[[NSString alloc] initWithUTF8String:groupid.c_str()] reason:[[NSString alloc] initWithUTF8String:data.data.c_str()] code:code];
        }
    }
}


void MsgClient::OnRecvMsg(const std::string& msg)
{
    std::cout << "MsgClient::OnGetMsg msg:" << msg << std::endl;
    NSString* ns_msg = [[NSString alloc] initWithUTF8String:msg.c_str()];
    NSData *data_msg = [[NSData alloc] initWithBytes:msg.c_str() length:msg.length()];
    GPBExtensionRegistry *extReg = [[GPBExtensionRegistry alloc] init];
    Entity *entity = [[Entity alloc] initWithData:data_msg extensionRegistry:extReg error:nil];
    
    
    Entity* entityMsg = [Entity parseFromData:data_msg error:nullptr];
    NSLog(@"MSMsgSender::OnRecvMsg EntityMsg:%@\n", [entityMsg debugDescription]);
    NSString* cont = [entityMsg msgCont];
    EMsgTag tag = [entityMsg msgTag];
    
    NSLog(@"OnRecvMsg tag:%d, type:%d, flag:%d, head:%d", entity.msgTag, entity.msgType, entity.msgFlag, entity.msgHead);
    //[m_txtMsgDelegate OnRecvSelfDefMessage:<#(MSTxtMessage *)#>];
    //[m_txtMsgDelegate OnNotifyLiveId:<#(NSString *)#> hostId:<#(NSString *)#>];
    //[m_txtMsgDelegate OnNotifyRedEnvelopeGrpId:<#(NSString *)#> hostId:<#(NSString *)#>];
    //[m_txtMsgDelegate OnNotifyBlacklist:<#(NSString *)#> userId:<#(NSString *)#>];
    //[m_txtMsgDelegate OnNotifyForbidden:<#(NSString *)#> userId:<#(NSString *)#>];
    //[m_txtMsgDelegate OnNotifySettedMgrGrpId:<#(NSString *)#> userId:<#(NSString *)#>];
}

void MsgClient::OnRecvGroupMsg(const std::string& msg)
{
    //std::cout << "MsgClient::OnRecvGroupMsg was called" << std::endl;
    //[m_txtMsgDelegate OnRecvTxtMessage:<#(MSTxtMessage *)#>];
}

void MsgClient::OnSyncSeqn(int64 seqn)
{
    std::cout << "MsgClient::OnSyncSeqn was called seqn:" << seqn << std::endl;
    //[MSSharedSeqnConfig updateGroupSeqnGroupId:m_userId Seqn:[NSNumber numberWithLongLong:seqn]];
    [m_sqlite3Manager updateUserSeqnUserId:m_userId seqn:[NSNumber numberWithLongLong:seqn]];
}

void MsgClient::OnSyncGroupSeqn(const std::string &groupid, int64 seqn)
{
    std::cout << "MsgClient::OnSyncGroupSeqn was called groupid:" << groupid << ", seqn: " << seqn << std::endl;
    //[MSSharedSeqnConfig updateGroupSeqnGroupId:[[NSString alloc] initWithUTF8String:groupid.c_str()] Seqn:[NSNumber numberWithLongLong:seqn]];
    [m_sqlite3Manager updateGroupSeqnGrpId:[NSString stringWithUTF8String:groupid.c_str()] seqn:[NSNumber numberWithLongLong:seqn]];
}

void MsgClient::OnMsgServerConnected()
{
    //std::cout << "MsgClient::OnMsgServerConnected was called" << std::endl;
    [m_clientDelegate OnMsgServerConnected];
}


void MsgClient::OnMsgServerConnecting()
{
    //std::cout << "MsgClient::OnMsgServerConnecting was called" << std::endl;
    [m_clientDelegate OnMsgServerConnecting];
}

void MsgClient::OnMsgServerDisconnect()
{
    //std::cout << "MsgClient::OnMsgServerDisconnect was called" << std::endl;
    [m_clientDelegate OnMsgServerDisconnect];
}

void MsgClient::OnMsgServerConnectionFailure()
{
    //std::cout << "MsgClient::OnMsgServerConnectionFailer was called" << std::endl;
    [m_clientDelegate OnMsgServerConnectionFailure];
}
