//
//  CRTDispatchConnection.h
//  MsgServerConnector
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTDispatchConnection__
#define __MsgServerConnector__CRTDispatchConnection__

#include <stdio.h>
#include <iostream>
#include "ProtoCommon.h"

class CRTDispatchConnection{
public:
    CRTDispatchConnection(){}
    virtual ~CRTDispatchConnection(){}

    //void DispatchMsg(const std::string& uid, const std::string& msg, const std::string& handle_cmd, const std::string& handle_mtype, const std::string& handle_data);
    void DispatchMsg(const std::string& uid, pms::RelayMsg& r_msg);
private:

};

#endif /* defined(__MsgServerConnector__CRTDispatchConnection__) */
