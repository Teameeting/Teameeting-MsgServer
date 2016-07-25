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
 *  when add an group success
 *
 *  params:
 */
-(void) OnAddGroupSuccessGrpId:(NSString*)grpId;

/**
 *  when add an group failed
 *
 *  params:
 */
-(void) OnAddGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code;

/**
 *  when remove an group success
 *
 *  params:
 */
-(void) OnRmvGroupSuccessGrpId:(NSString*)grpId;

/**
 *  when remove an group failed
 *
 *  params:
 */
-(void) OnRmvGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code;


@end

#endif /* MSGroupDelegate_h */
