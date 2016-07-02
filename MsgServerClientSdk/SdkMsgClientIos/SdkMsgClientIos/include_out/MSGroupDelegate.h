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

// when add an group
-(void) OnAddGroupSuccessGrpId:(NSString*)grpId;

-(void) OnAddGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code;

// when del an group
-(void) OnRmvGroupSuccessGrpId:(NSString*)grpId;

-(void) OnRmvGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code;


@end

#endif /* MSGroupDelegate_h */
