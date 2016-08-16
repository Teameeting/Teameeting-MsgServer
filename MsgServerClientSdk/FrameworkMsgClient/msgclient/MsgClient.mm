//
//  MsgClient.mm
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclient/MsgClient.h"
#import "msgclient/MSTxtMessage.h"
#import "msgclient/proto_ios/CommonMsg.pbobjc.h"
#import "msgclient/proto_ios/EntityMsg.pbobjc.h"
#import "msgclient/proto_ios/EntityMsgType.pbobjc.h"



#import "msgclient/MSMsgUtil.h"

int MsgClient::MCInit(const std::string& uid, const std::string& token, const std::string& nname)
{
    std::cout << "MCInit add module here, please implement it!!!" << std::endl;
    m_strUserId = uid;
    m_strToken = token;
    m_strNname = nname;
    m_nsUserId = [[NSString alloc] initWithUTF8String:uid.c_str()];
    m_nsToken = [[NSString alloc] initWithUTF8String:token.c_str()];
    m_nsNname = [[NSString alloc] initWithUTF8String:nname.c_str()];
    
    m_isFetched = false;
    m_nsGroupInfo = [[NSMutableArray alloc] init];
    m_recurLock = [[NSRecursiveLock alloc] init];
    m_sqlite3Manager = [[MSSqlite3Manager alloc] init];
    [m_sqlite3Manager initManager];
    CheckUserOrInit(m_nsUserId);
    GetLocalSeqnsFromDb();
    SyncSeqnFromDb2Core();
    return Init(uid, token, nname, pms::EModuleType(EModuleType_Tlive));
}

int MsgClient::MCUnin()
{
    PutLocalSeqnsToDb();
    if (m_sqlite3Manager)
    {
        [m_sqlite3Manager uninManager];
    }
    m_isFetched = false;
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
    if (m_isFetched)
    {
        NSLog(@"MCSyncMsg sync all seqns");
        SyncAllSeqns();
    } else {
        NSLog(@"MCSyncMsg should be fetched before called");
        FetchAllSeqns();
    }
    return 0;
}

int MsgClient::MCSync2Db()
{
    PutLocalSeqnsToDb();
    return 0;
}

/////////////////////////////////////////////////
//////////////////Send Message///////////////////
/////////////////////////////////////////////////

int MsgClient::MCSendTxtMsg(std::string& outmsgid, const std::string& groupid, const std::string& content)
{
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithGroupId:[NSString stringWithUTF8String:groupid.c_str()] content:[NSString stringWithUTF8String:content.c_str()] msgType:MCMsgTypeTtxt];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsg JSONToNSString error");
        return -3;
    }
    return SndMsg(outmsgid, groupid, "roomname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fgroup);
}


int MsgClient::MCSendTxtMsgTos(std::string& outmsgid, const std::string& groupid, const std::vector<std::string>& vusrs, const std::string& content)
{
    if (vusrs.size()==0) return -4;
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithGroupId:[NSString stringWithUTF8String:groupid.c_str()] content:[NSString stringWithUTF8String:content.c_str()] msgType:MCMsgTypeTtxt];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgTos JSONToNSString error");
        return -3;
    }
    return SndMsgTo(outmsgid, groupid, "roomname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fmulti, vusrs);
}

int MsgClient::MCSendTxtMsgToUsr(std::string& outmsgid, const std::string& userid, const std::string& content)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    
    MSMessage *txtMsg = (MSMessage*)[MSMsgUtil EncodeMessageWithUserId:[NSString stringWithUTF8String:userid.c_str()] content:[NSString stringWithUTF8String:content.c_str()] msgType:MCMsgTypeTtxt];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgToUsr JSONToNSString error");
        return -3;
    }
    
    return SndMsgTo(outmsgid, "groupid", "grpname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fsingle, uvec);
}

int MsgClient::MCSendTxtMsgToUsrs(std::string& outmsgid, const std::vector<std::string>& vusrs, const std::string& content)
{
    if (vusrs.size()==0) return -4;
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithUserIds:nil content:[NSString stringWithUTF8String:content.c_str()] msgType:MCMsgTypeTtxt];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgToUsrs JSONToNSString error");
        return -3;
    }
    
    return SndMsgTo(outmsgid, "groupid", "grpname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Ttxt, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fmulti, vusrs);
}


