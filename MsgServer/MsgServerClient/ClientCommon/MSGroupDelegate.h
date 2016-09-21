//
//  MSGroupDelegate.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSGroupDelegate_h
#define MSGroupDelegate_h

class MSGroupDelegate {
public:
/**
 *  when add an group success this function will be called
 *
 *  params:
 *      grpId: the groupid you just added
 */
    virtual void OnAddGroupSuccess(const std::string& grpId) = 0;

/**
 *  when add an group failed this function will be called
 *
 *  params:
 *      grpId: the groupid you just add
 *      reason: the reason failed
 *      code: the code failed
 */
    virtual void OnAddGroupFailed(const std::string& grpId, const std::string& reason, int code) = 0;

/**
 *  when remove an group success this function will be called
 *
 *  params:
 *      grpId: the groupid you just remove
 */
    virtual void OnRmvGroupSuccess(const std::string& grpId) = 0;

/**
 *  when remove an group failed this function will be called
 *
 *  params:
 *      grpId: the groupid you just remove
 *      reason: the reason failed
 *      code: the code failed
 */
    virtual void OnRmvGroupFailed(const std::string& grpId, const std::string& reason, int code) = 0;

};

#endif /* MSGroupDelegate_h */
