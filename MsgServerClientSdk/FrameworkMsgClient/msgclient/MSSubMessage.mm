//
//  MSTxtMessage.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclient/MSSubMessage.h"

#pragma mark - MSTxtMessage

@implementation  MSTxtMessage
- (instancetype)init
{
    self = [super init];
    if (self) {
        _message = [[MSMessage alloc] init];
    }
    return self;
}

-(int) getPush
{
    return [_message push];
}

-(void) setPush:(int)push
{
    [_message setPush:push];
}

-(NSString*) getContent
{
    return [_message content];
}

-(void) setContent:(NSString*)content
{
    [_message setContent:content];
}

-(NSString*) getGroupId
{
    return [_message groupId];
}

-(void) setGroupId:(NSString*)groupId
{
    [_message setGroupId:groupId];
}

-(NSString*) getToId
{
    return [_message toId];
}

-(void) setToId:(NSString*)toId
{
    [_message setToId:toId];
}
@end

#pragma mark - MSLivMessage

@implementation MSLivMessage
- (instancetype)init
{
    self = [super init];
    if (self) {
        _message = [[MSMessage alloc] init];
    }
    return self;
}

-(int) getPush
{
    return [_message push];
}

-(void) setPush:(int)push
{
    [_message setPush:push];
}

-(int) getFlag
{
    return [_message flag];
}

-(void) setFlag:(int)flag
{
    [_message setFlag:flag];
}

-(NSString*) getGroupId
{
    return [_message groupId];
}

-(void) setGroupId:(NSString*)groupId
{
    [_message setGroupId:groupId];
}

-(NSString*) getToId
{
    return [_message toId];
}

-(void) setToId:(NSString*)toId
{
    [_message setToId:toId];
}
@end

#pragma mark - MSRenMessage

@implementation MSRenMessage
- (instancetype)init
{
    self = [super init];
    if (self) {
        _message = [[MSMessage alloc] init];
    }
    return self;
}

-(int) getPush
{
    return [_message push];
}

-(void) setPush:(int)push
{
    [_message setPush:push];
}

-(NSString*) getToId
{
    return [_message toId];
}

-(void) setToId:(NSString*)toId
{
    [_message setToId:toId];
}

-(NSString*) getGroupId
{
    return [_message groupId];
}

-(void) setGroupId:(NSString*)groupId
{
    [_message setGroupId:groupId];
}

-(NSString*) getCash
{
    return [_message cash];
}

-(void) setCash:(NSString*)cash
{
    [_message setCash:cash];
}

-(NSString*) getWishcont
{
    return [_message wishcont];
}

-(void) setWishcont:(NSString*)wishcont
{
    [_message setWishcont:wishcont];
}

@end

#pragma mark - MSBlkMessage

@implementation MSBlkMessage
- (instancetype)init
{
    self = [super init];
    if (self) {
        _message = [[MSMessage alloc] init];
    }
    return self;
}

-(int) getPush
{
    return [_message push];
}

-(void) setPush:(int)push
{
    [_message setPush:push];
}

-(int) getFlag
{
    return [_message flag];
}

-(void) setFlag:(int)flag
{
    [_message setFlag:flag];
}

-(NSString*) getGroupId
{
    return [_message groupId];
}

-(void) setGroupId:(NSString*)groupId
{
    [_message setGroupId:groupId];
}

-(NSString*) getToId
{
    return [_message toId];
}

-(void) setToId:(NSString*)toId
{
    [_message setToId:toId];
}
@end

#pragma mark - MSFbdMessage

@implementation MSFbdMessage
- (instancetype)init
{
    self = [super init];
    if (self) {
        _message = [[MSMessage alloc] init];
    }
    return self;
}

-(int) getPush
{
    return [_message push];
}

-(void) setPush:(int)push
{
    [_message setPush:push];
}

-(int) getFlag
{
    return [_message flag];
}

-(void) setFlag:(int)flag
{
    [_message setFlag:flag];
}

-(NSString*) getGroupId
{
    return [_message groupId];
}

-(void) setGroupId:(NSString*)groupId
{
    [_message setGroupId:groupId];
}

-(NSString*) getToId
{
    return [_message toId];
}

-(void) setToId:(NSString*)toId
{
    [_message setToId:toId];
}
@end

#pragma mark - MSMgrMessage

@implementation MSMgrMessage
- (instancetype)init
{
    self = [super init];
    if (self) {
        _message = [[MSMessage alloc] init];
    }
    return self;
}

-(int) getPush
{
    return [_message push];
}

-(void) setPush:(int)push
{
    [_message setPush:push];
}

-(int) getFlag
{
    return [_message flag];
}

-(void) setFlag:(int)flag
{
    [_message setFlag:flag];
}

-(NSString*) getGroupId
{
    return [_message groupId];
}

-(void) setGroupId:(NSString*)groupId
{
    [_message setGroupId:groupId];
}

-(NSString*) getToId
{
    return [_message toId];
}

-(void) setToId:(NSString*)toId
{
    [_message setToId:toId];
}
@end
