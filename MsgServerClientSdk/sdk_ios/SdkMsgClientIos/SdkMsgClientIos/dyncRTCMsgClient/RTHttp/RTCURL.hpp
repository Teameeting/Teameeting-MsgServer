//
//  RTCURL.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTCURL_hpp
#define RTCURL_hpp

#include <stdio.h>
#include <string>
#include <curl/curl.h>

class RTCURL{
public:
    RTCURL();
    ~RTCURL();
    
    void RTCurlPostUserInit(const std::string& strPath, const std::string& strData, std::string& strResp);
    static size_t UserInitCallback(void* buffer, size_t size, size_t nmem, void* stream);
    
    void RTCurlPostApplyRoom(const std::string& strPath, const std::string& strData, std::string& strResp);
    static size_t ApplyRoomCallback(void* buffer, size_t size, size_t nmem, void* stream);
private:
    CURL        *pCurl;
    CURLcode    curlCode;
};

#endif /* RTCURL_hpp */
