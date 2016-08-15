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
#import "msgclient/MSTxtMessage.h"

@interface MSMsgUtil : NSObject

+ (NSString*)JSONToNSString:(id)obj;

+ (id)NSStringToJSONWithString:(NSString*)jstr;

/////////////////////////////////////////////////

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                  content:(NSString*)content
                                  msgType:(MCMsgType)msgType;

+ (MSTxtMessage*)EncodeMessageWithUserId:(NSString*)userId
                                 content:(NSString*)content
                                 msgType:(MCMsgType)msgType;

+ (MSTxtMessage*)EncodeMessageWithUserIds:(NSArray*)array
                                  content:(NSString*)content
                                  msgType:(MCMsgType)msgType;

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                   userId:(NSString*)userId
                                  msgType:(MCMsgType)msgType;

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                   userId:(NSString*)userId
                                     flag:(int)flag
                                  msgType:(MCMsgType)msgType;

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                   userId:(NSString*)userId
                                     cash:(NSString*)cash
                                     cont:(NSString*)cont
                                  msgType:(MCMsgType)msgType;

+ (MSTxtMessage*)DecodeDictToMessageWithDict:(NSDictionary*)dict;

@end

#endif /* MSMsgUtil_h */