int MsgClient::MCNotifyLive(std::string& outmsgid, const std::string& groupid, const std::string& hostid, int flag)
{
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithGroupId:[NSString stringWithUTF8String:groupid.c_str()] userId:[NSString stringWithUTF8String:hostid.c_str()] flag:flag msgType:MCMsgTypeTliv];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgToUsr JSONToNSString error");
        return -3;
    }

    return SndMsg(outmsgid, groupid, "grpname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tliv, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fgroup);
}

int MsgClient::MCNotifyRedEnvelope(std::string& outmsgid, const std::string& groupid, const std::string& hostid, const std::string& cash, const std::string& cont)
{
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithGroupId:[NSString stringWithUTF8String:groupid.c_str()] userId:[NSString stringWithUTF8String:hostid.c_str()] cash:[NSString stringWithUTF8String:cash.c_str()] cont:[NSString stringWithUTF8String:cont.c_str()] msgType:MCMsgTypeTren];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgToUsr JSONToNSString error");
        return -3;
    }

    return SndMsg(outmsgid, groupid, "grpname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tren, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fgroup);
}

int MsgClient::MCNotifyBlacklist(std::string& outmsgid, const std::string& groupid, const std::string& userid, int flag, const std::vector<std::string>& notifys)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    for (auto &it : notifys) {
        uvec.push_back(it);
    }
    
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithGroupId:[NSString stringWithUTF8String:groupid.c_str()] userId:[NSString stringWithUTF8String:userid.c_str()] flag:flag msgType:MCMsgTypeTblk];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgToUsr JSONToNSString error");
        return -3;
    }

    return SndMsgTo(outmsgid, groupid, "grpname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tblk, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fmulti, uvec);
}

int MsgClient::MCNotifyForbidden(std::string& outmsgid, const std::string& groupid, const std::string& userid, int flag, const std::vector<std::string>& notifys)
{
    std::vector<std::string> uvec;
    uvec.push_back(userid);
    for (auto &it : notifys) {
        uvec.push_back(it);
    }
    
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithGroupId:[NSString stringWithUTF8String:groupid.c_str()] userId:[NSString stringWithUTF8String:userid.c_str()] flag:flag msgType:MCMsgTypeTfbd];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgToUsr JSONToNSString error");
        return -3;
    }

    return SndMsgTo(outmsgid, groupid, "grpname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tfbd, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fmulti, uvec);
}

int MsgClient::MCNotifySettedMgr(std::string& outmsgid, const std::string& groupid, const std::string& userid, int flag, const std::vector<std::string>& notifys)
{
    MSTxtMessage *txtMsg = [MSMsgUtil EncodeMessageWithGroupId:[NSString stringWithUTF8String:groupid.c_str()] userId:[NSString stringWithUTF8String:userid.c_str()] flag:flag msgType:MCMsgTypeTmgr];
    NSString *jsonString = [MSMsgUtil JSONToNSString:[txtMsg mj_keyValues]];
    
    if (!jsonString)
    {
        NSLog(@"MCSendTxtMsgToUsr JSONToNSString error");
        return -3;
    }

    return SndMsg(outmsgid, groupid, "grpname", [jsonString cStringUsingEncoding:NSUTF8StringEncoding], (pms::EMsgTag)EMsgTag_Tchat, (pms::EMsgType)EMsgType_Tmgr, (pms::EModuleType)EModuleType_Tlive, (pms::EMsgFlag)EMsgFlag_Fgroup);
}

/////////////////////////////////////////////////
//////////Callback From XMsgCallback/////////////
/////////////////////////////////////////////////

void MsgClient::OnSndMsg(int code, const std::string& msgid)
{
    std::cout << "MsgClient::OnSndMsg msgid:" << msgid << ", code:" << code << std::endl;
    [m_txtMsgDelegate OnSendMessageId:[NSString stringWithCString:msgid.c_str() encoding:NSASCIIStringEncoding] code:code];
}

