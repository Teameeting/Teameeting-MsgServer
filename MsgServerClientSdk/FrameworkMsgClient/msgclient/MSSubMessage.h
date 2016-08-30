//
//  MSSubMessage.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSSubMessage_h
#define MSSubMessage_h

#import <Foundation/Foundation.h>
#import "MSMessage.h"

@interface MSTxtMessage : NSObject{
   @private MSMessage *_message;
}
-(int) getPush;
-(void) setPush:(int)push;
-(NSString*) getContent;
-(void) setContent:(NSString*)content;
-(NSString*) getGroupId;
-(void) setGroupId:(NSString*)groupId;
-(NSString*) getToId;
-(void) setToId:(NSString*)toId;
-(NSString*) getToNickName;
-(void) setToNickName:(NSString*)toNickName;
@end

@interface MSLivMessage : NSObject{
   @private MSMessage *_message;
}
-(int) getPush;
-(void) setPush:(int)push;
-(int) getFlag;
-(void) setFlag:(int)flag;
-(NSString*) getGroupId;
-(void) setGroupId:(NSString*)groupId;
-(NSString*) getToId;
-(void) setToId:(NSString*)toId;
-(NSString*) getToNickName;
-(void) setToNickName:(NSString*)toNickName;
@end

@interface MSRenMessage : NSObject{
   @private MSMessage *_message;
}
-(int) getPush;
-(void) setPush:(int)push;
-(NSString*) getToId;
-(void) setToId:(NSString*)toId;
-(NSString*) getGroupId;
-(void) setGroupId:(NSString*)groupId;
-(NSString*) getCash;
-(void) setCash:(NSString*)cash;
-(NSString*) getWishcont;
-(void) setWishcont:(NSString*)wishcont;
-(NSString*) getToNickName;
-(void) setToNickName:(NSString*)toNickName;
@end

@interface MSBlkMessage : NSObject{
   @private MSMessage *_message;
}
-(int) getPush;
-(void) setPush:(int)push;
-(int) getFlag;
-(void) setFlag:(int)flag;
-(NSString*) getGroupId;
-(void) setGroupId:(NSString*)groupId;
-(NSString*) getToId;
-(void) setToId:(NSString*)toId;
-(NSString*) getToNickName;
-(void) setToNickName:(NSString*)toNickName;
@end

@interface MSFbdMessage : NSObject{
   @private MSMessage *_message;
}
-(int) getPush;
-(void) setPush:(int)push;
-(int) getFlag;
-(void) setFlag:(int)flag;
-(NSString*) getGroupId;
-(void) setGroupId:(NSString*)groupId;
-(NSString*) getToId;
-(void) setToId:(NSString*)toId;
-(NSString*) getToNickName;
-(void) setToNickName:(NSString*)toNickName;
@end

@interface MSMgrMessage : NSObject{
   @private MSMessage *_message;
}
-(int) getPush;
-(void) setPush:(int)push;
-(int) getFlag;
-(void) setFlag:(int)flag;
-(NSString*) getGroupId;
-(void) setGroupId:(NSString*)groupId;
-(NSString*) getToId;
-(void) setToId:(NSString*)toId;
-(NSString*) getToNickName;
-(void) setToNickName:(NSString*)toNickName;
@end

#endif /* MSSubMessage_h */
