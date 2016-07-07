//
//  MSClientDelegate.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSClientDelegate_h
#define MSClientDelegate_h


@protocol MSClientDelegate <NSObject>

@required

/**
 *  when server was connected
 *
 *  params:
 */
-(void)OnMsgServerConnected;

/**
 *  when server was connecting
 *
 *  params:
 */
-(void)OnMsgServerConnecting;

/**
 *  when server was disconnected
 *
 *  params:
 */
-(void)OnMsgServerDisconnect;

/**
 *  when server connect failed
 *
 *  params:
 */
-(void)OnMsgServerConnectionFailure;

/**
 * when client initializing
 *
 *  params:
 */
-(void)OnMsgClientInitializing;

/**
 *  when client initialize ok
 *
 *  params:
 */
-(void)OnMsgClientInitialized;

/**
 *  when client un initialize
 *
 *  params:
 */
-(void)OnMsgClientUnInitialize;

/**
 *  when client initialize failed
 *
 *  params:
 */
-(void)OnMsgClientInitializeFailure;

@end
#endif /* MSClientDelegate_h */
