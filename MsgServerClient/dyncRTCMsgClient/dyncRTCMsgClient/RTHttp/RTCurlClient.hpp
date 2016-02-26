//
//  RTCurlClient.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTCurlClient_hpp
#define RTCurlClient_hpp

#include <stdio.h>
#include "RTCURL.hpp"

class RTCurlClient{
public:
    RTCurlClient(const std::string& httpHost);
    ~RTCurlClient();
    
public:
    void RTUserInit(const std::string& userid, const std::string& uactype, const std::string& uregtype, const std::string& ulogindev, const std::string& upushtoken, std::string& resp);
    void RTApplyRoom(const std::string& sign, const std::string& meetingname, const std::string& meetingtype, const std::string& pushable, const std::string& meetenable, const std::string& meetdesc, std::string& resp);
    
private:
    RTCURL       mCurl;
    std::string  mHttpHost;
};

#endif /* RTCurlClient_hpp */
