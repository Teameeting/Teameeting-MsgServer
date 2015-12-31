//
//  RTObserverConnection.h
//  dyncRTConnector
//
//  Created by hp on 12/29/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef dyncRTConnector_RTObserverConnection_h
#define dyncRTConnector_RTObserverConnection_h

#include "RTObserver.h"

class RTObserverConnection : public RTObserver{
public:
    RTObserverConnection(){}
    virtual ~ RTObserverConnection(){}
    
    virtual void ConnectionDisconnected() = 0;
};

#endif
