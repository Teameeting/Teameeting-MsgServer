//
//  MSGroupManager.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSGroupManager_h
#define MSGroupManager_h

#include <string>
#include "MSGroupDelegate.h"

class MSGroupManager{
public:
/**
 *  add group delegate
 *
 *  params:
 *      grpDelegate: the group delegate for callback to set
 *      grpQueue: the dispatch queue
 *
 *  return:
 *      void
 */
    void AddDelegate(const MSGroupDelegate* grpDelegate);

/**
 *  remove group delegate
 *
 *  params:
 *      grpDelegate: the group delegate to delete
 *
 *  return:
 *      void
 */
    void DelDelegate(const MSGroupDelegate* grpDelegate);

/**
 *  add to group
 *
 *  params:
 *      grpId: the group id to add
 *
 *  return:
 *      void
 */
    void AddGroup(const std::string& grpId);

/**
 *  remove from group
 *
 *  params:
 *      grpId: the group id to remove
 *
 *  return:
 *      void
 */
    void RmvGroup(const std::string& grpId);

};

#endif /* MSGroupManager_h */
