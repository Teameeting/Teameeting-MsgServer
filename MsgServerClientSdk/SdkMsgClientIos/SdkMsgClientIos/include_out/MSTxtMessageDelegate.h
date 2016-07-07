//
//  MSTxtMessageDelegate.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSTxtMessageDelegate_h
#define MSTxtMessageDelegate_h
#import "MSTxtMessage.h"

@protocol MSTxtMessageDelegate <NSObject>

@optional

/**
 *  when the msg you send reached server
 *
 *  params:
 */
-(void)OnSendMessageId:(NSString*)msgId
                  code:(int)code;

/**
 *  when recv text msg
 *
 *  params:
 */
-(void)OnRecvTxtMessage:(MSTxtMessage*)txtMsg;

/**
 *  when recv self define msg, not used now
 *
 *  params:
 */
-(void)OnRecvSelfDefMessage:(MSTxtMessage*)sdefMsg;

/**
 *  when hostId begin to live in grpId
 *
 *  params:
 */
-(void)OnNotifyLiveId:(NSString*)grpId
               hostId:(NSString*)hostId;

/**
 *  when hostId in grpId recv red-envelope
 *
 *  params:
 */
-(void)OnNotifyRedEnvelopeGrpId:(NSString*)grpId
                         hostId:(NSString*)hostId;

/**
 *  when userId was push to blacklist in grpId
 *
 *  params:
 */
-(void)OnNotifyBlacklist:(NSString*)grpId
                  userId:(NSString*)userId;

/**
 *  when userId was forbidden in grpId
 *
 *  params:
 */
-(void)OnNotifyForbidden:(NSString*)grpId
                  userId:(NSString*)userId;

/**
 *  when userId in grpId was setted to be manager
 *
 *  params:
 */
-(void)OnNotifySettedMgrGrpId:(NSString*)grpId
                       userId:(NSString*)userId;

@end

#endif /* MSTxtMessageDelegate_h */
