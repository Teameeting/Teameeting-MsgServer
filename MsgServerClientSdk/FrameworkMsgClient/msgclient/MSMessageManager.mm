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
    if ((nil==grpId)|| (nil==content)) return -101;
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
    if ((nil==grpId)||(nil==users)||(nil==content)) return -101;
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
    if ((nil==usrId)||(nil==content)) return -101;
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
    if ((nil==usrIds)||(nil==content)) return -101;
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
    if ((nil==grpId)||(nil==hostId)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyLive(msgid, [grpId UTF8String], [hostId UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                          hostId:(NSString*)hostId
                            cash:(NSString*)cash
                            cont:(NSString*)cont
                          cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId)||(nil==hostId)||(nil==cash)||(nil==cont)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyRedEnvelope(msgid, [grpId UTF8String], [hostId UTF8String], [cash UTF8String], [cont UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyBlacklistGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                       notifys:(NSArray*)notifys
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId)||(nil==userId)||(nil==notifys)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in notifys) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    int res = MsgClient::Instance().MCNotifyBlacklist(msgid, [grpId UTF8String], [userId UTF8String], vusers);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyForbiddenGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                       notifys:(NSArray*)notifys
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId)||(nil==userId)||(nil==notifys)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in notifys) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    int res = MsgClient::Instance().MCNotifyForbidden(msgid, [grpId UTF8String], [userId UTF8String], vusers);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifySettedMgrGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId)||(nil==userId)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    int res = MsgClient::Instance().MCNotifySettedMgr(msgid, [grpId UTF8String], [userId UTF8String], vusers);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

@end
