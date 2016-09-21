//
//  MSMsgUtil.h
//  SdkMsgClientIos
//
//  Created by hp on 7/12/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSMsgUtil_h
#define MSMsgUtil_h
#import <Foundation/Foundation.h>
#import "msgclient/MSSubMessage.h"

@interface MSMsgUtil : NSObject

+ (NSString*)JSONToNSString:(id)obj;

+ (id)NSStringToJSONWithString:(NSString*)jstr;

/////////////////////////////////////////////////

+ (MSMessage*)EncodeMessageWithTxtMsg:(MSTxtMessage*)txtMsg
                                  msgType:(MCMsgType)msgType;

+ (MSMessage*)EncodeMessageWithLivMsg:(MSLivMessage*)livMsg
                                  msgType:(MCMsgType)msgType;

+ (MSMessage*)EncodeMessageWithRenMsg:(MSRenMessage*)renMsg
                                  msgType:(MCMsgType)msgType;

+ (MSMessage*)EncodeMessageWithBlkMsg:(MSBlkMessage*)blkMsg
                                  msgType:(MCMsgType)msgType;

+ (MSMessage*)EncodeMessageWithFbdMsg:(MSFbdMessage*)fbdMsg
                                  msgType:(MCMsgType)msgType;

+ (MSMessage*)EncodeMessageWithMgrMsg:(MSMgrMessage*)mgrMsg
                                  msgType:(MCMsgType)msgType;


+ (MSMessage*)DecodeDictToMessageWithDict:(NSDictionary*)dict;

@end

#endif /* MSMsgUtil_h */
