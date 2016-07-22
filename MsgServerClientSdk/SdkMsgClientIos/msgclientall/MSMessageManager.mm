//
//  MSMessageManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclientall/MSMessageManager.h"
#import "msgclientall/MsgClient.h"

@implementation MSMessageManager

-(void)addDelegateId:(id<MSTxtMessageDelegate>)txtDelegate
       delegateQueue:(dispatch_queue_t)txtQueue
{
    MsgClient::Instance().MCSetTxtMsgDelegate(txtDelegate);
}

-(void)delDelegateId:(id<MSTxtMessageDelegate>)txtDelegate
{
    MsgClient::Instance().MCSetTxtMsgDelegate(nullptr);
}

-(void)syncMsg
{
    MsgClient::Instance().MCSyncMsg();
}

-(int)sendTxtMsgGrpId:(NSString*)grpId
                 cont:(NSString*)content
               cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCSendTxtMsg(msgid, [grpId UTF8String], [content UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendTxtMsgTosGrpId:(NSString*)grpId
                   users:(NSArray*)users
                    cont:(NSString*)content
                  cmsgid:(NSString**)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in users) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    int res = MsgClient::Instance().MCSendTxtMsgTos(msgid, [grpId UTF8String], vusers, [content UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendTxtMsgToUsrId:(NSString*)usrId
                   cont:(NSString*)content
                 cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCSendTxtMsgToUsr(msgid, [usrId UTF8String], [content UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendTxtMsgToUsrIds:(NSArray*)usrIds
                    cont:(NSString*)content
                  cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in usrIds) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    int res = MsgClient::Instance().MCSendTxtMsgToUsrs(msgid, vusers, [content UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyLiveGrpId:(NSString*)grpId
                   hostId:(NSString*)hostId
                   cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyLive(msgid, [grpId UTF8String], [hostId UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                          hostId:(NSString*)hostId
                          cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyRedEnvelope(msgid, [grpId UTF8String], [hostId UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyBlacklistGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyBlacklist(msgid, [grpId UTF8String], [userId UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyForbiddenGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyForbidden(msgid, [grpId UTF8String], [userId UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifySettedMgrGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifySettedMgr(msgid, [grpId UTF8String], [userId UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

@end