void MsgClient::OnCmdCallback(int code, int cmd, const std::string& groupid, const MSCbData& data)
{
    NSLog(@"MsgClient::OnCmdCallback cmd:%d, groupid.length:%lu, result:%d, seqn:%lld", cmd, groupid.length(), data.result, data.seqn);
    switch (cmd)
    {
        case MSADD_GROUP:
        {
            if (code == 0)
            {
                NSString *nsGrpId = [NSString stringWithCString:groupid.c_str() encoding:NSUTF8StringEncoding];
                NSLog(@"OnCmdCallback add group ok, insert groupid and seqn, toggle callback");
                if (![m_sqlite3Manager isGroupExistsUserId:m_nsUserId GrpId:nsGrpId])
                {
                    [m_sqlite3Manager addGroupIdUserId:m_nsUserId GrpId:nsGrpId];
                }
                if (![m_sqlite3Manager isUserExistsUserId:m_nsUserId seqnId:nsGrpId])
                {
                    [m_sqlite3Manager addGroupSeqnUserId:m_nsUserId GrpId:nsGrpId seqn:[NSNumber numberWithLongLong:data.seqn]];
                }
                UpdateLocalSeqn(groupid, data.seqn);
                UpdateSeqnFromDb2Core();
                [m_groupDelegate OnAddGroupSuccessGrpId:nsGrpId];
            } else if (code == -1)
            {
                [m_groupDelegate OnAddGroupFailedGrpId:[[NSString alloc] initWithUTF8String:groupid.c_str()] reason:[[NSString alloc] initWithUTF8String:data.data.c_str()] code:code];
            }
        }
            break;
        case MSRMV_GROUP:
        {
            if (code == 0)
            {
                NSString *nsGrpId = [NSString stringWithCString:groupid.c_str() encoding:NSUTF8StringEncoding];
                NSLog(@"OnCmdCallback del group ok, del groupid and seqn, toggle callback");
                [m_sqlite3Manager delGroupIdUserId:m_nsUserId GrpId:nsGrpId];
                RemoveLocalSeqn(groupid);
                //Update seqn from db 2 core
                [m_groupDelegate OnRmvGroupSuccessGrpId:nsGrpId];
            } else if (code == -1)
            {
                [m_groupDelegate OnRmvGroupFailedGrpId:[[NSString alloc] initWithUTF8String:groupid.c_str()] reason:[[NSString alloc] initWithUTF8String:data.data.c_str()] code:code];
            }
        }
            break;
        case MSFETCH_SEQN:
        {
            if (groupid.length()==0) // for user
            {
                if (data.result==0)
                {
                    UpdateGroupInfoToDb(m_nsUserId, [NSNumber numberWithLongLong:data.seqn], [NSNumber numberWithInt:1]);
                } else if (data.result==-1)
                {
                    UpdateGroupInfoToDb(m_nsUserId, [NSNumber numberWithLongLong:data.seqn], [NSNumber numberWithInt:-1]);
                } else if (data.result==-2)
                {
                    UpdateGroupInfoToDb(m_nsUserId, [NSNumber numberWithLongLong:data.seqn], [NSNumber numberWithInt:-2]);
                }
            } else { // for group
                if (data.result==0)
                {
                    UpdateGroupInfoToDb([NSString stringWithCString:groupid.c_str() encoding:NSUTF8StringEncoding], [NSNumber numberWithLongLong:data.seqn], [NSNumber numberWithInt:1]);
                    UpdateSeqnFromDb2Core();
                } else if (data.result==-1)
                {
                    UpdateGroupInfoToDb([NSString stringWithCString:groupid.c_str() encoding:NSUTF8StringEncoding], [NSNumber numberWithLongLong:data.seqn], [NSNumber numberWithInt:-1]);
                    //Update seqn from db 2 core
                } else if (data.result==-2)
                {
                    UpdateGroupInfoToDb([NSString stringWithCString:groupid.c_str() encoding:NSUTF8StringEncoding], [NSNumber numberWithLongLong:data.seqn], [NSNumber numberWithInt:-2]);
                    //Update seqn from db 2 core
                }
            }
        }
            break;
        default:
            break;
    }
}


