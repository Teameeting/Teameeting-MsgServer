//
//  TMMsgSender.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_TMMsgSender_h
#define MsgClientIos_TMMsgSender_h
#import "MsgClientProtocol.h"

class MsgClient;

@interface TMMsgSender : NSObject {

@private
    MsgClient    *msgClient;
}

- (int) tMInitMsgProtocol:(id<MsgClientProtocol>)protocol
                   server:(NSString*) server
                    port :(int) port;
- (int) tMUint;

- (int) tMConnStatus;

- (int) tMLoginUserid:(NSString*) userid
                 pass:(NSString*) pass;

- (int) tMSndMsgUserid:(NSString*) userid
                  pass:(NSString*) pass
                roomid:(NSString*) roomid
                   msg:(NSString*) msg;

- (int) tMGetMsgUserid:(NSString*) userid
                  pass:(NSString*) pass;

- (int) tMLogoutUserid:(NSString*) userid
                  pass:(NSString*) pass;

- (int) tMCreateRoomUserid:(NSString*) userid
                      pass:(NSString*) pass
                    roomid:(NSString*) roomid;

- (int) tMEnterRoomUserid:(NSString*) userid
                     pass:(NSString*) pass
                   roomid:(NSString*) roomid;

- (int) tMLeaveRoomUserid:(NSString*) userid
                     pass:(NSString*) pass
                   roomid:(NSString*) roomid;

- (int) tMDestroyRoomUserid:(NSString*) userid
                       pass:(NSString*) pass
                     roomid:(NSString*) roomid;

- (int) tMSndMsgToUserid:(NSString*) userid
                    pass:(NSString*) pass
                  roomid:(NSString*) roomid
                     msg:(NSString*) msg
                   ulist:(NSArray*) ulist;

@end

#endif
