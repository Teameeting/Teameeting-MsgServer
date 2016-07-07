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
// when server was connected
-(void)OnMsgServerConnected;

// when server was connecting
-(void)OnMsgServerConnecting;

// when server was disconnected
-(void)OnMsgServerDisconnect;

// when server connect failed
-(void)OnMsgServerConnectionFailure;

// when client initializing
-(void)OnMsgClientInitializing;

// when client initialize ok
-(void)OnMsgClientInitialized;

// when client un initialize
-(void)OnMsgClientUnInitialize;

// when client initialize failed
-(void)OnMsgClientInitializeFailure;

@end
#endif /* MSClientDelegate_h */
