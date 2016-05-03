//
//  RTCurlClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTCurlClient.hpp"

RTCurlClient::RTCurlClient(const std::string& httpHost)
: mHttpHost(httpHost){

}

RTCurlClient::~RTCurlClient()
{

}

void RTCurlClient::RTUserInit(const std::string& userid, const std::string& uactype, const std::string& uregtype, const std::string& ulogindev, const std::string& upushtoken, std::string& resp)
{
    std::string path = mHttpHost + "/users/init";
    std::string data = "userid="+userid+"&uactype="+uactype+"&uregtype="+uregtype+"&ulogindev="+ulogindev+"&upushtoken="+upushtoken;
    RTCURL curl;
    curl.RTCurlPostUserInit(path, data, resp);
}

void RTCurlClient::RTApplyRoom(const std::string& sign, const std::string& meetingname, const std::string& meetingtype, const std::string& pushable, const std::string& meetenable, const std::string& meetdesc, std::string& resp)
{
    std::string path = mHttpHost + "/meeting/applyRoom";
    std::string data = "sign="+sign+"&meetingname="+meetingname+"&meetingtype="+meetingtype+"&pushable="+pushable+"&meetenable="+meetenable+"&meetdesc="+meetdesc;
    RTCURL curl;
    curl.RTCurlPostApplyRoom(path, data, resp);
}
