//
//  MSTxtMessageDelegate.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSTxtMessageDelegate_h
#define MSTxtMessageDelegate_h
#import "msgclientall/MSTxtMessage.h"

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
-(void)OnNotifyLiveMessage:(MSTxtMessage*)livMsg;

/**
 *  when hostId in grpId recv red-envelope
 *
 *  params:
 */
-(void)OnNotifyRedEnvelopeMessage:(MSTxtMessage*)renMsg;

/**
 *  when userId was push to blacklist in grpId
 *
 *  params:
 */
-(void)OnNotifyBlacklistMessage:(MSTxtMessage*)blkMsg;

/**
 *  when userId was forbidden in grpId
 *
 *  params:
 */
-(void)OnNotifyForbiddenMessage:(MSTxtMessage*)fbdMsg;

/**
 *  when userId in grpId was setted to be manager
 *
 *  params:
 */
-(void)OnNotifySettedMgrMessage:(MSTxtMessage*)mgrMsg;

@end

#endif /* MSTxtMessageDelegate_h */
