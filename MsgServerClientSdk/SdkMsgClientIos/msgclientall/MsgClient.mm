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
    m_strUserId = uid;
    m_strToken = token;
    m_strNname = nname;
    m_nsUserId = [[NSString alloc] initWithUTF8String:uid.c_str()];
    m_nsToken = [[NSString alloc] initWithUTF8String:token.c_str()];
    m_nsNname = [[NSString alloc] initWithUTF8String:nname.c_str()];
    
    m_recurLock = [[NSRecursiveLock alloc] init];
    m_sqlite3Manager = [[MSSqlite3Manager alloc] init];
    [m_sqlite3Manager initManager];
    CheckUserOrInit(m_nsUserId);
    GetLocalSeqnsFromDb();
    return Init(uid, token, nname, pms::EModuleType(EModuleType_Tlive));
}

int MsgClient::MCUnin()
{
    PutLocalSeqnsToDb();
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
    SyncSeqn(m_userSeqn, (pms::EMsgRole)EMsgRole_Rrecver);
    for (auto &it : m_groupSeqn)
    {
        SyncGroupSeqn(it.first, it.second, (pms::EMsgRole)EMsgRole_Rrecver);
    }
    return 0;
}

int MsgClient::MCSendTxtMsg(std::string& outmsgid, const std::string& groupid, const std::string& content)
{
    return SndMsg(outmsgid, groupid, "roomname", content, (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fgroup);
}

int MsgClient::MCSendTxtMsgToUsr(std::string& outmsgid, const std::string& userid, const std::string& content)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    return SndMsgTo(outmsgid, "groupid", "grpname", content, (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
}

int MsgClient::MCSendTxtMsgToUsrs(std::string& outmsgid, const std::vector<std::string>& vusrs, const std::string& content)
{
    return SndMsgTo(outmsgid, "groupid", "grpname", content, (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, vusrs);
}


int MsgClient::MCNotifyLive(std::string& outmsgid, const std::string& groupid, const std::string& hostid)
{
    std::vector<std::string> uvec;
    uvec.push_back(hostid);
    return SndMsgTo(outmsgid, groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tliv, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
}

int MsgClient::MCNotifyRedEnvelope(std::string& outmsgid, const std::string& groupid, const std::string& hostid)
{
    std::vector<std::string> uvec;
    uvec.push_back(hostid);
    return SndMsgTo(outmsgid, groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tren, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
}

int MsgClient::MCNotifyBlacklist(std::string& outmsgid, const std::string& groupid, const std::string& userid)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    return SndMsgTo(outmsgid, groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tblk, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
}

int MsgClient::MCNotifyForbidden(std::string& outmsgid, const std::string& groupid, const std::string& userid)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    return SndMsgTo(outmsgid, groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tfbd, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
}

int MsgClient::MCNotifySettedMgr(std::string& outmsgid, const std::string& groupid, const std::string& userid)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    return SndMsgTo(outmsgid, groupid, "grpname", "", (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tmgr, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
}

/////////////////////////////////////////////////
//////////Callback From XMsgCallback/////////////
/////////////////////////////////////////////////

void MsgClient::OnSndMsg(int code, const std::string& msgid)
{
    std::cout << "MsgClient::OnSndMsg msgid:" << msgid << ", code:" << code << std::endl;
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
            UpdateLocalSeqn(groupid, data.seqn);
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
            RemoveLocalSeqn(groupid);
            [m_groupDelegate OnRmvGroupSuccessGrpId:nsGrpId];
        } else if (code == -1)
        {
            [m_groupDelegate OnRmvGroupFailedGrpId:[[NSString alloc] initWithUTF8String:groupid.c_str()] reason:[[NSString alloc] initWithUTF8String:data.data.c_str()] code:code];
        }
    }
}


void MsgClient::OnRecvMsg(int64 seqn, const std::string& msg)
{
    UpdateLocalSeqn(m_strUserId, seqn);
    
    std::cout << "MsgClient::OnRecvMsg msg:" << msg << std::endl;
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

void MsgClient::OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg)
{
    //std::cout << "MsgClient::OnRecvGroupMsg was called" << std::endl;
    UpdateLocalSeqn(seqnid, seqn);
    //[m_txtMsgDelegate OnRecvTxtMessage:<#(MSTxtMessage *)#>];
}

void MsgClient::OnSyncSeqn(int64 seqn, int role)
{
    std::cout << "MsgClient::OnSyncSeqn was called seqn:" << seqn << std::endl;
    //[m_sqlite3Manager addUserId:m_userId];
    //[m_sqlite3Manager updateUserSeqnUserId:m_userId seqn:[NSNumber numberWithLongLong:seqn]];
    
    // if it is sender, this means client send msg, and just get the newmsg's seqn
    // if the new seqn is bigger 1 than cur seqn, it is ok, just update seqn.
    // if the new seqn is bigger 2 or 3 than cur seqn, this need sync data
    // if it is recver, this means client need sync data
    int index = seqn - m_userSeqn;
    if (role == EMsgRole_Rsender)
    {
        if (index == 1)
        {
            m_userSeqn = seqn;
            UpdateLocalSeqn(m_strUserId, seqn);
        } else if (index > 1)
        {
            SyncData(m_userSeqn);
        }
    }  else if (role == EMsgRole_Rrecver)
    {
        if (index >0)
        {
            SyncData(m_userSeqn);
        }
    }
}

// get group max seqn
void MsgClient::OnSyncGroupSeqn(const std::string &groupid, int64 seqn)
{
    std::cout << "MsgClient::OnSyncGroupSeqn was called groupid:" << groupid << ", seqn: " << seqn << std::endl;
    //[m_sqlite3Manager updateGroupSeqnGrpId:[NSString stringWithUTF8String:groupid.c_str()] seqn:[NSNumber numberWithLongLong:seqn]];
    // this need sync data
    int64 lseqn = GetLocalSeqnFromId(groupid);
    assert(lseqn>0);
    if (seqn > lseqn)
        SyncGroupData(groupid, lseqn);
}

// there are new group msg to sync
void MsgClient::OnGroupNotify(int code, const std::string& seqnid)
{
    if (code==0)
    {
        int64 seqn = GetLocalSeqnFromId(seqnid);
        if (seqn>0)
            SyncGroupData(seqnid, seqn);
        else
            assert(seqn>0);
    } else {
        NSLog(@"MsgClient::OnGroupNotify error code is :%d!!!", code);
    }
}

void MsgClient::OnNotifySeqn(int code, const std::string& seqnid)
{
    if (code==0)
    {
        if (seqnid.length()>0)
        {
            NSLog(@"THIS SHOULD NOT BE HAPPEND HERE, seqnid:%@", [NSString stringWithUTF8String:seqnid.c_str()]);
            //int64 seqn = GetLocalSeqnFromId(seqnid);
            //if (seqn>0)
            //    SyncGroupSeqn(seqnid, seqn, (pms::EMsgRole)EMsgRole_Rsender);
            //else
            //    assert(seqn>0);
        } else if (seqnid.length()==0) // this means userid
        {
            SyncSeqn(m_userSeqn, (pms::EMsgRole)EMsgRole_Rsender);
        }
    } else {
        NSLog(@"MsgClient::OnGroupSeqn error code is :%d!!!", code);
    }
}

void MsgClient::OnNotifyData(int code, const std::string& seqnid)
{
    if (code==0)
    {
        if (seqnid.length()>0)
        {
            int64 seqn = GetLocalSeqnFromId(seqnid);
            if (seqn>0)
                SyncGroupData(seqnid, seqn);
            else
                assert(seqn>0);
        } else if (seqnid.length()==0) // this means userid
        {
            NSLog(@"MsgClient::OnNotifyData notify user to sync data!!!");
            SyncData(m_userSeqn);
        }
    } else {
        NSLog(@"MsgClient::OnGroupNotify error code is :%d!!!", code);
    }
}

void MsgClient::OnMsgServerConnected()
{
    std::cout << "MsgClient::OnMsgServerConnected was called, sync msg once" << std::endl;
    MCSyncMsg();
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
