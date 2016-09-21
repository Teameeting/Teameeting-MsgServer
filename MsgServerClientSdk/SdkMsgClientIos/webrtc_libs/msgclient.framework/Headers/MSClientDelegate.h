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
 *  when server was connected this function will be called
 *
 *  params:
 */
-(void)OnMsgServerConnected;

/**
 *  when server was connecting this function will be called
 *
 *  params:
 */
-(void)OnMsgServerConnecting;

/**
 *  when server was disconnected this function will be called
 *
 *  params:
 */
-(void)OnMsgServerDisconnect;

/**
 *  when server connect failed this function will be called
 *
 *  params:
 */
-(void)OnMsgServerConnectionFailure;

/**
 * when client initializing this function will be called
 *
 *  params:
 */
-(void)OnMsgClientInitializing;

/**
 *  when client initialize ok this function will be called
 *
 *  params:
 */
-(void)OnMsgClientInitialized;

/**
 *  when client un initialize ok this function will be called
 *
 *  params:
 */
-(void)OnMsgClientUnInitialize;

/**
 *  when client initialize failed this function will be called
 *
 *  params:
 */
-(void)OnMsgClientInitializeFailure;

@end
#endif /* MSClientDelegate_h */
