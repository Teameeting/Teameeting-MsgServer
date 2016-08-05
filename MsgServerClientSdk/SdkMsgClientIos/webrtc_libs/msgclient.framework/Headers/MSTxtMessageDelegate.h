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
 *  when the msg you send reached server this function will be called
 *
 *  params:
 *      msgId: the message you just send, this 'msgId' is got by invoke methods in MSMessageManager.h
 *      code: the code, usually it is 0
 */
-(void)OnSendMessageId:(NSString*)msgId
                  code:(int)code;

/**
 *  when recv text msg this function will be called
 *  in each received msg, 'nickname', 'uiconurl', 'fromid', 'msgType' will be all setted
 *  besides, different msg will has different valid field
 *
 *  params:
 *      txtMsg: the message you received
 *
 *  'groupid' and 'content' will be setted
 *  'groupid' means this message belong which group
 *  'content' means the message content
 */
-(void)OnRecvTxtMessage:(MSTxtMessage*)txtMsg;

/**
 *  when recv self define msg this function will be called, not used now
 *  in each received msg, 'nickname', 'uiconurl', 'fromid', 'msgType' will be all setted
 *  besides, different msg will has different valid field
 *
 *  params:
 *      sdefMsg: the message you received
 */
-(void)OnRecvSelfDefMessage:(MSTxtMessage*)sdefMsg;

/**
 *  when hostId begin to live in grpId this function will be called
 *  in each received msg, 'nickname', 'uiconurl', 'fromid', 'msgType' will be all setted
 *  besides, different msg will has different valid field
 *
 *  params:
 *      livMsg: the message you received
 *
 *  'groupid' will be setted
 *  'groupid' means this message belong which group
 */
-(void)OnNotifyLiveMessage:(MSTxtMessage*)livMsg;

/**
 *  when hostId in grpId recv red-envelope this function will be called
 *  in each received msg, 'nickname', 'uiconurl', 'fromid', 'msgType' will be all setted
 *  besides, different msg will has different valid field
 *
 *  params:
 *      renMsg: the message you received
 *
 *  'groupid' and 'userid' and 'cash' and 'wishcont' will be setted
 *  'groupid' means this message belong which group
 *  'userid' means this red envelope is send to
 *  'cash" means the cash amount
 *  'wishcont' means the wish content
 */
-(void)OnNotifyRedEnvelopeMessage:(MSTxtMessage*)renMsg;

/**
 *  when userId was push to blacklist in grpId this function will be called
 *  in each received msg, 'nickname', 'uiconurl', 'fromid', 'msgType' will be all setted
 *  besides, different msg will has different valid field
 *
 *  params:
 *      blkMsg: the message you received
 *
 *  'groupid' and 'userid' will be setted
 *  'groupid' means this message belong which group
 *  'userid' means who will be pull blacklist, if userid equals your id, means you are pulled to blacklist
 *  if userid not equals your id, means you are notified to know
 */
-(void)OnNotifyBlacklistMessage:(MSTxtMessage*)blkMsg;

/**
 *  when userId was forbidden in grpId this function will be called
 *  in each received msg, 'nickname', 'uiconurl', 'fromid', 'msgType' will be all setted
 *  besides, different msg will has different valid field
 *
 *  params:
 *      fbdMsg: the message you received
 *
 *  'groupid' and 'userid'will be setted
 *  'groupid' means this message belong which group
 *  'userid' means who will be forbidden to talk, if userid equals your id, means you are forbidden to talk
 *  if userid not equals your id, means you are notified to know
 */
-(void)OnNotifyForbiddenMessage:(MSTxtMessage*)fbdMsg;

/**
 *  when userId in grpId was setted to be manager this function will be called
 *  in each received msg, 'nickname', 'uiconurl', 'fromid', 'msgType' will be all setted
 *  besides, different msg will has different valid field
 *
 *  params:
 *      mgrMsg: the message you received
 *
 *  'groupid' and 'userid' will be setted
 *  'groupid' means this message belong which group
 *  'userid' means who will be setted to be manager, if userid equals your id, means you are setted to be manager
 *  if userid not equals your id, means you are notified to know
 */
-(void)OnNotifySettedMgrMessage:(MSTxtMessage*)mgrMsg;

@end

#endif /* MSTxtMessageDelegate_h */
