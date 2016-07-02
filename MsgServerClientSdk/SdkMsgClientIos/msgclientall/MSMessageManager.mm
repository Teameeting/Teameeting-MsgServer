//
//  MSMessageManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSMessageManager.h"
#import "MsgClient.h"

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

-(void)sendTxtMsgGrpId:(NSString*)grpId
                  cont:(NSString*)content
{
    MsgClient::Instance().MCSendTxtMsg([grpId UTF8String], [content UTF8String]);
}

-(void)sendTxtMsgToUsrId:(NSString*)usrId
                    cont:(NSString*)content
{
    MsgClient::Instance().MCSendTxtMsgToUsr([usrId UTF8String], [content UTF8String]);
}

-(void)sendTxtMsgToUsrIds:(NSArray*)usrIds
                     cont:(NSString*)content
{
    std::vector<std::string> vusers;
    MsgClient::Instance().MCSendTxtMsgToUsrs(vusers, [content UTF8String]);
}

-(void)sendNotifyLiveGrpId:(NSString*)grpId
                    hostId:(NSString*)hostId
{
    MsgClient::Instance().MCNotifyLive([grpId UTF8String], [hostId UTF8String]);
}

-(void)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                           hostId:(NSString*)hostId
{
    MsgClient::Instance().MCNotifyRedEnvelope([grpId UTF8String], [hostId UTF8String]);
}

-(void)sendNotifyBlacklistGrpId:(NSString*)grpId
                         userId:(NSString*)userId
{
    MsgClient::Instance().MCNotifyBlacklist([grpId UTF8String], [userId UTF8String]);
}

-(void)sendNotifyForbiddenGrpId:(NSString*)grpId
                         userId:(NSString*)userId
{
    MsgClient::Instance().MCNotifyForbidden([grpId UTF8String], [userId UTF8String]);
}

-(void)sendNotifySettedMgrGrpId:(NSString*)grpId
                         userId:(NSString*)userId
{
    MsgClient::Instance().MCNotifySettedMgr([grpId UTF8String], [userId UTF8String]);
}

@end