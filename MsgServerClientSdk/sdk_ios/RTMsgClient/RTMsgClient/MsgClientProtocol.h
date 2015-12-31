//
//  MsgClientProtocol.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MsgClientProtocol_h
#define MsgClientIos_MsgClientProtocol_h

@protocol MsgClientProtocol <NSObject>

@required
- (void) OnLoginCode:(int) code;

@required
- (void) OnSndMsgMsg:(NSString*) msg;

@required
- (void) OnGetMsgFrom:(NSString*) from msg:(NSString*) msg;

@required
- (void) OnLogoutCode:(int) code;

@end

#endif
