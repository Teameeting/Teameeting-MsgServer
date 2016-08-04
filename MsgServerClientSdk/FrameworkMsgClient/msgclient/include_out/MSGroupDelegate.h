//
//  MSGroupDelegate.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSGroupDelegate_h
#define MSGroupDelegate_h

#import <Foundation/Foundation.h>


@protocol MSGroupDelegate <NSObject>

@optional

/**
 *  when add an group success this function will be called
 *
 *  params:
 *      grpId: the groupid you just added
 */
-(void) OnAddGroupSuccessGrpId:(NSString*)grpId;

/**
 *  when add an group failed this function will be called
 *
 *  params:
 *      grpId: the groupid you just add
 *      reason: the reason failed
 *      code: the code failed
 */
-(void) OnAddGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code;

/**
 *  when remove an group success this function will be called
 *
 *  params:
 *      grpId: the groupid you just remove
 */
-(void) OnRmvGroupSuccessGrpId:(NSString*)grpId;

/**
 *  when remove an group failed this function will be called
 *
 *  params:
 *      grpId: the groupid you just remove
 *      reason: the reason failed
 *      code: the code failed
 */
-(void) OnRmvGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code;


@end

#endif /* MSGroupDelegate_h */
