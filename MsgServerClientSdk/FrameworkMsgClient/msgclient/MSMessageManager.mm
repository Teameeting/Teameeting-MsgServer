//
//  MSMessageManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclient/MSMessageManager.h"
#import "msgclient/MsgClient.h"

@implementation MSMessageManager

-(void)addDelegateId:(id<MSSubMessageDelegate>)subDelegate
       delegateQueue:(dispatch_queue_t)subQueue
{
    MsgClient::Instance().MCSetTxtMsgDelegate(subDelegate);
}

-(void)delDelegateId:(id<MSSubMessageDelegate>)subDelegate
{
    MsgClient::Instance().MCSetTxtMsgDelegate(nullptr);
}

-(void)syncMsg
{
    MsgClient::Instance().MCSyncMsg();
}

-(void)sync2Db
{
    MsgClient::Instance().MCSync2Db();
}

-(NSString*)sendTxtMsg:(MSTxtMessage*)txtMsg
{
    if (nil==txtMsg)
    {
        NSLog(@"sendTxtMsg txtMsg is null");
        return nil;
    }
    NSLog(@" send grpid:%@, content:%@", [txtMsg getGroupId], [txtMsg getContent]);
    if ((nil==[txtMsg getGroupId] || 0==[[txtMsg getGroupId] length]) || (nil==[txtMsg getContent] || 0==[[txtMsg getContent] length]))
    {
        NSLog(@"sendTxtMsg params is null");
        return nil;
    }
    std::string msgid("");
    MsgClient::Instance().MCSendTxtMsg(msgid, txtMsg);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendTxtMsgTos:(MSTxtMessage*)txtMsg
                    users:(NSArray*)users
{
    if (nil==txtMsg) return nil;
    if ((nil==[txtMsg getGroupId] || 0==[[txtMsg getGroupId] length])||(nil==[txtMsg getContent] || [[txtMsg getContent] length]==0)||(nil==users || [users count]==0)) return nil;
    std::string msgid("");
    std::vector<std::string> vusers;
    for (NSString* name in users) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    MsgClient::Instance().MCSendTxtMsgTos(msgid, txtMsg, vusers);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendTxtMsgToUser:(MSTxtMessage*)txtMsg
{
    if (nil==txtMsg) return nil;
    if ((nil==[txtMsg getToId] || 0==[[txtMsg getToId] length])||(nil==[txtMsg getContent] || [[txtMsg getContent] length]==0)) return nil;
    std::string msgid("");
    MsgClient::Instance().MCSendTxtMsgToUsr(msgid, txtMsg);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendTxtMsgToUsers:(MSTxtMessage*)txtMsg
                        users:(NSArray*)users
{
    if (nil==txtMsg) return nil;
    if ((nil==[txtMsg getContent] || 0==[[txtMsg getContent] length])||(nil==users || 0==[users count])) return nil;
    std::string msgid("");
    std::vector<std::string> vusers;
    for (NSString* name in users) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    MsgClient::Instance().MCSendTxtMsgToUsrs(msgid, txtMsg, vusers);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendNotifyLive:(MSLivMessage*)livMsg
{
    if (nil==livMsg) return nil;
    if ((nil==[livMsg getGroupId] || 0==[[livMsg getGroupId] length])||(nil==[livMsg getToId] || 0==[[livMsg getToId] length])) return nil;
    std::string msgid("");
    MsgClient::Instance().MCNotifyLive(msgid, livMsg);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendNotifyRedEnvelope:(MSRenMessage*)renMsg
{
    if (nil==renMsg) return nil;
    if ((nil==[renMsg getGroupId] || 0==[[renMsg getGroupId] length])
        ||(nil==[renMsg getToId] || 0==[[renMsg getToId] length])
        ||(nil==[renMsg getCash] || 0==[[renMsg getCash] length])
        ||(nil==[renMsg getWishcont] || 0==[[renMsg getWishcont] length])) return nil;
    std::string msgid("");
    MsgClient::Instance().MCNotifyRedEnvelope(msgid, renMsg);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendNotifyBlacklist:(MSBlkMessage*)blkMsg
                        notifys:(NSArray*)notifys
{
    if (nil==blkMsg) return nil;
    if ((nil==[blkMsg getGroupId] || 0==[[blkMsg getGroupId] length])
        ||(nil==[blkMsg getToId] || 0==[[blkMsg getToId] length])
        ||(nil==notifys || 0==[notifys count])) return nil;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in notifys) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    MsgClient::Instance().MCNotifyBlacklist(msgid, blkMsg, vusers);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendNotifyForbidden:(MSFbdMessage*)fbdMsg
                        notifys:(NSArray*)notifys
{
    if (nil==fbdMsg) return nil;
    if ((nil==[fbdMsg getGroupId] || 0==[[fbdMsg getGroupId] length])
        ||(nil==[fbdMsg getToId] || 0==[[fbdMsg getToId] length])
        ||(nil==notifys || [notifys count]==0)) return nil;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in notifys) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    MsgClient::Instance().MCNotifyForbidden(msgid, fbdMsg, vusers);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

-(NSString*)sendNotifySettedMgr:(MSMgrMessage*)mgrMsg
{
    if (nil==mgrMsg) return nil;
    if ((nil==[mgrMsg getGroupId] || 0==[[mgrMsg getGroupId] length])||(nil==[mgrMsg getToId] || 0==[[mgrMsg getToId] length])) return nil;
    std::string msgid;
    std::vector<std::string> vusers;
    MsgClient::Instance().MCNotifySettedMgr(msgid, mgrMsg, vusers);
    return [NSString stringWithUTF8String:msgid.c_str()];
}

@end