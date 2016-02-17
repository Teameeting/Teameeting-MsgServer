//
//  MsgClientProtocolImpl.h
//  MsgClientIos
//
//  Created by hp on 12/27/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MsgClientProtocolImpl_h
#define MsgClientIos_MsgClientProtocolImpl_h
#import "MsgClientProtocol.h"

@protocol ShowResultProtocol;

@interface MsgClientProtocolImpl : NSObject<MsgClientProtocol>


@property (nonatomic, assign) id<ShowResultProtocol> delegate;

@end


@protocol ShowResultProtocol <NSObject>

@required
-(void)resultDisplayCallback:(NSString*)backInfo
                        mMsg:(NSString*)mMsg
                        nInt:(int)nInt;

@end

#endif