void MsgClient::OnRecvMsg(int64 seqn, const std::string& msg)
{
    std::cout << "MsgClient::OnRecvMsg was called, seqn:" << seqn << ", msg.length:" << msg.length() << std::endl;
    
    UpdateLocalSeqn(m_strUserId, seqn);
    UpdateSeqnToDb(m_strUserId, seqn);
    
    NSError *error = nil;
    Byte *b = (Byte*)malloc(msg.length());
    memset(b, 0x00, msg.length());
    memcpy(b, msg.c_str(), msg.length());
    NSData *ns_dtbyte = [NSData dataWithBytes:b length:msg.length()];
    Entity *entityByte = [Entity parseFromData:ns_dtbyte error:&error];
    if (entityByte == nil || error != nil) {
        NSLog(@"eeeeeeeeeee-->entityByte is nil");
        if (error) {
            NSLog(@"eeeee Entity parseFromData has error:%@", error);
        }
        if (b) free(b);
        b = nil;
        return;
    }
    
    if ([[entityByte usrFrom] compare:m_nsUserId]==0) {
        NSLog(@"MsgClient::OnRecvMsg recv the msg you just send!!!, so return");
        if (b) free(b);
        b = nil;
        return;
    }
    NSLog(@"MsgClient::OnRecvMsg entityByte tag:%d, cont:%@, romid:%@, usr_from:%@", [entityByte msgTag], [entityByte msgCont], [entityByte romId], [entityByte usrFrom]);
    
    MSTxtMessage *txtMsg = [MSMsgUtil DecodeDictToMessageWithDict:[MSMsgUtil NSStringToJSONWithString:[entityByte msgCont]]];
    NSDate *dateNow = [NSDate dateWithTimeIntervalSince1970:[entityByte msgTime]];
    [txtMsg setMillSec:[entityByte msgTime]];
    [txtMsg setMsgId:[entityByte cmsgId]];
    NSLog(@"MsgClient::OnRecvMsg dateNow:%@, msg_time:%u"\
          , dateNow, [entityByte msgTime]);
    
    switch ([entityByte msgType])
    {
        case pms::EMsgType::TTXT:
        {
            [m_txtMsgDelegate OnRecvTxtMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TFIL:
        {
        }
            break;
        case pms::EMsgType::TPIC:
        {
        }
            break;
        case pms::EMsgType::TAUD:
        {
        }
            break;
        case pms::EMsgType::TVID:
        {
        }
            break;
        case pms::EMsgType::TEMJ:
        {
        }
            break;
        case pms::EMsgType::TSDF:
        {
            [m_txtMsgDelegate OnRecvSelfDefMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TLIV:
        {
        }
            break;
        case pms::EMsgType::TREN:
        {
        }
            break;
        case pms::EMsgType::TBLK:
        {
            [m_txtMsgDelegate OnNotifyBlacklistMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TFBD:
        {
            [m_txtMsgDelegate OnNotifyForbiddenMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TMGR:
        {
        }
            break;
        default:
        {
        }
            break;
    }
    if (b) free(b);
    b = nil;
}

void MsgClient::OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg)
{
    std::cout << "MsgClient::OnRecvGroupMsg was called, seqn:" << seqn << ", seqnid:" << seqnid << ", msg.length:" << msg.length() << std::endl;
    
    UpdateLocalSeqn(seqnid, seqn);
    UpdateSeqnToDb(seqnid, seqn);
    
    NSError *error = nil;
    Byte *b = (Byte*)malloc(msg.length());
    memset(b, 0x00, msg.length());
    memcpy(b, msg.c_str(), msg.length());
    
    NSData *ns_dtbyte = [NSData dataWithBytes:b length:msg.length()];
    Entity *entityByte = [Entity parseFromData:ns_dtbyte error:&error];
    if (entityByte == nil || error != nil) {
        NSLog(@"eeeeeeeeeee-->entityByte is nil");
        if (error) {
            NSLog(@"eeeee Entity parseFromData has error:%@", error);
        }
        if (b) free(b);
        b = nil;
        return;
    }
    
    if ([[entityByte usrFrom] compare:m_nsUserId]==0) {
        NSLog(@"MsgClient::OnRecvGroupMsg recv the msg you just send!!!, so return");
        if (b) free(b);
        b = nil;
        return;
    }
    NSLog(@"MsgClient::OnRecvGroupMsg entityByte tag:%d, cont:%@, romid:%@, usr_from:%@", [entityByte msgTag], [entityByte msgCont], [entityByte romId], [entityByte usrFrom]);
    
    MSTxtMessage *txtMsg = [MSMsgUtil DecodeDictToMessageWithDict:[MSMsgUtil NSStringToJSONWithString:[entityByte msgCont]]];
    NSDate *dateNow = [NSDate dateWithTimeIntervalSince1970:[entityByte msgTime]];
    [txtMsg setMillSec:[entityByte msgTime]];
    [txtMsg setMsgId:[entityByte cmsgId]];
    NSLog(@"MsgClient::OnRecvGroupMsg dateNow:%@, msg_time:%u"\
          , dateNow, [entityByte msgTime]);
    
    switch ([entityByte msgType])
    {
        case pms::EMsgType::TTXT:
        {
            [m_txtMsgDelegate OnRecvTxtMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TFIL:
        {
        }
            break;
        case pms::EMsgType::TPIC:
        {
        }
            break;
        case pms::EMsgType::TAUD:
        {
        }
            break;
        case pms::EMsgType::TVID:
        {
        }
            break;
        case pms::EMsgType::TEMJ:
        {
        }
            break;
        case pms::EMsgType::TSDF:
        {
            [m_txtMsgDelegate OnRecvSelfDefMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TLIV:
        {
            [m_txtMsgDelegate OnNotifyLiveMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TREN:
        {
            [m_txtMsgDelegate OnNotifyRedEnvelopeMessage:txtMsg];
        }
            break;
        case pms::EMsgType::TBLK:
        {
        }
            break;
        case pms::EMsgType::TFBD:
        {
        }
            break;
        case pms::EMsgType::TMGR:
        {
            [m_txtMsgDelegate OnNotifySettedMgrMessage:txtMsg];
        }
            break;
        default:
        {
        }
            break;
    }
    if (b) free(b);
    b = nil;
}


//////////////Below Used in this class//////////////////////////
//////////////Later it will be removed//////////////////////////

void MsgClient::OnSyncSeqn(int64 maxseqn, int role)
{
    // if it is sender, this means client send msg, and just get the newmsg's seqn
    // if the new seqn is bigger 1 than cur seqn, it is ok, just update seqn.
    // if the new seqn is bigger 2 or 3 than cur seqn, this need sync data
    // if it is recver, this means client need sync data
    NSLog(@"MsgClient::OnSyncSeqn NOT IMPLEMENT!!!");
}

// get group max seqn
void MsgClient::OnSyncGroupSeqn(const std::string &groupid, int64 maxseqn)
{
    NSLog(@"MsgClient::OnSyncGroupSeqn NOT IMPLEMENT!!!");
}

// not implement
void MsgClient::OnGroupNotify(int code, const std::string& seqnid)
{
    NSLog(@"MsgClient::OnGroupNotify NOT IMPLEMENT!!!");
}

void MsgClient::OnNotifySeqn(int code, const std::string& seqnid)
{
    NSLog(@"MsgClient::OnNotifySeqn NOT IMPLEMENT!!!");
}

void MsgClient::OnNotifyData(int code, const std::string& seqnid)
{
    NSLog(@"MsgClient::OnNotifyData NOT IMPLEMENT!!!");
}

//////////////Above Used in this class//////////////////////////

void MsgClient::OnMsgServerConnected()
{
    std::cout << "MsgClient::OnMsgServerConnected was called, fetchallseqn once" << std::endl;
    FetchAllSeqns();
    [m_clientDelegate OnMsgServerConnected];
    [m_clientDelegate OnMsgClientInitializing];
    
    // check per second until get all fetch
    NSTimeInterval period = 1.0; //设置时间间隔
    //dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_source_t _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
    dispatch_source_set_timer(_timer, dispatch_walltime(NULL, 0), period * NSEC_PER_SEC, 0); //每秒执行
    dispatch_source_set_event_handler(_timer, ^{
        NSLog(@"MsgClient::OnMsgServerConnected timer was called...............");
        //在这里执行事件
        if (IsFetchedAll())
        {
            // stop timer here
            m_isFetched = true;
            UpdateSeqnFromDb2Core();
            SyncAllSeqns();
            [m_clientDelegate OnMsgClientInitialized];
            dispatch_source_set_cancel_handler(_timer, ^{
               
                NSLog(@"MsgClient::OnMsgServerConnected dispatch_source_cancel...ok");
            });
            dispatch_source_cancel(_timer);
        } else {
            FetchAllSeqns();
        }
    });
    dispatch_resume(_timer);
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
