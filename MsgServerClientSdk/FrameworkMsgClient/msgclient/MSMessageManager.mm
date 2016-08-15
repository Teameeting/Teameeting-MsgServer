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

-(void)sync2Db
{
    MsgClient::Instance().MCSync2Db();
}

-(int)sendTxtMsgGrpId:(NSString*)grpId
                 cont:(NSString*)content
               cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId || [grpId length]==0)|| (nil==content || [content length]==0)) return -101;
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
    if ((nil==grpId || [grpId length]==0)||(nil==users || [users count]==0)||(nil==content || [content length]==0)) return -101;
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
    if ((nil==usrId || [usrId length]==0)||(nil==content || [content length]==0)) return -101;
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
    if ((nil==usrIds || [usrIds count]==0)||(nil==content || [content length]==0)) return -101;
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
                     flag:(int)flag
                   cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId || [grpId length]==0)||(nil==hostId || [hostId length]==0)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyLive(msgid, [grpId UTF8String], [hostId UTF8String], flag);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                          hostId:(NSString*)hostId
                            cash:(NSString*)cash
                            cont:(NSString*)cont
                          cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId || [grpId length]==0)||(nil==hostId || [hostId length]==0)||(nil==cash || [cash length]==0)||(nil==cont || [cont length]==0)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    int res = MsgClient::Instance().MCNotifyRedEnvelope(msgid, [grpId UTF8String], [hostId UTF8String], [cash UTF8String], [cont UTF8String]);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyBlacklistGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                          flag:(int)flag
                       notifys:(NSArray*)notifys
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId || [grpId length]==0)||(nil==userId || [userId length]==0)||(nil==notifys || [notifys count]==0)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in notifys) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    int res = MsgClient::Instance().MCNotifyBlacklist(msgid, [grpId UTF8String], [userId UTF8String], flag, vusers);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifyForbiddenGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                          flag:(int)flag
                       notifys:(NSArray*)notifys
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId || [grpId length]==0)||(nil==userId || [userId length]==0)||(nil==notifys || [notifys count]==0)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    for (NSString* name in notifys) {
        vusers.push_back([name cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    int res = MsgClient::Instance().MCNotifyForbidden(msgid, [grpId UTF8String], [userId UTF8String], flag, vusers);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

-(int)sendNotifySettedMgrGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                          flag:(int)flag
                        cmsgid:(NSString *__autoreleasing *)cmsgid
{
    if ((nil==grpId || [grpId length]==0)||(nil==userId || [userId length]==0)) return -101;
    if (!cmsgid) return -100;
    std::string msgid;
    std::vector<std::string> vusers;
    int res = MsgClient::Instance().MCNotifySettedMgr(msgid, [grpId UTF8String], [userId UTF8String], flag, vusers);
    *cmsgid = [NSString stringWithUTF8String:msgid.c_str()];
    return res;
}

@end
